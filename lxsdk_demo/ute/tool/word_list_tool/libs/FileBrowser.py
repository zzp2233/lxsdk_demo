# This Python file uses the following encoding: utf-8
from PyQt5.QtWidgets import QFileDialog
from PyQt5.QtCore import QObject, pyqtSlot


class FileBrowser(QObject):
    def __init__(self, parent=None):
        super().__init__(parent)

    @pyqtSlot(result=str)
    def opendialog(self):
        dialog = QFileDialog()
        dialog.setFileMode(QFileDialog.ExistingFile)
        if dialog.exec_() == QFileDialog.Accepted:
            selectedFiles = dialog.selectedFiles()
            if selectedFiles:
                return selectedFiles[0]
        return ""

    @pyqtSlot(result=str)
    def opendirdialog(self):
        dialog = QFileDialog()
        dialog.setFileMode(QFileDialog.Directory)
        if dialog.exec_() == QFileDialog.Accepted:
            selectedFiles = dialog.selectedFiles()
            if selectedFiles:
                return selectedFiles[0]
        return ""

    @pyqtSlot(result=str)
    def select_headerfile(self):
        dialog = QFileDialog()
        filename = dialog.getOpenFileName(None, "选择项目多国语配置头文件", "./ute_multi_language_config.h",
                                          "头文件 (*.h)")
        if filename:
            return filename[0]
        return ""
