pragma Singleton

import QtQuick

QtObject {
    id: root

    readonly property color active_state_color: "#000000"
    readonly property color selective_state_color: "#E0E0E0"
    readonly property color passive_state_color: "#FFFFFF"
    readonly property color border_color: "grey"
}