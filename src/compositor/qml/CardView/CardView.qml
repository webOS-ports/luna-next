import QtQuick 2.0

import "../Compositor/compositor.js" as CompositorLogic

Item {
    id: cardViewDisplay

    signal cardAdded(variant window, int winId)
    signal cardRemoved(variant window, int winId)

    ListModel {
        // This model contains the list of the cards
        id: listCardsModel
    }

    ListView {
        id: listCardsView

        anchors.fill: parent

        property real cardScale: 0.6
        property real cardWindowWidth: width*cardScale
        property real cardWindowHeight: height*cardScale

        preferredHighlightBegin: width/2-cardWindowWidth/2
        preferredHighlightEnd: width/2+cardWindowWidth/2
        highlightRangeMode: ListView.StrictlyEnforceRange

        model: listCardsModel
        spacing: 0 //root.computeFromLength(4)
        orientation: ListView.Horizontal
        smooth: true
        focus: true

        delegate: CardWindowDelegate {
            height: listCardsView.height
            width: cardWidth

            onSwitchToMaximize: {
                // maximize window
                root.setCurrentMaximizedWindow(window);
            }
            onDestructionRequest: {
                // remove card & emit signal
                listCardsModel.remove(index);
                cardRemoved(window, winId);
            }
        }
    }

    function appendCard(window, winId) {
        listCardsModel.append({"window": window, "winId": winId});

        listCardsView.positionViewAtEnd();

        // emit corresponding signal
        cardAdded(window, winId);
    }
}

