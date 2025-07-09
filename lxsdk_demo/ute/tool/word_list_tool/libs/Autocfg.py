import os
import re
import yaml

from PyQt5.QtCore import QObject, pyqtSlot

yaml_file = 'config.yaml'
output_yaml = 'custom.yaml'
temp_yaml = 'temp.yaml'
map_yaml = 'map.yaml'

# 定义宏名称与原来语言ID的映射字典
language_id_map = {
    'CHINESE': 'zh_rcn',
    'ENGLISH': 'en_rus',
    'KOREAN': 'ko_rkr',
    'JAPANESE': 'ja_rjp',
    'GERMAN': 'de_rde',
    'SPANISH': 'es_res',
    'FRENCH': 'fr_rfr',
    'ITALIAN': 'it_rit',
    'PORTUGUESE': 'pt_rpt',
    'ARABIC': 'ar_ril',
    'INDIA': 'hi_rin',
    'HINDI': 'hi_rin',
    'POLISH': 'pl_rpl',
    'RUSSIAN': 'ru_rru',
    'DUTCH': 'nl_rnl',
    'TURKISH': 'tr_rtr',
    'BENGALI': 'bn_rbn',
    'URDU': 'ur_rur',
    'INDONESUAN': 'id_rid',
    'PUNJABI': 'pa_rpa',
    'THAI': 'th_rth',
    'CZECK': 'cs_rcs',
    'TRADITIONAL_CHINESE': 'zh_rtw',
    'ROMANIAN': 'ro_rro',
    'VIETNAMESE': 'vi_rvn',
    'MALAYSIA': 'ms_rmy',
    'GREEK': 'el',
}

def process_yaml(yaml_data, folder_path):
    for entry in yaml_data:
        text_parts = entry['text'].split()
        region_code = text_parts[0].lower()  # 将 region_code 转为小写

        file_name = f"ute_multi_language_{region_code}.c"

        if not os.path.exists(os.path.join(folder_path, file_name)):
            entry['checked'] = False


def process_yaml_alt(yaml_data, language_code, number):
    for entry in yaml_data:
        text_parts = entry['text'].split()
        region_code = text_parts[0].lower()

        if region_code == language_code:
            entry['checked'] = int(number) == 1


def process_yaml_uncheck(yaml_data):
    for entry in yaml_data:
        entry['checked'] = False


class Autocfg(QObject):
    def __init__(self, parent=None):
        super().__init__(parent)

    @pyqtSlot(str)
    def by_filename(self, folder_path):
        with open(yaml_file, 'r', encoding='utf-8') as file:
            yaml_data = yaml.safe_load(file)

        process_yaml(yaml_data, folder_path)

        with open(output_yaml, 'w', encoding='utf-8') as file:
            yaml.dump(yaml_data, file, default_flow_style=False)

    @pyqtSlot(str)
    def by_header(self, header_file):
        with open(header_file, 'r', encoding='utf-8') as file:
            config_data = file.readlines()

        with open(yaml_file, 'r', encoding='utf-8') as file:
            yaml_data = yaml.safe_load(file)

        # 检查配置文件是否存在，如果不存在则生成
        if not os.path.exists(map_yaml):
            print(f"Creating {map_yaml}...")  # 添加调试信息
            with open(map_yaml, 'w', encoding='utf-8') as file:
                print(f"Writing to {map_yaml}: {language_id_map}")  # 添加调试信息
                yaml.dump(language_id_map, file, default_flow_style=False)
            print(f"{map_yaml} created and written.")  # 添加调试信息

        # 读取配置文件
        with open(map_yaml, 'r', encoding='utf-8') as file:
            language_id_mapping = yaml.safe_load(file)

        if language_id_mapping == None:
            language_id_mapping = language_id_map

        pattern = re.compile(r'#define SCREEN_TITLE_MULTIPLE_(\w+)_LANGUAGE_SUPPORT\s+(\d+)')

        process_yaml_uncheck(yaml_data)
        for line in config_data:
            match = pattern.match(line)
            if match:
                language_code, number = match.group(1).upper(), match.group(2)  # 使用大写匹配字典中的键
                original_language_id = language_id_mapping.get(language_code)
                if original_language_id:
                    process_yaml_alt(yaml_data, original_language_id, number)

        with open(output_yaml, 'w', encoding='utf-8') as file:
            yaml.dump(yaml_data, file, default_flow_style=False)

    @pyqtSlot()
    def unselect_all(self):
        with open(yaml_file, 'r', encoding='utf-8') as file:
            yaml_data = yaml.safe_load(file)

        process_yaml_uncheck(yaml_data)

        with open(temp_yaml, 'w', encoding='utf-8') as file:
            yaml.dump(yaml_data, file, default_flow_style=False)
