import QtQuick 2.0

Rectangle {
    id: windowContainer

    property variant child
    property variant compositor

    // enumeration can't be defined in QML, it has to be exposed by C++
    // so let's say:
    //   0 is Normal
    //   1 is Maximized
    //   2 is Fullscreen
    property int windowState: -1

    // that part should be moved to a window manager, or maybe to the card view interface
    property variant cardViewParent

    gradient: Gradient {
        GradientStop { position: 0.0; color: "red" }
        GradientStop { position: 1.0; color: "black" }
    }

    state: "unintialized"
    onWindowStateChanged: state = windowState == 2 ? "fullscreen" : windowState == 1 ? "maximized" : "card"

    states: [
        State {
           name: "card"
        },
        State {
            name: "maximized"
        },
        State {
           name: "fullscreen"
       }
    ]

    ParentAnimation {
        id: parentChangeAnimation
        target: windowContainer

        alwaysRunToEnd: true

        property alias targetWidth: widthTargetAnimation.to
        property alias targetHeight: heightTargetAnimation.to

        running: false
        NumberAnimation {
            target: windowContainer
            properties: "x,y"; to: 0; duration: 300
        }
        NumberAnimation {
            id: widthTargetAnimation
            target: windowContainer
            properties: "width"; duration: 300
        }
        NumberAnimation {
            id: heightTargetAnimation
            target: windowContainer
            properties: "height"; duration: 300
        }
    }

    function setNewParent(newParent) {
        parentChangeAnimation.newParent = newParent;
        parentChangeAnimation.targetWidth = newParent.width;
        parentChangeAnimation.targetHeight = newParent.height;
        parentChangeAnimation.start();
        parentChangeAnimation.stop();
    }
}
