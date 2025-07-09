import os
import shutil
import subprocess
import glob
import re
from datetime import datetime
import xml.etree.ElementTree as ET
import zipfile

# 定义固定内容（sys.cfg 的内容）
SYS_CFG_CONTENT = b"DUF-UIAB"

sdk_name = 'AB5691_SDK'
config_name = 'project_config'

def update_watch_setting(file_path, replacements):
    tree = ET.parse(file_path)
    root = tree.getroot()
    for add_element in root.findall(".//add"):
        key = add_element.get("key")
        if key in replacements:
            add_element.set("value", replacements[key])
    tree.write(file_path, encoding="utf-8", xml_declaration=True)

def execute_downloader(downloader_path, app_dcf_path, watch_setting_path, key_file, output_path, output_name):
    cmd = [
        downloader_path,
        "-b",
        "-o",
        os.path.join(output_path, output_name),
        "-i",
        app_dcf_path,
        "-s",
        watch_setting_path,
        "-k",
        key_file
    ]
    try:
        subprocess.check_call(cmd, cwd=os.path.dirname(downloader_path))
    except subprocess.CalledProcessError:
        print(f"错误: 执行 Downloader.exe 生成 {output_name} 失败。")
        return 1
    return 0

def find_project_config(script_dir, sdk_name, config_name):
    current_dir = script_dir
    while current_dir != os.path.dirname(current_dir):
        sdk_path = os.path.join(current_dir, sdk_name)
        if os.path.exists(sdk_path):
            config_path = os.path.join(sdk_path, 'ute', config_name)
            if os.path.exists(config_path):
                return config_path
        current_dir = os.path.dirname(current_dir)
    return None

def get_ute_info(filename, pattern):
    with open(filename, 'r', errors='ignore') as file:
        content = file.read()
    match = re.search(pattern, content)
    return match.group(1) if match else ""

def get_current_time_formatted():
    return datetime.now().strftime("%Y%m%d")

def clear_output_directory(output_dir):
    if os.path.exists(output_dir):
        for filename in os.listdir(output_dir):
            file_path = os.path.join(output_dir, filename)
            try:
                if os.path.isfile(file_path) or os.path.islink(file_path):
                    os.unlink(file_path)
                elif os.path.isdir(file_path):
                    shutil.rmtree(file_path)
            except Exception as e:
                print(f'Failed to delete {file_path}. Reason: {e}')

def ensure_directory_exists(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

def copy_file(src, dst):
    try:
        shutil.copy2(src, dst)
    except FileNotFoundError as e:
        print(f"错误: 复制文件失败 - {e}")
        return False
    return True

def remove_file(file_path):
    try:
        os.remove(file_path)
    except OSError as e:
        print(f"错误: 删除文件失败 - {e}")
        return False
    return True

def compress_files_to_zip(files, output_zip_path):
    with zipfile.ZipFile(output_zip_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for file in files:
            if os.path.exists(file):
                zipf.write(file, os.path.basename(file))
            else:
                print(f"警告: 文件 {file} 不存在，未添加到压缩文件中。")

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(script_dir, "Output")
    downloader_dir = os.path.join(script_dir, "Downloader")
    downloader_path = os.path.join(downloader_dir, "Downloader.exe")
    app_dcf_path = os.path.join(script_dir, "app.dcf")
    watch_setting_path = os.path.join(script_dir, "Settings", "watch.setting")
    settings_target = os.path.join(downloader_dir, "Settings")
    key_dir = os.path.join(downloader_dir, "key")

    project_config_path = find_project_config(script_dir, sdk_name, config_name)
    if not project_config_path:
        print("项目配置文件未找到")
        return 1

    project_config = os.path.join(project_config_path, 'ute_project_config.h')
    project_name = get_ute_info(project_config, r'#define\s+PROJECT_(\w+)_SUPPORT\s+1')
    custom_patch = os.path.join(project_config_path, f'ute_project_config_{project_name.lower()}.h')
    print("project_name:", project_name)
    print("custom_patch:", custom_patch)
    project_version = get_ute_info(custom_patch, r'#define\s+UTE_SW_VERSION\s+"(\w+)"')
    if not project_version:
        print("版本号未找到")
        project_version = ''

    code_fot_name = f"{project_version}_OTA_CODE_{get_current_time_formatted()}.fot"
    code_prd_name = f"{project_version}_MP_{get_current_time_formatted()}.prd"
    firmware_name = f"{project_version}_OTA_UI_CODE_{get_current_time_formatted()}.bin"
    code_fot_path = os.path.join(output_dir, code_fot_name)
    dfu_ui_code_path = os.path.join(output_dir, firmware_name)

    # 创建或清空 Output 目录
    clear_output_directory(output_dir)
    ensure_directory_exists(output_dir)

    # 检查关键文件和目录是否存在
    files_and_dirs = {
        downloader_path: "Downloader.exe",
        app_dcf_path: "app.dcf 文件",
        watch_setting_path: "watch.setting 文件",
        os.path.join(script_dir, "Settings"): "Settings 目录",
        key_dir: "key 目录"
    }
    for path, description in files_and_dirs.items():
        if not os.path.exists(path):
            print(f"错误: {description}不存在。")
            return 1

    key_files = glob.glob(os.path.join(key_dir, "*.key"))
    if not key_files:
        print("错误: key 目录下没有 .key 文件。")
        return 1
    elif len(key_files) > 1:
        print("错误: key 目录下有多个 .key 文件。请确保只存在一个。")
        return 1

    if os.path.exists(settings_target):
        shutil.rmtree(settings_target, ignore_errors=True)

    try:
        shutil.copytree(os.path.join(script_dir, "Settings"), settings_target)
    except FileExistsError:
        print("错误: Settings 目录复制失败（目标目录可能未完全删除）")
        return 1
    
    modified_watch_setting_path = os.path.join(settings_target, "watch.setting")
    if os.path.exists(modified_watch_setting_path):
        update_watch_setting(modified_watch_setting_path, {
            "bt_addr_mode": "单次递增",
            "bt_addr": "78:02:B7:00:00:01",
            "bt_addr_start": "78:02:B7:00:00:01",
            "bt_addr_step": "00:00:00:00:00:01",
            "bt_addr_end": "78:02:B7:FF:FF:FF"
        })
    else:
        print("错误: 复制后的 watch.setting 文件不存在。")
        return 1

    if execute_downloader(downloader_path, app_dcf_path, modified_watch_setting_path, key_files[0], output_dir, code_fot_name) != 0:
        return 1

    print("成功生成:", code_fot_name)

    ui_bin_path = os.path.join(script_dir, "ui.bin")
    if not os.path.exists(ui_bin_path) or not os.path.exists(code_fot_path):
        print("错误: ui.bin 或 code.fot 文件不存在。")
        return 1

    ui_size_cfg_path = os.path.join(output_dir, "ui_size.cfg")
    try:
        with open(ui_size_cfg_path, "w") as f:
            f.write(str(os.path.getsize(ui_bin_path)) + "\n")
    except OSError as e:
        print(f"错误: 无法创建 ui_size.cfg 文件 - {e}")
        return 1

    try:
        with open(dfu_ui_code_path, "wb") as outfile:
            outfile.write(SYS_CFG_CONTENT)
            with open(ui_size_cfg_path, "rb") as infile:
                shutil.copyfileobj(infile, outfile)
            with open(ui_bin_path, "rb") as infile:
                shutil.copyfileobj(infile, outfile)
            with open(code_fot_path, "rb") as infile:
                shutil.copyfileobj(infile, outfile)
    except OSError as e:
        print(f"错误: 文件合并失败 - {e}")
        return 1
    
    print("成功生成:", firmware_name)

    os.remove(ui_size_cfg_path)

    if execute_downloader(downloader_path, app_dcf_path, modified_watch_setting_path, key_files[0], output_dir, code_prd_name) != 0:
        return 1

    print("成功生成:", code_prd_name)

    if os.path.exists(settings_target):
        shutil.rmtree(settings_target, ignore_errors=True)

    # 压缩 app.lst 和 map.txt 到 log.zip
    files_to_compress = [
        os.path.join(script_dir, "app.lst"),
        os.path.join(script_dir, "map.txt")
    ]
    output_zip_path = os.path.join(output_dir, "log.zip")
    compress_files_to_zip(files_to_compress, output_zip_path)
    print("成功生成:", output_zip_path)

    return 0

if __name__ == "__main__":
    exit(main())