import QtQuick 2.0

// This import should not be needed if we separate correctly the Compositor
// and the CardView. But for the early stage, let's use it here.
import "../Compositor" as Compositor

Item {
    id: cardDelegateContainer

    property real cardWidth
    property real cardHeight

    Item {
        id: cardDelegateWindow

        y: parent.height/2 - cardDelegateContent.height/2
        height: parent.cardHeight
        width: parent.cardWidth
        scale:  (listCardsView.currentIndex === index) ? 1.0: 0.9

        Item {
            id: cardDelegateContent

            children: [ window ]

            anchors.fill: parent

            Component.onCompleted: {
                window.parent = cardDelegateContent;
                window.x = 0;
                window.y = 0;
                window.width = parent.width;
                window.height = parent.height
                window.cardViewParent = cardDelegateContent;
                window.windowState = 0;
            }
        }

        Text {
            anchors.top: cardDelegateWindow.top
            anchors.horizontalCenter: cardDelegateWindow.horizontalCenter

            height: 20
            text: index

            z: 1
        }

        Behavior on y {
            NumberAnimation { duration: 100 }
        }

        Behavior on scale {
            NumberAnimation { duration: 100 }
        }

        // Swipe a card up of down the screen to close the window:
        // use a movable area containing the card window
        MouseArea {
            anchors.fill: cardDelegateWindow
            drag.target: cardDelegateWindow
            drag.axis: Drag.YAxis
            drag.minimumY: -cardDelegateContent.height;
            drag.maximumY: listCardsView.height;
            drag.filterChildren: true
            enabled: (listCardsView.currentIndex === index) && (window.windowState === 0)

            onClicked: {
                // maximize window
                root.setCurrentMaximizedWindow(window);
            }

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
