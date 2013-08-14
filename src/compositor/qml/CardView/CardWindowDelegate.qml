import QtQuick 2.0
import LunaNext 0.1

import "../Compositor" as Compositor

Item {
    id: cardDelegateContainer


    // this is the card window instance wrapping the window container
    property variant cardWindow
    property real cardWidth
    property real cardHeight

    signal switchToMaximize();
    signal destructionRequest()

    Item {
        id: cardDelegateWindow

        y: cardDelegateContainer.height/2 - cardDelegateContainer.cardHeight/2
        height: cardDelegateContainer.cardHeight
        width: cardDelegateContainer.cardWidth
        scale:  (cardDelegateContainer.ListView.isCurrentItem) ? 1.0: 0.9

        Item {
            id: cardWindowWrapper

            children: [ cardWindow ]

            anchors.fill: parent

            Component.onCompleted: {
                cardWindow.parent = cardWindowWrapper;
                cardWindow.anchors.fill = cardWindowWrapper;
                cardWindow.visible = true;
            }
            Component.onDestruction: {
                cardWindow.visible = false;
                cardWindow.anchors.fill = undefined;
                cardWindow.parent = null;
                }
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
            drag.minimumY: -cardDelegateWindow.height;
            drag.maximumY: listCardsView.height;
            drag.filterChildren: true
            enabled: cardDelegateContainer.ListView.isCurrentItem && (cardWindow.windowState === WindowState.Carded)

            onClicked: switchToMaximize()

            onReleased: {
                if( cardDelegateWindow.y > -cardDelegateWindow.height/2 && cardDelegateWindow.y < cardDelegateContainer.height-cardDelegateWindow.height/2 ) {
                    cardDelegateWindow.y = cardDelegateContainer.height/2 - cardDelegateWindow.height/2;
                }
                else {
                    destructionRequest()
                }
            }
        }
    }
}
