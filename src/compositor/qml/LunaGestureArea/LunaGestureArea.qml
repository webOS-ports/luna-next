import QtQuick 2.0
// import Qt.labs.gestures 1.0

Item {
    id: gestureAreaDisplay

    signal tapGesture
    signal swipeUpGesture(int modifiers)
    signal swipeDownGesture(int modifiers)
    signal swipeLeftGesture(int modifiers)
    signal swipeRightGesture(int modifiers)

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

            mouse.accepted = true;
        }
        onReleased: {
            var xDiff = mouseX - pressedX;
            var yDiff = mouseY - pressedY;

            var diffTime = Date.now() - timeStampPressed; /* in milliseconds here */

            if( diffTime < 500 &&
                (Math.abs(xDiff) > 10 || Math.abs(yDiff) > 10) ) {
                /* Consider this as a swipe */
                var angleTanAbs = Math.abs(xDiff/yDiff);

                /* Separate the various swipe cases */
                if( angleTanAbs < 1 /*Math.tan(Math.PI/4)*/ ) { // swipe Up or Down
                    if( yDiff>0 )
                        swipeDownGesture(mouse.modifiers);
                    else
                        swipeUpGesture(mouse.modifiers);
                }
                else {  // only posibility left: swipe Left or Right
                    if( xDiff>0 )
                        swipeRightGesture(mouse.modifiers);
                    else
                        swipeLeftGesture(mouse.modifiers);
                }
            }
            else
            {
                tapGesture();
            }

            mouse.accepted = true;
        }
    }
}
