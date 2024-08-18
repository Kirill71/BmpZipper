import QtQuick
import QtQuick.Controls.Basic
import BmpZipperPlugin

Button {
    id: root

    required property var controlCallback

    anchors.horizontalCenter: parent.horizontalCenter

    contentItem: Text {
        id: _controlText
        text: root.text
        font: root.font
        opacity: enabled ? 1.0 : 0.3
        color: ColorMap.active_state_color
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        id: _controlBackground
        anchors.fill: parent
        implicitWidth: 100
        implicitHeight: 40
        color: ColorMap.passive_state_color
        opacity: enabled ? 1 : 0.3
        border.color: ColorMap.border_color
        border.width: 1
        radius: 10
    }

    Timer {
        id: _delayTimer
        interval: 200
        repeat: false

        onTriggered: {
            if (controlCallback) {
                controlCallback();
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onPressed: {
            _controlBackground.color = ColorMap.active_state_color
            _controlText.color = ColorMap.passive_state_color
            _delayTimer.start();
        }
        onReleased: {
            _controlBackground.color = ColorMap.selective_state_color
            _controlText.color = ColorMap.active_state_color
        }
        onEntered: {
            _controlBackground.color = ColorMap.selective_state_color
        }
        onExited: {
            _controlBackground.color = ColorMap.passive_state_color
        }
    }
}