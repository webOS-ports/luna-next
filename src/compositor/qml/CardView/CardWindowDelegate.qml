import QtQuick 2.0

// This import should not be needed if we separate correctly the Compositor
// and the CardView. But for the early stage, let's use it here.
import "../Compositor" as Compositor

Item {
    id: cardDelegateContainer

    height: listCardsView.height
    width: listCardsView.cardWindowWidth

    // Swipe a card up of down the screen to close the window:
    // use a Flickable area containing the card window
    Flickable {
        id: cardDelegateVerticalSwipe

        anchors.fill: cardDelegateContainer
        flickableDirection: Flickable.VerticalFlick
        interactive: (listCardsView.currentIndex === index)

        Item {
            anchors.verticalCenter: parent.verticalCenter
            height: parent.width
            width: parent.width

            // Of course this should be reworked to point to the real compositor window
            Compositor.DummyWindow {
                id: cardDelegateWindow

                anchors.fill: parent
                scale:  (listCardsView.currentIndex === index) ? 1.0: 0.9

                Behavior on scale {
                    NumberAnimation { duration: 100 }
                }
            }

            Text {
                anchors.top: cardDelegateWindow.top
                anchors.horizontalCenter: cardDelegateWindow.horizontalCenter

                height: 20
                text: index + " : " + appName
            }
        }
    }

}
