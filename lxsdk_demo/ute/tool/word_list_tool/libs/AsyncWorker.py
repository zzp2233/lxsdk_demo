# This Python file uses the following encoding: utf-8
from PyQt5.QtCore import QThread, pyqtSignal
from libs import ConvertExcel2CHeader


class AsyncWorker(QThread):
    step = pyqtSignal(float)
    showLog = pyqtSignal(str)

    def __init__(self, xls, langlist):
        self.xls = xls
        self.langList = langlist
        # self.genDct = False
        # self.savePsRam = False
        # self.khmer3rd = False
        self.progress = 0.0
        self.progressStep = 0 if len(self.langList) <= 0 else 1.0 / len(self.langList)
        super().__init__()

    def run(self):
        self.progress = 0.0
        for lang in self.langList:
            languageIDX = lang["xid"]
            languageID = lang["name"].split()[0].strip()
            languageDes = lang["name"].split()[1].strip()
            self.progress += self.progressStep
            if not self.make_word_list(self.xls, languageIDX, languageID, languageDes, self.langList):
                return

            self.showLog.emit("proc: {} end.".format({lang["name"]}))
            self.step.emit(self.progress)
            print(ConvertExcel2CHeader.lastErrInfo)

        self.progress = 1.1
        self.step.emit(self.progress)

    def make_word_list(self, xls, xid, language_id, language_des, langList):
        # self.showLog.emit(f"Begin make_word_list {ConvertExcel2CHeader}")
        if not ConvertExcel2CHeader.CheckExcelFormat(xls):
            self.showLog.emit(
                "[error] input.xlsx 文件格式不正确！请确保与压缩包内 input.xlsx 格式一致【不要改变表结构及表头，仅按列替换单元格数据内容】！")
            return False

        print("AsyncWorker Begin make_word_list")
        ConvertExcel2CHeader.MainProc(xls, xid, language_id, language_des, langList)
        print("AsyncWorker End make_word_list")
        return True
