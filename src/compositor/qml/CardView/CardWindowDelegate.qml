import QtQuick 2.0

Item {
    id: cardDelegateContainer

    property real cardWidth: ListView.view.cardWindowWidth
    property real cardHeight: ListView.view.cardWindowHeight

    signal switchToMaximize();
    signal destructionRequest()

    Item {
        id: cardDelegateWindow

        y: parent.height/2 - cardDelegateContent.height/2
        height: parent.cardHeight
        width: parent.cardWidth
        scale:  (cardDelegateContainer.ListView.isCurrentItem) ? 1.0: 0.9

        Item {
            id: cardDelegateContent

            children: [ window ]

            anchors.fill: parent

            Component.onCompleted: {
                window.parent = cardDelegateContent;
                window.x = 0;
                window.y = 0;
                window.width = cardDelegateContainer.cardWidth;
                window.height = cardDelegateContainer.cardHeight;
                window.cardViewParent = cardDelegateContent;
                window.windowState = 0;
                window.visible = true;
            }
            Component.onDestruction: {
                // If this delegate gets destroyed *and* the window is
                // in card mode, then we cut the bindings to cardDelegateContent
                // and we hide the window
                if( window.windowState === 0 ) {
                    window.visible = false;
                    window.width = 100;  // hard-coded random value
                    window.height = 100; // hard-coded random value
                }
            }
        }

        Text {
            anchors.top: cardDelegateWindow.top
            anchors.horizontalCenter: cardDelegateWindow.horizontalCenter

            height: 20
            text: winId

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
