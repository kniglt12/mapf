#!/bin/bash

# 如果任何命令失败，立即退出脚本
set -e
# 将未设置的变量视为错误
set -u

# --- 配置 ---
# 为文件路径和可执行文件定义变量，使其更易于修改。
EXECUTABLE="./build/lifelong"
PROBLEM_FILE="./example_problems/random.domain/random_32_32_20_100.json"
MAP_FILE="./example_problems/random.domain/maps/random-32-32-20.map"
PLAN_FILE="test.json"
VISUALIZER="PlanViz/script/run.py"

# --- 运行前检查 ---
# 确保所需的文件和可执行文件存在。
if [ ! -f "$EXECUTABLE" ]; then
    echo "错误: 在 $EXECUTABLE 未找到可执行文件。请先构建项目。"
    exit 1
fi

if [ ! -f "$PROBLEM_FILE" ]; then
    echo "错误: 在 $PROBLEM_FILE 未找到问题文件。"
    exit 1
fi

if [ ! -f "$MAP_FILE" ]; then
    echo "错误: 在 $MAP_FILE 未找到地图文件。"
    exit 1
fi

if [ ! -f "$VISUALIZER" ]; then
    echo "错误: 在 $VISUALIZER 未找到可视化脚本。"
    exit 1
fi

# --- 执行 ---
# 使用 C++ 程序生成计划。
echo "正在生成计划..."
"$EXECUTABLE" --inputFile "$PROBLEM_FILE" -o "$PLAN_FILE"

# 检查计划文件是否已成功创建。
if [ ! -f "$PLAN_FILE" ]; then
    echo "错误: 计划文件 $PLAN_FILE 未被创建。"
    exit 1
fi

# 使用 Python 脚本可视化计划。
echo "正在可视化计划..."
python3 "$VISUALIZER" --map "$MAP_FILE" --plan "$PLAN_FILE" --grid --aid --tid


echo "完成。"