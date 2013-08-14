import QtQuick 2.0

Item {
    id: cardWindow

    property variant windowContainer
    property Item cardView

    visible: false

    onVisibleChanged: if( windowContainer && visible ) windowContainer.firstCardDisplayDone = true;

    Component.onCompleted: {
        windowContainer.cardViewParent = cardWindow;
    }
}
