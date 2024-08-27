import QtQuick
import QtQuick.Controls
import BmpZipperPlugin

Item {
    id: _mainWindow

    width: 640
    height: 480

    CustomDialogManager {
        id: _customDialogManager

        width: _mainWindow.width
        height: _mainWindow.height
    }

    ProgressModel {
        id: _customProgressModel
    }

    ListView {
        id: _listView

        readonly property real scaleRatio: 0.9

        width: _mainWindow.width * scaleRatio
        height: _mainWindow.height * scaleRatio
        ScrollBar.vertical: ScrollBar { }

        anchors.horizontalCenter: parent.horizontalCenter

        model: FileListModel {
            folder: initialFolder
        }

        delegate: FileListModelDelegate {
            dialogManager: _customDialogManager
            progressModel: _customProgressModel
        }
    }

    CustomProgressBar {
        id: _progressBar

        visible: false
        from: _customProgressModel.min
        to: _customProgressModel.max
        progressText: _customProgressModel.text
    }

    Connections {
        target: _customProgressModel
        function onProgressChanged(value) {
            _progressBar.visible = true;
            if (value < _progressBar.to - 1) {
                _progressBar.value = value;
            }
            else {
                _progressBar.visible = false;
                _progressBar.value = 1;
                _customDialogManager.showDialog({
                    title: "Message",
                    message: "Operation Succeed"
                });
            }
        }
    }
}

