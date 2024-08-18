import QtQuick
import QtQuick.Controls
import BmpZipperPlugin

ProgressBar {
    id: root

    value: 1
    padding: 2

    property var progressText: ""

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 15

    background: Rectangle {
        implicitWidth: 200
        implicitHeight: 6
        color: ColorMap.selective_state_color
        radius: 3
    }

    contentItem: Item {
        id: _contentItem
        implicitWidth: 200
        implicitHeight: 4

        Rectangle {
            width: root.visualPosition * parent.width
            height: parent.height
            radius: 2
            color: ColorMap.active_state_color
            visible: !root.indeterminate
        }
        Item {
            anchors.fill: parent
            visible: root.indeterminate
            clip: true

            Row {
                spacing: 20
                Repeater {
                    model: root.width / 40 + 1

                    Rectangle {
                        color: ColorMap.active_state_color
                        width: 20
                        height: root.height
                    }
                }
                XAnimator on x {
                    from: 0
                    to: -40
                    loops: Animation.Infinite
                    running: root.indeterminate
                }
            }
        }

        Text {
            id: messageLabel

            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            text: progressText
            color: ColorMap.active_state_color
            wrapMode: Text.WordWrap
            anchors.top: _contentItem.bottom
        }
    }
}