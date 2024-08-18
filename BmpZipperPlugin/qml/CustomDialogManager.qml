import QtQuick
import QtQuick.Controls

Item {
    id: root

    property var dialogConfig: ({})

    function showDialog(config) {
        dialogConfig = config;
        _dialogLoader.source = Qt.resolvedUrl("CustomDialog.qml");
        _dialogLoader.item.custom_title = dialogConfig.title || ""
        _dialogLoader.item.message = dialogConfig.message || ""
        _dialogLoader.item.okButtonText = dialogConfig.okButtonText || "OK"
        _dialogLoader.item.windowWidth = root.width
        _dialogLoader.item.windowHeight = root.height
        _dialogLoader.item.accepted.connect(dialogConfig.onAccepted || function (){})
        _dialogLoader.item.open()
    }

    Loader {
        id: _dialogLoader

        active: dialogConfig !== null

        onStatusChanged: {
            if (_dialogLoader.status === Loader.Ready) console.log('Loaded');
            if (_dialogLoader.status === Loader.Null) console.log('Null');
            if (_dialogLoader.status === Loader.Error) console.log('Error');
            if (_dialogLoader.status === Loader.Loading) console.log('Loading');
        }
    }
}