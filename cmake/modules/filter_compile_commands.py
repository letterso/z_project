#!/usr/bin/env python3
"""
过滤compile_commands.json，只保留项目源代码的编译单元，排除第三方库
"""
import json
import sys
import os


def filter_compile_commands(input_file, output_file, source_dir):
    """Filter compile_commands.json to only include project source files"""
    with open(input_file, 'r', encoding='utf-8') as f:
        commands = json.load(f)
    
    # 只保留src目录和app目录下的文件
    # 排除thirdparty、build/_deps等第三方库目录
    filtered = []
    src_dir = os.path.join(source_dir, 'src')
    app_dir = os.path.join(source_dir, 'app')
    exclude_patterns = ['thirdparty', '_deps', 'external', 'third_party', 'build']
    
    for cmd in commands:
        file_path = cmd.get('file', '')
        # 规范化路径以进行比较
        file_path_normalized = os.path.normpath(file_path)
        
        # 检查是否在src或app目录中
        in_src = file_path_normalized.startswith(os.path.normpath(src_dir))
        in_app = file_path_normalized.startswith(os.path.normpath(app_dir))
        
        # 检查是否包含排除的模式
        exclude = any(pattern in file_path_normalized for pattern in exclude_patterns)
        
        if (in_src or in_app) and not exclude:
            filtered.append(cmd)
    
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(filtered, f, indent=2)
    
    print(f'Filtered {len(commands)} -> {len(filtered)} compilation units')
    return len(filtered)


if __name__ == '__main__':
    if len(sys.argv) != 4:
        print('Usage: filter_compile_commands.py <input> <output> <source_dir>')
        sys.exit(1)
    
    count = filter_compile_commands(sys.argv[1], sys.argv[2], sys.argv[3])
    sys.exit(0 if count > 0 else 1)
