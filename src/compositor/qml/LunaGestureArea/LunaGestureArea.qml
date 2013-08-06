import QtQuick 2.0
// import Qt.labs.gestures 1.0

Item {
    id: gestureAreaDisplay

    signal tapGesture
    signal swipeGesture(real angle)

    // Black rectangle behind, and a glowing image in front
    Rectangle {
        anchors.fill: parent
        color: "black"
    }
    Image {
        anchors.fill: parent
        source: "../images/glow.png"
        smooth: true
        fillMode: Image.Stretch
    }
    MouseArea {
        id: gestureAreaInput
        anchors.fill: parent

        property real pressedX: 0
        property real pressedY: 0
        property var timeStampPressed: 0

        onPressed: {
            pressedX = mouseX;
            pressedY = mouseY;
            timeStampPressed = Date.now();
        }
        onReleased: {
            var xDiff = mouseX - pressedX;
            var yDiff = mouseY - pressedY;

            var diffTime = Date.now() - timeStampPressed; /* in milliseconds here */

            if( diffTime < 500 &&
                (Math.abs(xDiff) > 10 || Math.abs(yDiff) > 10) ) {
                /* Consider this as a swipe */
                var angle = Math.atan(yDiff/xDiff);

                /* Convert the angle to a [-PI,PI] interval depending on dx and dy signs */
                if( xDiff < 0 ) {
                    if( yDiff < 0 )
                        angle -= Math.PI;
                    else
                        angle += Math.PI;
                }

                swipeGesture(angle);
            }
        }
    }
}
