#!/usr/bin/env python3
"""
扫描 CM 配置，生成 bag_topic_index.json，并自动修复 default_qos.xml。

key 值来源于 scfi_mapping.yaml 中的 portName（即 Callback name 参数的真实值），
不自行构造 key，确保与运行时 topic name 完全一致。

关联规则：scfi_mapping.yaml 和 dds.json 通过各自父目录的 processName 匹配
  scfi_mapping.yaml: <scfi_root>/.../<processName>/scfi_mapping.yaml
  dds.json:          <dds_root>/.../<processName>/dds.json

用法示例：
  python3 scan_cm_config.py \\
      --scfi-root /path/to/outputcfg \\
      --dds-root  /path/to/build/pack/packages \\
      -o bag_topic_index.json

  # 也可以直接指定单个文件对（用于精确控制）：
  python3 scan_cm_config.py \\
      --scfi /path/to/scfi_mapping.yaml \\
      --dds  /path/to/dds.json \\
      -o bag_topic_index.json

  # 扫描完成后，自动在对应 etc 目录的 default_qos.xml 中补全 datawriter profile：
  python3 scan_cm_config.py --scfi ... --dds ... --patch-qos -o bag_topic_index.json
"""

import argparse
import copy
import json
import re
import shutil
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

import yaml  # pip install pyyaml


# ---------------------------------------------------------------------------
# 自动修复 default_qos.xml：为每个 datareader 补全对应的 datawriter
#
# reader profile 命名规则：domain_{domainId}_{network}_{svcId}_{instId}_{topic}_reader
# writer profile 命名规则：domain_{domainId}_{network}_{svcId}_{instId}_{topic}_writer
#
# writer 结构与 reader 完全一致，仅：
#   - 标签名 datareader → datawriter
#   - profile name 后缀 _reader → _writer
#   - transchannel/ShmId 置为 0（写包不走进程间 SHM 通道）
# ---------------------------------------------------------------------------

# ElementTree 默认会修改 XML 格式，使用正则直接操作文本保留原始格式
def patch_qos_xml(qos_path: Path) -> int:
    """
    在 qos_path 指定的 XML 文件中，为每个 datareader profile 补全
    对应的 datawriter profile（若已存在则跳过）。
    返回新增的 writer profile 数量。
    修改前自动备份为 <file>.bak（若 .bak 已存在则不覆盖）。
    """
    text = qos_path.read_text(encoding="utf-8")

    # 找出所有已存在的 writer profile 名，避免重复添加
    existing_writers = set(re.findall(
        r'<datawriter\s+name="([^"]+)"', text))

    # 找出所有 reader profile 块：<datareader name="...">...</datareader>
    reader_pattern = re.compile(
        r'(<datareader\s+name="([^"]+_reader)"[^>]*>)(.*?)(</datareader>)',
        re.DOTALL)

    new_blocks = []
    for m in reader_pattern.finditer(text):
        reader_name = m.group(2)                          # e.g. ...LocationDdsEvent_reader
        writer_name = reader_name[:-len("_reader")] + "_writer"

        if writer_name in existing_writers:
            continue  # 已存在，跳过

        # 构造 writer 块：替换标签名和 profile 名，ShmId 置 0
        inner = m.group(3)
        inner = re.sub(r'<ShmId>\d+</ShmId>', '<ShmId>0</ShmId>', inner)

        writer_block = (
            f'        <datawriter name="{writer_name}">'
            + inner
            + '</datawriter>'
        )
        new_blocks.append((reader_name, writer_name, writer_block))
        existing_writers.add(writer_name)

    if not new_blocks:
        return 0

    # 备份
    bak = qos_path.with_suffix(qos_path.suffix + ".bak")
    if not bak.exists():
        shutil.copy2(qos_path, bak)

    # 在 </dds_profile_list> 或文件末尾的 </datareader_list> / </qos_profile_list>
    # 找到最后一个 </datareader> 之后插入所有 writer 块
    insert_after = re.compile(r'(</datareader>)(\s*)(</\w+>)', re.DOTALL)
    last_match = None
    for last_match in re.finditer(r'</datareader>', text):
        pass  # 找最后一个

    if last_match is None:
        print(f"[WARN] {qos_path.name}: 未找到 </datareader> 插入点", file=sys.stderr)
        return 0

    insert_pos = last_match.end()
    insertion = "\n" + "\n".join(b for _, _, b in new_blocks)
    text = text[:insert_pos] + insertion + text[insert_pos:]

    qos_path.write_text(text, encoding="utf-8")
    return len(new_blocks)


# ---------------------------------------------------------------------------
# 解析 scfi_mapping.yaml
# 返回: { serviceName: [(instanceId:int, portName:str), ...] }
# ---------------------------------------------------------------------------
def parse_scfi(scfi_path: Path) -> dict:
    with open(scfi_path, encoding="utf-8") as f:
        data = yaml.safe_load(f)

    result = {}

    # 查找任意深度的 recvInstanceID 节点
    def _find_recv(node):
        if not isinstance(node, dict):
            return None
        if "recvInstanceID" in node:
            return node["recvInstanceID"]
        for v in node.values():
            found = _find_recv(v)
            if found is not None:
                return found
        return None

    recv = _find_recv(data)
    if recv is None:
        return result

    for svc_name, svc_data in recv.items():
        if not isinstance(svc_data, dict):
            continue
        instance_ids = svc_data.get("instanceId", [])
        port_names   = svc_data.get("portName", [])
        if len(instance_ids) != len(port_names):
            print(f"[WARN] {scfi_path.name}: {svc_name} "
                  f"instanceId/portName 长度不匹配，跳过", file=sys.stderr)
            continue
        result[svc_name] = list(zip(
            [int(i) for i in instance_ids],
            port_names
        ))
    return result


# ---------------------------------------------------------------------------
# 解析 dds.json
# 返回: { serviceShortName: { instanceId:int -> record } }
# record 含: serviceName, eventName, topicName, domainId, vlan, shortName
# ---------------------------------------------------------------------------
def parse_dds(dds_path: Path) -> dict:
    with open(dds_path, encoding="utf-8") as f:
        data = json.load(f)

    etc_path = str(dds_path.parent)
    result = {}

    for svc in data.get("services", []):
        service_name  = svc["service"]
        short_name_svc = service_name.rstrip("/").split("/")[-1]

        # eventId -> event 元信息
        event_map = {e["id"]: e for e in svc.get("events", [])}

        inst_map = {}
        for inst in svc.get("instances", []):
            iid        = int(inst["instance"])
            short_name = inst.get("shortName", "")
            domain_id  = inst.get("domainId", 0)
            vlan       = inst.get("vlan", "")

            # 取第一个（通常也是唯一一个）event
            inst_events = inst.get("events", [])
            if not inst_events:
                continue
            ev = event_map.get(inst_events[0]["id"])
            if ev is None:
                continue

            inst_map[iid] = {
                "serviceName": service_name,
                "eventName":   ev["event"],
                "topicName":   ev.get("topicName", ""),
                "domainId":    domain_id,
                "vlan":        vlan,
                "shortName":   short_name,
                "etcPath":     etc_path,
            }

        if inst_map:
            result[short_name_svc] = inst_map

    return result


# ---------------------------------------------------------------------------
# 合并一对 (scfi, dds) → list of records
# ---------------------------------------------------------------------------
def merge(scfi_map: dict, dds_map: dict) -> list:
    records = []
    for svc_name, pairs in scfi_map.items():
        inst_map = dds_map.get(svc_name)
        if inst_map is None:
            print(f"[SKIP] service '{svc_name}' 不在 dds.json 中", file=sys.stderr)
            continue
        for (iid, port_name) in pairs:
            rec = inst_map.get(iid)
            if rec is None:
                print(f"[SKIP] {svc_name} instanceId={iid} 不在 dds.json 中",
                      file=sys.stderr)
                continue
            records.append({
                "key":         port_name,       # portName 即 Callback name 参数
                "serviceName": rec["serviceName"],
                "eventName":   rec["eventName"],
                "instanceId":  iid,
                "shortName":   rec["shortName"],
                "domainId":    rec["domainId"],
                "vlan":        rec["vlan"],
                "topicName":   rec["topicName"],
                "etcPath":     rec["etcPath"],
            })
    return records


# ---------------------------------------------------------------------------
# main
# ---------------------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(
        description="扫描 CM 配置，用 scfi_mapping.yaml 的 portName 作为 key")

    grp_root = parser.add_argument_group("目录扫描模式（自动按 processName 匹配）")
    grp_root.add_argument("--scfi-root", metavar="DIR",
                          help="scfi_mapping.yaml 搜索根目录")
    grp_root.add_argument("--dds-root", metavar="DIR",
                          help="dds.json 搜索根目录")

    grp_file = parser.add_argument_group("单文件模式（精确指定单对文件）")
    grp_file.add_argument("--scfi", metavar="FILE",
                          help="单个 scfi_mapping.yaml 路径")
    grp_file.add_argument("--dds", metavar="FILE",
                          help="单个 dds.json 路径")

    parser.add_argument("-o", "--output", default="bag_topic_index.json",
                        help="输出文件路径（默认 bag_topic_index.json）")
    parser.add_argument("--section", default="recvInstanceID",
                        help="yaml 中要使用的 instanceID 节（默认 recvInstanceID）")
    parser.add_argument("--patch-qos", action="store_true",
                        help="自动在 etcPath 下的 default_qos.xml 中补全 datawriter profile")
    args = parser.parse_args()

    # 收集 (scfi_path, dds_path) 对
    pairs = []

    if args.scfi and args.dds:
        pairs.append((Path(args.scfi), Path(args.dds)))

    if args.scfi_root and args.dds_root:
        # 按 processName（父目录名）自动匹配
        scfi_files = {p.parent.name: p
                      for p in Path(args.scfi_root).rglob("scfi_mapping.yaml")}
        dds_files  = {p.parent.name: p
                      for p in Path(args.dds_root).rglob("dds.json")}

        matched = set(scfi_files) & set(dds_files)
        unmatched_scfi = set(scfi_files) - matched
        if unmatched_scfi:
            print(f"[WARN] 以下 process 有 scfi_mapping.yaml 但无对应 dds.json: "
                  f"{sorted(unmatched_scfi)}", file=sys.stderr)

        for pname in sorted(matched):
            pairs.append((scfi_files[pname], dds_files[pname]))
        print(f"[INFO] 匹配到 {len(pairs)} 对 (scfi, dds)")

    if not pairs:
        parser.print_help()
        sys.exit(1)

    all_records = []
    seen_keys   = {}   # key -> 首次来源，用于重复检测

    for scfi_path, dds_path in pairs:
        print(f"[SCAN] {scfi_path.parent.name}")
        print(f"       scfi: {scfi_path}")
        print(f"       dds:  {dds_path}")

        try:
            scfi_map = parse_scfi(scfi_path)
        except Exception as e:
            print(f"[WARN] 解析 scfi 失败: {e}", file=sys.stderr)
            continue

        try:
            dds_map = parse_dds(dds_path)
        except Exception as e:
            print(f"[WARN] 解析 dds 失败: {e}", file=sys.stderr)
            continue

        records = merge(scfi_map, dds_map)
        for r in records:
            k = r["key"]
            if k in seen_keys:
                print(f"[WARN] key='{k}' 重复（首次来自 {seen_keys[k]}），跳过",
                      file=sys.stderr)
                continue
            seen_keys[k] = scfi_path.parent.name
            all_records.append(r)

    with open(args.output, "w", encoding="utf-8") as f:
        json.dump(all_records, f, ensure_ascii=False, indent=2)

    print(f"[DONE] 共 {len(all_records)} 条记录 -> {args.output}")

    # --patch-qos：为每个涉及的 etcPath 下的 default_qos.xml 补全 datawriter
    if args.patch_qos:
        etc_paths = sorted({r["etcPath"] for r in all_records})
        total_added = 0
        for etc in etc_paths:
            qos_file = Path(etc) / "default_qos.xml"
            if not qos_file.exists():
                print(f"[WARN] 未找到 {qos_file}，跳过", file=sys.stderr)
                continue
            added = patch_qos_xml(qos_file)
            if added > 0:
                print(f"[QOS]  {qos_file.parent.name}/default_qos.xml "
                      f"新增 {added} 个 datawriter profile")
            else:
                print(f"[QOS]  {qos_file.parent.name}/default_qos.xml 无需修改")
            total_added += added
        print(f"[QOS]  共新增 {total_added} 个 datawriter profile")


if __name__ == "__main__":
    main()
