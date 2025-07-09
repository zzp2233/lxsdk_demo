# This Python file uses the following encoding: utf-8
import sys
import os

from PyQt5.QtCore import QUrl
from PyQt5.QtQml import QQmlApplicationEngine
from PyQt5.QtWidgets import QApplication
from PyQt5.QtGui import QIcon
from libs import Backend
from libs import Tools
from libs import FileBrowser
from libs import Autocfg


def is_debugging():
    return sys.gettrace() is not None


if __name__ == "__main__" or __name__ == "WordListTool":
    app = QApplication(sys.argv)
    engine = QQmlApplicationEngine()
    icon = os.path.join(os.path.dirname(os.path.realpath(__file__)), 'logo.ico')
    app.setWindowIcon(QIcon(icon))

    # 将Python对象注册为QML上下文属性
    backend = Backend.Backend()
    tools = Tools.Tools()
    fileBrower = FileBrowser.FileBrowser()
    autocfg = Autocfg.Autocfg()
    engine.rootContext().setContextProperty("backend", backend)
    engine.rootContext().setContextProperty("tools", tools)
    engine.rootContext().setContextProperty("fileBrower", fileBrower)
    engine.rootContext().setContextProperty("autocfg", autocfg)

    qml_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'main.qml')
    QQmlApplicationEngine.addImportPath(engine, os.path.join(os.path.dirname(os.path.abspath(__file__)), "PyQt5\Qt5\qml"))
    engine.load(QUrl.fromLocalFile(qml_file))

    if not is_debugging():
        if not engine.rootContext():
            sys.exit(-1)
        sys.exit(app.exec())

    else:
        app.exec()
