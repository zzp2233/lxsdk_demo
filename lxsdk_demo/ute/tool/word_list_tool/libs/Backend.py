# This Python file uses the following encoding: utf-8
from PyQt5.QtCore import QObject, pyqtSignal, pyqtSlot, Qt, QProcess

import json
import shutil
import yaml
import os
from os import path, remove
from pathlib import Path
from queue import Queue, Empty
from libs import AsyncWorker as Asw


class Backend(QObject):
    progress_changed = pyqtSignal(float)
    showLog = pyqtSignal(str)  # 定义一个带 str 参数的信号

    def __init__(self):
        self.progress = 0.0
        self.progressStep = 0.0
        self.thread = None
        self.basePath = ""
        self.explorer = QProcess()
        self.clearLog = False
        self.sendMsg = False
        self.queue = Queue()
        self.message = ""
        self.threadmt = []
        super().__init__()

    def open_explorer(self, path):
        x = str(path)
        print(f"open_explorer {path}")
        self.explorer.start('explorer.exe', [x])

    @pyqtSlot(str)
    def send_message(self, msg):
        self.queue.put(msg)
        self.sendMsg = True

    @pyqtSlot(str)
    def log(self, input_string):
        print("Print():", input_string)

    @pyqtSlot(int, int, result=int)
    def add(self, a, b):
        return a + b

    @pyqtSlot(str, bool)
    def checkBox(self, idc, checked):
        print("{1} {2}", idc, checked)

    @pyqtSlot(float)
    def on_progress(self, progress):
        print("Received message:", progress)
        if progress > 1.0:
            progress = 1.0
            self.send_message("\n---本次任务已经完成，请自行校验数据正确性。---\n")
            f = Path(self.basePath).absolute().parent
            output_dir = f.joinpath("output")
            self.open_explorer(output_dir)

        self.progress = progress

    @pyqtSlot(str)
    def procWorkFlow(self, json_data):
        self.clearLog = True
        # 反序列化JSON字符串为Python字典
        data = json.loads(json_data, parse_int=int, parse_float=float)
        # 在控制台输出Python字典对象
        print("path:{1} genDct:{2}", data["base"]["path"])

        lang_list = data["lang"]
        lang_enable_list = []
        for lang in lang_list:
            languageID = lang["name"].split()[0].strip()
            if lang["enable"]:
                lang_enable_list.append(lang)
            print(lang["xid"], lang["name"], "Y" if lang["enable"] else "N", languageID)

        if not Path(data["base"]["path"]).exists():
            self.send_message("data file '{}' not exist.".format(data["base"]["path"]))
            return

        if len(lang_enable_list) <= 0:
            self.send_message("任务列表为空。\n")
            return

        self.basePath = data["base"]["path"]

        print(f"langList[input]: {lang_enable_list}")
        self.thread = Asw.AsyncWorker(data["base"]["path"], lang_enable_list)
        print("connect ASW.AsyncWorker signal.")
        self.thread.step.connect(self.on_progress, type=Qt.ConnectionType.QueuedConnection)
        self.thread.showLog.connect(self.send_message, type=Qt.ConnectionType.QueuedConnection)

        outPathBase = Path(self.basePath).absolute().parent
        output_dir = outPathBase.joinpath("output")
        print(f"Clear {output_dir}")
        try:
            shutil.rmtree(output_dir)
            self.send_message(f"成功删除目录 {output_dir} 及其子目录下所有文件")
        except OSError as e:
            self.send_message(f"[error] {output_dir} - {e.strerror}")
            print(f"[error] {output_dir} - {e.strerror}")

        print("Start ASW.AsyncWorker thread.")
        self.thread.start()

    @pyqtSlot(result=list)
    def on_tempConfig(self):
        # 读取 YAML 文件
        try:
            if path.exists('temp.yaml'):
                yaml_cfg = 'temp.yaml'
                with open(yaml_cfg, 'r', encoding='utf-8') as file:
                    data = yaml.safe_load(file)
                remove(yaml_cfg)
                return data
            else:
                raise IOError
        except IOError as e:
            errInfo = str(e)
            print("[error] Load yaml config err:", errInfo)
            self.send_message(f"[error] Load config.yaml err: {errInfo}")

    @pyqtSlot(result=list)
    def on_getConfig(self):
        # 读取 YAML 文件
        try:
            yaml_cfg = 'config.yaml'
            if path.exists('custom.yaml'):
                yaml_cfg = 'custom.yaml'
            else:
                if not path.exists('config.yaml'):
                    # 使用相对路径，基于当前文件位置获取config.yaml的路径
                    script_dir = os.path.dirname(os.path.realpath(__file__))
                    parent_dir = os.path.dirname(script_dir)
                    shutil.copy(os.path.join(parent_dir, 'config.yaml'), os.path.join(os.getcwd(), "config.yaml"))
            print(f"Load yaml config: {yaml_cfg}")
            with open(yaml_cfg, 'r', encoding='utf-8') as file:
                data = yaml.safe_load(file)
            return data
        except IOError as e:
            errInfo = str(e)
            print("[error] Load yaml config err:", errInfo)
            self.send_message(f"[error] Load config.yaml err: {errInfo}")
            return []

    @pyqtSlot(result=float)
    def getProgressValue(self):
        return self.progress

    @pyqtSlot(result=bool)
    def getLogCommand(self):
        return self.clearLog

    @pyqtSlot(bool)
    def setLogCommand(self, state):
        self.clearLog = state

    @pyqtSlot(result=str)
    def getLogMessage(self):
        if self.sendMsg is True:
            try:
                if not self.message:
                    self.message = str(self.queue.get_nowait())
                else:
                    self.message += "\n" + str(self.queue.get_nowait())
            except Empty:
                pass
            return self.message
        else:
            return "None"

    @pyqtSlot(bool)
    def setLogMessage(self, state):
        self.sendMsg = state
        self.message = ""
