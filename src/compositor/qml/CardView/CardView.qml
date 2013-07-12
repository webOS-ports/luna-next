import QtQuick 2.0

Item {
    id: cardViewDisplay

    property alias listCardsModel: listCardsModel

    ListModel {
        id: listCardsModel
    }

    ListView {
        id: listCardsView

        anchors.fill: parent

        property real cardWindowWidth: width/4 - spacing

        preferredHighlightBegin: width/2-cardWindowWidth/2
        preferredHighlightEnd: width/2+cardWindowWidth/2
        highlightRangeMode: ListView.StrictlyEnforceRange

        spacing: root.computeFromLength(4)
        orientation: ListView.Horizontal
        smooth: true
        focus: true

        model: listCardsModel
        delegate: CardWindowDelegate {}
    }

    function addWindow(window) {
        listCardsModel.append({"window": window})
    }

    function removeWindow(window) {
        for (i = 0; i < listCardsModel.count; ++i) {
            if (listCardsModel.get(i) === window) {
                listCardsModel.remove(i);
                break;
            }
        }
    }

    function setCurrentWindowMaximizedState(bMaximized) {
        var currentModelItem = listCardsModel.get(listCardsView.currentIndex);
        var currentWindow = currentModelItem.window;
        var currentWindowContainer = currentWindow.parent;
        if( currentWindowContainer ) {
            if( !bMaximized ) {
                currentWindowContainer.state = "normal";
            }
            else {
                currentWindowContainer.state = "maximized";
            }
        }
    }
}

