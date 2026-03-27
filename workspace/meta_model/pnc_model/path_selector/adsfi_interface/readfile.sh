#!/bin/bash

readfile() {
    local dir="$1"
    shopt -s nullglob  # 使 * 在空目录时不返回字面值
    for file in "$dir"/*; do
        if [ -d "$file" ]; then
            readfile "$file"
        else
            echo "Processing: $file"
            # 安全地重写文件（保留内容）
            if ! cat "$file" > "$file.tmp"; then
                echo "Error reading $file" >&2
                continue
            fi
            if ! mv "$file.tmp" "$file"; then
                echo "Error replacing $file" >&2
                rm -f "$file.tmp"
            fi
        fi
    done
}

if [ $# -eq 1 ]; then
    if [ -d "$1" ]; then
        echo "Unpacking folder: $1"
        readfile "$1"
    elif [ -f "$1" ]; then
        echo "Unpacking file: $1"
        cat "$1" > "$1.tmp" && mv "$1.tmp" "$1"
    else
        echo "Error: $1 is not a valid file or directory" >&2
        exit 1
    fi
else
    echo "Unpacking current folder"
    readfile "."
fi
