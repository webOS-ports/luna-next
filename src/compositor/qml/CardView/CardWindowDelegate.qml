import QtQuick 2.0

// This import should not be needed if we separate correctly the Compositor
// and the CardView. But for the early stage, let's use it here.
import "../Compositor" as Compositor

Item {
    id: cardDelegateContainer

    height: listCardsView.height
    width: listCardsView.cardWindowWidth

    // Swipe a card up of down the screen to close the window:
    // use a movable area containing the card window
    Item {
        id: cardDelegateWindow

        y: parent.height/2 - cardDelegateContent.height/2
        height: parent.width
        width: parent.width
        scale:  (listCardsView.currentIndex === index) ? 1.0: 0.9

        // Of course this should be reworked to point to the real compositor window
        Compositor.DummyWindow {
            id: cardDelegateContent

            anchors.fill: parent

            Behavior on scale {
                NumberAnimation { duration: 100 }
            }
        }

        Text {
            anchors.top: cardDelegateContent.top
            anchors.horizontalCenter: cardDelegateWindow.horizontalCenter

            height: 20
            text: index + " : " + appName
        }

        Behavior on y {
            NumberAnimation { duration: 100 }
        }

        MouseArea {
            anchors.fill: parent
            drag.target: cardDelegateWindow
            drag.axis: Drag.YAxis
            drag.minimumY: -cardDelegateContent.height;
            drag.maximumY: listCardsView.height;
            drag.filterChildren: true
            enabled: (listCardsView.currentIndex === index)

            onReleased: {
                if( cardDelegateWindow.y > -cardDelegateContent.height/2 && cardDelegateWindow.y < listCardsView.height-cardDelegateContent.height/2 ) {
                    cardDelegateWindow.y = cardDelegateContainer.height/2 - cardDelegateContent.height/2;
                }
                else {
                    listCardsModel.remove(index)
                }
            }
        }
    }
}
