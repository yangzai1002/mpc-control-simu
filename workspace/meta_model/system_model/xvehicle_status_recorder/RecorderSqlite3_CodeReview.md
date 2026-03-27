# RecorderSqlite3.hpp 代码审查报告

## 🔴 严重问题 (Critical Issues)

### 1. **线程安全：析构时访问队列无保护** (Line 628-633)
```cpp
// Flush remaining records
while (!_record_queue.empty()) {  // ⚠️ 没有持有 _queue_mutex！
    RecordBatch batch = std::move(_record_queue.front());
    _record_queue.pop();
    write_batch(batch);
}
```
**问题**: 析构函数中访问 `_record_queue` 时没有加锁，可能导致数据竞争。
**影响**: 程序崩溃或数据损坏
**修复**:
```cpp
std::unique_lock<std::mutex> lock(_queue_mutex);
while (!_record_queue.empty()) {
    RecordBatch batch = std::move(_record_queue.front());
    _record_queue.pop();
    lock.unlock();  // 释放锁后再写入
    write_batch(batch);
    lock.lock();
}
```

---

### 2. **SQL 注入风险** (Line 568, 582, 695)
```cpp
std::string check_sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + table_name + "'";
```
**问题**: 表名直接拼接到 SQL，如果表名包含 `'; DROP TABLE--` 会导致 SQL 注入
**影响**: 安全漏洞，可能破坏数据库
**修复**: 使用参数化查询或转义表名
```cpp
// 方案1: 使用白名单验证
bool is_valid_table_name(const std::string& name) {
    return std::regex_match(name, std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
}

// 方案2: 转义特殊字符
std::string escape_identifier(const std::string& name) {
    return "\"" + std::regex_replace(name, std::regex("\""), "\"\"") + "\"";
}
```

---

### 3. **const_cast 未定义行为风险** (Line 50, 71)
```cpp
const_cast<T&>(obj).enumerate_internal(collector);
```
**问题**: 对真正的 const 对象使用 const_cast 会导致未定义行为
**影响**: 潜在的内存损坏或崩溃
**修复**:
```cpp
// 方案1: 要求 enumerate_internal 有 const 重载
template <typename T>
static void flatten(const std::string& prefix, const T& obj, std::map<std::string, std::string>& fields) {
    FieldCollector collector(prefix, fields);
    obj.enumerate_internal(collector);  // 调用 const 版本
}

// 方案2: 接受非 const 引用
template <typename T>
static void flatten(const std::string& prefix, T& obj, std::map<std::string, std::string>& fields) {
    // ...
}
```

---

### 4. **record_id 重复插入冲突** (Line 436)
```cpp
fields["record_id"] = std::to_string(_db_manager->next_record_id());
```
**问题**: 手动设置 record_id，但表定义使用 `AUTOINCREMENT` (Line 583)，会导致冲突
**影响**: 插入失败
**修复**: 移除手动设置，让数据库自动生成
```cpp
// 删除这行
// fields["record_id"] = std::to_string(_db_manager->next_record_id());

// 在 create_table 时也移除 AUTOINCREMENT，使用普通 INTEGER PRIMARY KEY
create_sql << "record_id INTEGER PRIMARY KEY";  // SQLite 会自动递增
```

---

### 5. **数据库句柄生命周期问题** (Line 219-223)
```cpp
sqlite3* get_db() {
    std::lock_guard<std::mutex> lock(_mutex);
    rotate_if_needed();  // ⚠️ 可能关闭 _current_db 并创建新的
    return _current_db;  // 返回裸指针，调用者使用时可能已失效
}
```
**问题**: 多线程环境下，调用者持有 db 指针时，另一个线程可能触发轮转关闭数据库
**影响**: 使用已释放的指针，导致崩溃
**修复**:
```cpp
// 方案1: 返回时延长锁的生命周期（不推荐）
// 方案2: 在 write_batch 中重新获取 db（推荐）
void write_batch(const RecordBatch& batch) {
    sqlite3* db = _db_manager->get_db();  // 每次操作重新获取
    // ... 使用 db
}
```

---

## 🟡 重要问题 (Important Issues)

### 6. **队列无界增长导致 OOM**
**问题**: `_record_queue` 没有大小限制，如果写入速度慢于记录速度，队列会无限增长
**影响**: 内存耗尽
**修复**: 添加队列大小限制
```cpp
// 在 record() 方法中
std::lock_guard<std::mutex> lock(_queue_mutex);
if (_record_queue.size() >= MAX_QUEUE_SIZE) {
    AERROR << "Queue full, dropping record";
    return;  // 或阻塞等待
}
```

---

### 7. **SQLite 资源未正确释放** (Line 232, 570)
```cpp
// close_current_db
sqlite3_close(_current_db);  // ⚠️ 应使用 sqlite3_close_v2

// create_table_if_not_exists
int rc = sqlite3_prepare_v2(db, check_sql.c_str(), -1, &stmt, nullptr);
if (rc == SQLITE_OK) {
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);  // ⚠️ rc != SQLITE_OK 时未 finalize
}
```
**修复**:
```cpp
// 使用 RAII 或确保所有路径都 finalize
sqlite3_close_v2(_current_db);  // 等待所有语句完成

// 或者
if (rc == SQLITE_OK) {
    rc = sqlite3_step(stmt);
}
sqlite3_finalize(stmt);  // 无论如何都 finalize
```

---

### 8. **flush() 可能导致死锁** (Line 650)
```cpp
void flush_timer() {
    // ...
    flush();  // flush() 需要 _queue_mutex
}

void flush() {
    std::unique_lock<std::mutex> lock(_queue_mutex);  // 获取队列锁
    // ...
    _cv.notify_one();  // 唤醒 writer
}
```
**问题**: 如果 writer 线程在 `write_batch` 中调用 `get_db()`（需要 RotatingDbManager 的 mutex），而同时 flush_timer 持有 `_queue_mutex` 等待队列清空，可能形成死锁
**修复**: 使用超时机制或重新设计锁顺序

---

### 9. **表结构不一致** (Line 587)
```cpp
if (field_name != "record_id" && !field_value.empty()) {
    create_sql << ", " << field_name << " TEXT";
}
```
**问题**: 如果第一条记录的某个 vector 为空（被忽略），后续记录该 vector 不为空，表中缺少该字段导致插入失败
**影响**: 数据丢失
**修复**: 使用 ALTER TABLE 动态添加列，或预定义完整表结构

---

### 10. **时间戳转换魔术数字** (Line 310-313)
```cpp
std::time_t created = std::filesystem::last_write_time(entry.path()).time_since_epoch().count() / 10000000 - 11644473600LL;
```
**问题**:
- 硬编码的魔术数字不易理解
- 可能因平台而异
- last_write_time 返回的是 file_time_type，不是 system_clock，转换不正确

**修复**:
```cpp
auto ftime = std::filesystem::last_write_time(entry.path());
auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
    ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
);
std::time_t created = std::chrono::system_clock::to_time_t(sctp);
```

---

## 🟢 次要问题 (Minor Issues)

### 11. **缺少移动/拷贝控制**
```cpp
class RecorderSqlite3 {
    // 应该添加
public:
    RecorderSqlite3(const RecorderSqlite3&) = delete;
    RecorderSqlite3& operator=(const RecorderSqlite3&) = delete;
    RecorderSqlite3(RecorderSqlite3&&) = delete;
    RecorderSqlite3& operator=(RecorderSqlite3&&) = delete;
};
```

---

### 12. **性能：每次插入都构建 SQL** (Line 694-714)
**问题**: 每次插入都动态构建 SQL 语句，应该缓存预编译语句
**修复**:
```cpp
std::map<std::string, sqlite3_stmt*> _prepared_stmts;  // 缓存预编译语句
```

---

### 13. **错误处理不一致**
- Line 594: CREATE TABLE 失败只记录日志
- Line 283: 数据库打开失败抛出异常
- 应该统一错误处理策略

---

### 14. **WAL 模式配置不完整** (Line 288)
```cpp
// 应该添加更多配置
sqlite3_exec(db, "PRAGMA journal_mode=WAL", ...);
sqlite3_exec(db, "PRAGMA synchronous=NORMAL", ...);  // 提高性能
sqlite3_exec(db, "PRAGMA wal_autocheckpoint=1000", ...);  // 自动检查点
```

---

### 15. **字段名未转义**
字段名如果包含空格或 SQL 关键字会导致语法错误：
```cpp
create_sql << ", \"" << field_name << "\" TEXT";  // 添加引号
```

---

### 16. **异常安全问题**
- Line 204-206: `create_directories` 可能抛出异常
- Line 399-411: 析构函数中 `join()` 可能抛出异常
```cpp
~RecorderSqlite3() noexcept {
    try {
        _running = false;
        // ...
    } catch (...) {
        // 吞掉异常，避免析构函数抛出
    }
}
```

---

## 📊 优先级建议

| 优先级 | 问题编号 | 建议处理时间 |
|--------|---------|-------------|
| P0 (立即修复) | 1, 2, 3, 4, 5 | 发布前必须修复 |
| P1 (尽快修复) | 6, 7, 8, 9 | 下个版本修复 |
| P2 (计划修复) | 10-16 | 后续迭代优化 |

---

## 🔧 快速修复清单

1. ✅ 为 async_writer 退出路径添加互斥锁
2. ✅ 添加表名白名单验证，防止 SQL 注入
3. ✅ 移除 const_cast，使用 const 正确的方法
4. ✅ 移除手动设置 record_id
5. ✅ 修改 sqlite3_close 为 sqlite3_close_v2
6. ✅ 添加队列大小限制（建议 1000-10000）
7. ✅ 添加拷贝/移动构造函数的删除声明
8. ✅ 修复时间戳转换
9. ✅ 为字段名添加引号转义
10. ✅ 在析构函数中捕获异常

---

生成时间: 2026-02-04
审查人: Claude Code
