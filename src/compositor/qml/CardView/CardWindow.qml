import QtQuick 2.0
import LunaNext 0.1

Item {
    id: cardWindow

    property Item windowContainer
    property Item cardView

    visible: false

    onVisibleChanged: if( windowContainer && visible ) windowContainer.firstCardDisplayDone = true;

    Component.onCompleted: {
        windowContainer.cardViewParent = cardWindow;
    }

    function isWrappedWindowCarded() {
        return (windowContainer && windowContainer.windowState === WindowState.Carded);
    }
}
