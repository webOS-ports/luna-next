import QtQuick 2.0
import LunaNext 0.1

import "../Compositor" as Compositor

Item {
    id: cardDelegateContainer

    property real cardWidth: ListView.view.cardWindowWidth
    property real cardHeight: ListView.view.cardWindowHeight

    // this is the window container wrapping the app window
    property variant windowContainer: window

    signal switchToMaximize();
    signal destructionRequest()

    Item {
        id: cardDelegateWindow

        y: cardDelegateContainer.height/2 - cardDelegateContent.height/2
        height: cardDelegateContainer.cardHeight
        width: cardDelegateContainer.cardWidth
        scale:  (cardDelegateContainer.ListView.isCurrentItem) ? 1.0: 0.9

        Item {
            id: cardDelegateContent

            children: [ windowContainer ]

            anchors.fill: parent

            Component.onCompleted: {
                windowContainer.cardViewParent = cardDelegateContent;
                windowContainer.visible = true;

                if( !windowContainer.firstCardDisplayDone ) {
                    windowContainer.firstCardDisplayDone = true;
                }
            }
            Component.onDestruction: {
                // If this delegate gets destroyed *and* the window is
                // in card mode, then we cut the bindings to cardDelegateContent
                // and we hide the window
                if( windowContainer && windowContainer.windowState === WindowState.Carded ) {
                    windowContainer.visible = false;
                }
            }
        }

        /*
          *** Tofe: only useful for debugging
        Text {
            anchors.top: cardDelegateWindow.top
            anchors.horizontalCenter: cardDelegateWindow.horizontalCenter

            height: 20
            text: winId

            z: 1
        }
        */

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
            enabled: (listCardsView.currentIndex === index) && (window.windowState === WindowState.Carded)

            onClicked: switchToMaximize()

            onReleased: {
                if( cardDelegateWindow.y > -cardDelegateContent.height/2 && cardDelegateWindow.y < listCardsView.height-cardDelegateContent.height/2 ) {
                    cardDelegateWindow.y = cardDelegateContainer.height/2 - cardDelegateContent.height/2;
                }
                else {
                    destructionRequest()
                }
            }
        }
    }
}
