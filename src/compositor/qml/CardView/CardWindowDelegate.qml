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

        WindowContainer {
            id: cardDelegateContent

            child: window
            maximizedWindowParent: maximizedWindowContainer
            normalParent: cardDelegateWindow

            x: 0; y: 0; width: parent.width; height: parent.height

            Component.onCompleted: {
                window.parent = cardDelegateContent;
                window.anchors.fill = cardDelegateContent;
                window.anchors.margins = 20;
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

        MouseArea {
            anchors.fill: parent
            drag.target: cardDelegateWindow
            drag.axis: Drag.YAxis
            drag.minimumY: -cardDelegateContent.height;
            drag.maximumY: listCardsView.height;
            drag.filterChildren: true
            enabled: (listCardsView.currentIndex === index) && (cardDelegateContent.state === "normal")

            onClicked: {
                // maximize window
                cardViewDisplay.setCurrentWindowMaximizedState(true);
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
