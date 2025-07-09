@echo off
set PATH=D:\msys64\mingw32\bin;%PATH%
g++ -s -o2 --static ./utils/tbl_conv.cpp -o ./utils/tbl_conv.exe
.\utils\tbl_conv.exe
move /y Hindi.py .\libs\Hindi.py
move /y Khmer.py .\libs\Khmer.py
del .\utils\tbl_conv.exe
pyinstaller -F -w -i logo.ico --version-file=ver.rc --add-data "./logo.ico;." --add-data "./config.yaml;." --add-data "./main.qml;." --add-data "./ToolTip.qml;." --add-data "./ToolTipArea.qml;." WordListTool.py
pause
