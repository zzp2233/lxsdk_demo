#coding:utf-8
#!/usr/bin/env python3

import os
import sys
import time
import struct
import argparse
import platform
import subprocess
import array
import hashlib
import shutil
import zipfile
import re
def get_ute_version(filename):
    with open(filename, 'r', errors='ignore') as file:
        content = file.read()

    match = re.search(r'#define\s+UTE_SW_VERSION\s+"(\w+)"', content)
    print("version:match",match)
    if match:
        ute_version = match.group(1)
        print("version:",ute_version)
    else:
        print("version is null")
    return ute_version
    
def charToNumber(s):
    s = s[::-1]
    num = 0
    for i, v in enumerate(s):
        offset = ord(v) - ord('0')
        num += offset * (10 ** i)
    return num

def getUteVersionNumber(version):
    num =0
    if 'V' in version:
        versionSplit =  version.split("V")
        print("versionSplit",versionSplit[1])
        # num = charToNumber(versionSplit[1])
        num = int(versionSplit[1])
        print("getUteVersionNumber:num=",num)
    return num

def get_ute_project(filename):
    with open(filename, 'r', errors='ignore') as file:
        content = file.read()

    match = re.search(r'#define\s+PROJECT_(\w+)_SUPPORT\s+1', content)
    print("name:match",match)
    if match:
        ute_name = match.group(1)
        #print("ute_name:",ute_name)
    else:
        ute_name = "default"
        #print("ute_name is default")
    return ute_name

def get_ute_custom_patch(filename):
    try:
        with open(filename, 'r', encoding='utf-8') as file:
            content = file.read()
    except Exception as e:
        print(f"Error: An error occurred while reading the file {filename}: {e}")
        return ""
    
    pattern = r'#define\s+UTE_UI_CONFIG_PATCH\s+"([^"]+)"'
    try:
        match = re.search(pattern, content)
    except re.error as e:
        print(f"Error: A regex error occurred: {e}")
        return ""
    
    if match:
        print("UTE_UI_CONFIG_PATCH macro found:", match.group(1))
        return match.group(1)
    else:
        print("UTE_UI_CONFIG_PATCH macro not found.")
        return ""

def panic(err_msg):
    print('Error: %s\n' %err_msg)
    sys.exit(1)

def main(argv):
    current_dir = os.getcwd()
    print('current_dir:',current_dir)
    index = current_dir.find('app')
    if (index==-1):
        panic("path is error")
    else:    
        config_file = current_dir[0:index]+"ute\\project_config\\ute_project_config.h"
        print('config_file:',config_file)
    project_name = get_ute_project(config_file)
    print('project_name:',project_name)
    custom_patch = get_ute_custom_patch(current_dir[0:index] + 'ute\\project_config\\' + 'ute_project_config_' + project_name.lower() + '.h')
    if custom_patch != '':
        project_dir = current_dir[0:index] + "ute\\ui_config_patch\\" + custom_patch
    else:
        project_dir = current_dir[0:index]+"ute\\ui_config_patch\\"+project_name
        if not os.path.exists(project_dir):
            project_dir = current_dir[0:index]+"ute\\ui_config_patch\\default"

    print('project_dir:',project_dir)
    
    if os.path.exists("Output\\bin\\res\\"):
       os.system('rmdir /s /q Output\\bin\\res\\')
    if os.path.exists("Output\\bin\\Settings\\"):
       os.system('rmdir /s /q Output\\bin\\Settings\\')
    if os.path.exists("Output\\bin\\ui\\"):
       os.system('rmdir /s /q Output\\bin\\ui\\')

    cpy_cmd = "xcopy /s/y/q "+project_dir+"\\app\\ ..\\..\\..\\app\\"
    print('cpy_cmd:',cpy_cmd)
    os.system(cpy_cmd)

    # language
    language_dir = os.path.join(project_dir, 'defaultLanguage')
    target_language_dir = project_dir = current_dir[0:index]+"ute\\language\\defaultLanguage"
    
    # 检查源目录是否存在
    if os.path.exists(language_dir):
        # 如果目标目录存在，先删除
        if os.path.exists(target_language_dir):
            shutil.rmtree(target_language_dir)
        # 复制源目录到目标目录
        shutil.copytree(language_dir, target_language_dir)
    #     print(f"成功将 {language_dir} 复制到 {target_language_dir}")
    # else:
    #     print(f"{language_dir} 不存在")

    #git copy
    git_copy_file = current_dir[0:index]+"ute\\tool\\AStyle\\pre-commit"
    git_copy_cmd = "copy /y "+git_copy_file+" ..\\..\\..\\.git\hooks"
    print('git_copy_cmd:',git_copy_cmd)
    os.system(git_copy_cmd)

    return 0

if __name__ == "__main__":
    main(sys.argv)
