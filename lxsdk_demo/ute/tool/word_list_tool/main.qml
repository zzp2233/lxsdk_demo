import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 1.5
import QtQuick.Layouts 1.3

Window {
    id: main
    width: 1030
    height: 720
    minimumWidth: 1030 // 设置最小宽度
    minimumHeight: 640 // 设置最小高度
    visible: true
    title: qsTr("词条生成工具 V2.2.1")

    function elableGuiInput(bEnable) {
        flowLanguages.enabled = bEnable;
        buttonGen.enabled = bEnable;
        // checkBoxGenDct.enabled = bEnable;
        // checkBoxSavePsRam.enabled = bEnable;
        // checkBox3rdKhmer.enabled = bEnable;
        textInputPath.enabled = bEnable;
        if (bEnable) {
            updateUi_Timer.start();
        }
        else {
            updateUi_Timer.stop();
        }
    }

    function onProgressChanged(progress)
    {
        progressBar.value = progress
        if (progress >= 1.0)
        {
            elableGuiInput(true)
        }
    }

    function onCheckLog()
    {
        if (backend.getLogCommand() === true) {
            info.clear()
            backend.setLogCommand(false)
        }
        if (backend.getLogMessage() !== "None") {
            var str_msg = backend.getLogMessage()
            if (str_msg.indexOf("\n") >= 0){
                var str_tbl = str_msg.split("\n");
                for (var i = 0;i < str_tbl.length;i++)
                {
                   info.append(str_tbl[i] + "\n");
                }
            } else {
                info.append(str_msg + "\n");
            }
            backend.setLogMessage(false);
        }
    }

    Component.onCompleted: {
        var vCwd = tools.getExePath();
        console.log("Cwd: [" + vCwd + "]")
        textInputPath.text = tools.pathJoin(vCwd, "input.xlsx")
        updateUi_Timer.start();
        updateUi_Timer_Progress.start();
        updateUi_Timer_Log.start();
    }

    Timer {
        id: updateUi_Timer
        interval: 600
        repeat: true
        running: false
        onTriggered:
        {
            var iEnableCount = 0;
            for (var i = 0; i < checkbox_languageModel.count; i++) {
                if (checkbox_languageModel.get(i).checked)
                    iEnableCount++;
            }
            buttonGen.enabled = (iEnableCount > 0);
            // checkBoxGenDct.enabled = (iEnableCount > 0);
            // checkBoxSavePsRam.enabled = (iEnableCount > 0);
            // checkBox3rdKhmer.enabled = (iEnableCount > 0);
        }
    }

    Timer {
        id: updateUi_Timer_Progress
        interval: 600
        repeat: true
        running: false
        onTriggered:
        {
            onProgressChanged(backend.getProgressValue());
        }
    }

    Timer {
        id: updateUi_Timer_Log
        interval: 600
        repeat: true
        running: false
        onTriggered:
        {
            onCheckLog();
        }
    }

    Rectangle {
        id: rectangle
        anchors.fill: parent
        property int heightParent: parent.height * 0.4

        Column {
            spacing: 0
            width: parent.width
            height: parent.height
            Rectangle {
                width: parent.width
                height: parent.height * 0.05

                Label {
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    id: label
                    text: qsTr("input.xlsx 文件路径:")
                }

                Rectangle {
                    x: 162
                    width: parent.width - 232
                    height: parent.height
                    border.color: "lightblue"

                    TextInput {
                        verticalAlignment: TextInput.AlignVCenter
                        id: textInputPath
                        anchors.fill: parent
                        anchors.margins: 2
                        focus: false
                    }
                }

                Button {
                    width: parent.height
                    height: parent.height
                    anchors.rightMargin: 4
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    id: buttonBrowser
                    text: qsTr("浏览")
                    onClicked: {
                        textInputPath.text = fileBrower.opendialog();
                    }
                }
            }
            Rectangle {
                width: parent.width
                height: parent.height * 0.05
                // Row
                // {
                //     anchors.horizontalCenter: parent.horizontalCenter
                //     anchors.verticalCenter: parent.verticalCenter
                //     spacing: 40
                //     CheckBox {
                //         id: checkBoxGenDct
                //         text: qsTr("是否输出dct格式词条数据？")
                //         checked: true
                //         onClicked:
                //         {
                //             backend.checkBox("checkBoxGenDct", checked)
                //         }
                //     }
                //     CheckBox {
                //         id: checkBoxSavePsRam
                //         text: qsTr("减少PsRam占用？")
                //         checked: true
                //         onClicked:
                //         {
                //             backend.checkBox("checkBoxSavePsRam", checked)
                //         }
                //         ToolTipArea {
                //             showDelay: 500
                //             hideDelay: 500
                //             tip {
                //                 text: "选中后将尽可能减少对PsRam的占用。\n如果影响到编译，请取消选中后再试。"
                //                 x: checkBoxSavePsRam.width + 10
                //                 y: checkBoxSavePsRam.height/2 - tip.height/2
                //             }
                //         }
                //     }
                //     CheckBox {
                //         id: checkBox3rdKhmer
                //         text: qsTr("使用第三方高棉语字库？")
                //         checked: true
                //         onClicked:
                //         {
                //             backend.checkBox("checkBox3rdKhmer", checked)
                //         }
                //         ToolTipArea {
                //             showDelay: 500
                //             hideDelay: 500
                //             tip {
                //                 text: "使用第三方高棉语字库时必须勾选此项，\n否则生成的词条解码会有问题，\n如果使用UTE内部的高棉语字库则不勾选此项。"
                //                 x: checkBox3rdKhmer.width + 10
                //                 y: checkBox3rdKhmer.height/2 - tip.height/2
                //             }
                //         }
                //     }
                // }
            }
            Rectangle {
                id: lang_area
                width: parent.width
                height: parent.height * 0.5

                Column {
                    spacing: 10
                    anchors.fill: parent
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 40

                        Button {
                            id: buttonAutocfgUnselectAll
                            text: qsTr("全部取消选择")
                            onClicked: {
                                autocfg.unselect_all();
                                var data = backend.on_tempConfig()
                                console.log(data)
                                checkbox_languageModel.clear()
                                checkbox_languageModel.append(data)
                            }
                        }

                        Button {
                            id: buttonAutocfgByFilename
                            text: qsTr("自动选择(基于项目多国语文件名)")
                            onClicked: {
                                var dir = fileBrower.opendirdialog();
                                if (dir !== "") {
                                    autocfg.by_filename(dir);
                                    var data = backend.on_getConfig()
                                    console.log(data)
                                    checkbox_languageModel.clear()
                                    checkbox_languageModel.append(data)
                                }
                            }
                        }

                        Button {
                            id: buttonAutocfgByHeader
                            text: qsTr("自动选择(基于项目多国语头文件)")
                            onClicked: {
                                var header = fileBrower.select_headerfile();
                                if (header !== "") {
                                    autocfg.by_header(header);
                                    var data = backend.on_getConfig()
                                    console.log(data)
                                    checkbox_languageModel.clear()
                                    checkbox_languageModel.append(data)
                                }
                            }
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: lang_area.height * 0.75
                        color: "lightyellow"

                        ListModel {
                            id: checkbox_languageModel
                        }

                        Flow {
                            id: flowLanguages
                            x: 1
                            y: 20
                            width: parent.width
                            height: lang_area.height * 0.75
                            spacing: 3

                            Repeater {
                                model: checkbox_languageModel
                                CheckBox {
                                    checked: model.checked
                                    width: 255
                                    text: model.text
                                    id: checkboxitem
                                    onClicked: {
                                        model.checked = !model.checked
                                    }
                                }
                            }
                        }

                        Component.onCompleted: {
                            var data = backend.on_getConfig()
                            console.log(data)
                            checkbox_languageModel.append(data)
                        }
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        id: buttonGen
                        text: qsTr("生成词条文件")
                        onClicked: {
                            const data = {
                              base: {
                                path: textInputPath.text,
                                // genDct: checkBoxGenDct.checked,
                                // savePsRam: checkBoxSavePsRam.checked,
                                // khmer3rd: checkBox3rdKhmer.checked
                              },
                              lang: [
                              ]
                            };

                            console.log("Path:" + textInputPath.text)
                            var iEnableCount = 0;
                            var i = 0;
                            for (; i < checkbox_languageModel.count; i++) {
                                var obj = { xid: checkbox_languageModel.get(i).xid, name: checkbox_languageModel.get(i).text, enable : checkbox_languageModel.get(i).checked }
                                data.lang.push(obj)

                                if (checkbox_languageModel.get(i).checked)
                                    iEnableCount++;
                            }

                            if (iEnableCount < 1)
                            {
                                return;
                            }

                            elableGuiInput(false);

                            // 转换为 JSON
                            // 增加可读性
                            const jsonData = JSON.stringify(data, null, 2);
                            console.log(jsonData);

                            // 传递给后台处理
                            const jsonDataTrans = JSON.stringify(data);
                            console.log("Call backend.procWorkFlow")
                            backend.procWorkFlow(jsonDataTrans)
                            console.log("Call backend.procWorkFlow end.")
                        }
                    }
                }
            }
            Column {
                width: parent.width
                height: parent.height * 0.4
                spacing: 0

                Rectangle {
                    width: parent.width
                    height: parent.height * 0.95
                    border.width: 1
                    border.color: "lightblue"

                    TextArea {
                        id: info
                        Layout.preferredWidth:parent.width
                        Layout.fillHeight:  true
                        readOnly:           true
                        textFormat:         TextEdit.RichText
                        text:               ""
                        width: parent.width
                        height: parent.height
                        anchors.fill: parent
                        function clear()
                        {
                            info.cursorPosition = 0
                            info.text = ""
                        }
                        function append(strAdd)
                        {
                            info.text = info.text + strAdd
                            info.cursorPosition = info.length - 1
                        }
                    }
                }
                Rectangle {
                    color: "green"
                    width: parent.width
                    height: parent.height * 0.05
                    ProgressBar {
                        id: progressBar
                        anchors.fill: parent
                        value: 0.0
                    }
                }
            }
        }
    }
}
