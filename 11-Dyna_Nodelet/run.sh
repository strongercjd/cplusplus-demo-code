#!/bin/bash

# 定义要检查的程序文件名
TARGET_FILE="plugin_loader"

export LD_LIBRARY_PATH="$(pwd)"

# 检查当前目录下是否存在 plugin_loader 文件
if [ ! -f "$TARGET_FILE" ]; then
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] 未找到 $TARGET_FILE，开始执行 make 构建..."
    
    # 执行 make 命令，并检查是否执行成功
    if make; then
        echo "[$(date +'%Y-%m-%d %H:%M:%S')] make 构建完成！"
        # 执行 plugin_loader 程序
        ./"$TARGET_FILE"
    else
        echo "[$(date +'%Y-%m-%d %H:%M:%S')] 错误：make 构建失败！"
        exit 1  # 构建失败时退出脚本，返回非0状态码
    fi
else
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] 找到 $TARGET_FILE，开始执行..."
    
    # 检查文件是否有可执行权限，没有则添加
    if [ ! -x "$TARGET_FILE" ]; then
        echo "[$(date +'%Y-%m-%d %H:%M:%S')] $TARGET_FILE 缺少可执行权限，正在添加..."
        chmod +x "$TARGET_FILE"
    fi
    
    # 执行 plugin_loader 程序
    ./"$TARGET_FILE"
    
    # 捕获程序执行的退出码，方便排查问题
    EXIT_CODE=$?
    echo "[$(date +'%Y-%m-%d %H:%M:%S')] $TARGET_FILE 执行完成，退出码：$EXIT_CODE"
fi

exit 0