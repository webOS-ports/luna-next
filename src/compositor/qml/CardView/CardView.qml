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
}

