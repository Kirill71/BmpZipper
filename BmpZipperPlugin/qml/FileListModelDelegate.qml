import QtQuick
import BmpZipperPlugin

Item {
    id: root

    property var dialogManager: null
    property var progressModel: null

    function updateColor(color) {
        _nameText.color = color;
        _sizeText.color = color;
    }

    height: 50
    width: ListView.view.width

    CompressionModel {
        id: _compressionModel
        progressModel: root.progressModel
    }

    Rectangle {
        id: _background
        anchors.fill: parent
        border.color: ColorMap.border_color
        border.width: 1
        color: ListView.isCurrentItem ? "#f0f0f0" : ColorMap.passive_state_color
        radius: 5
        scale: 1.0
        transformOrigin: Item.Center

        Behavior on scale {
            ScaleAnimator {
                duration: 200
                easing.type: Easing.OutQuad
            }
        }

        MouseArea {
            id: _mouseArea
            acceptedButtons: Qt.LeftButton
            anchors.fill: parent
            hoverEnabled: true
            drag.target: Item {}

            onClicked: {
                if (fileName.endsWith(".barch")) {
                    _compressionModel.decompress(filePath);
                } else if(fileName.endsWith(".bmp")) {
                    _compressionModel.compress(filePath);
                } else {
                    if (dialogManager === null) return;
                    dialogManager.showDialog({
                        title: "Message",
                        message: "Unsupported file format!",
                    });
                }
            }
            onEntered: {
                _background.color = ColorMap.selective_state_color;
                _background.scale = 1.1
            }
            onExited: {
                _background.color = ListView.isCurrentItem ? "#F0F0F0" : ColorMap.passive_state_color;
                updateColor(ColorMap.active_state_color);
                _background.scale = 1.0
            }
            onPressed: {
                _background.color = ColorMap.active_state_color;
                updateColor(ColorMap.passive_state_color);
            }
            onReleased: {
                _background.color = ColorMap.selective_state_color;
                updateColor(ColorMap.active_state_color);
            }
        }

        Row {
            id: _row
            anchors.centerIn: parent
            spacing: 10

            Text {
                id: _nameText
                color: ColorMap.active_state_color
                font.pixelSize: 16
                text: fileName
            }
            Rectangle {
                color: ColorMap.border_color
                height: _row.height
                width: 1
            }
            Text {
                id: _sizeText
                color: ColorMap.active_state_color
                font.pixelSize: 16
                text: fileSize + " kb"
            }
        }
    }

    Connections {
         target: progressModel
        function onProgressChanged(value) {
            if (value < progressModel.max) {
                _mouseArea.enabled = false;
            }
        }
    }

    Connections {
        target: _compressionModel
        function onErrorOccured(text) {
            if (dialogManager === null)
                return;

            dialogManager.showDialog({
                title: "Error",
                message: text
            });
        }
    }
}
