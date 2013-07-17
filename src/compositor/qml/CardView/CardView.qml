import QtQuick 2.0

import "../Compositor/compositor.js" as CompositorLogic

Item {
    id: cardViewDisplay

    property alias listCardsModel: listCardsModel

    ListModel {
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

        spacing: 0 //root.computeFromLength(4)
        orientation: ListView.Horizontal
        smooth: true
        focus: true

        model: listCardsModel
        delegate: CardWindowDelegate {
            height: listCardsView.height
            width: cardWidth

            cardWidth: listCardsView.cardWindowWidth
            cardHeight: listCardsView.cardWindowHeight
        }
    }

    function addCard(window) {
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
}

