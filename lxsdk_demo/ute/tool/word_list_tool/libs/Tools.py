# This Python file uses the following encoding: utf-8
from pathlib import Path
import sys
from PyQt5.QtCore import QObject, pyqtSlot


class Tools(QObject):
    def __init__(self):
        super().__init__()

    @pyqtSlot(result=str)
    def getExePath(self):
        if getattr(sys, 'frozen', False):
            # 打包后的可执行文件
            sCwd = Path(sys.executable).absolute().parent.joinpath('work')
            return str(sCwd)
        else:
            # 未打包的脚本文件
            sCwd = Path(sys.argv[0]).absolute().parent.joinpath('work')
            return str(sCwd)

    @pyqtSlot(str, str, result=str)
    def pathJoin(self, s1, s2):
        print(f"----------{s1} -> {s2}")
        sRet = Path(s1).joinpath(s2)
        return str(sRet)
