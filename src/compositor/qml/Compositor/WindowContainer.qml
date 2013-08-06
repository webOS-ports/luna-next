import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: windowContainer

    // the window app that will be wrapped in this window container
    property alias child: childWrapper.child
    // a backlink to the window manager instance
    property variant windowManager

    // enumeration can't be defined in QML, it has to be exposed by C++
    // so let's say:
    //   0 is Card
    //   1 is Maximized
    //   2 is Fullscreen
    property int windowState: 0
    property bool firstCardDisplayDone: false

    // that part should be moved to a window manager, or maybe to the card view interface
    property variant cardViewParent

    // this is the radius that should be applied to the corners of this window container
    property real cornerRadius: 20

    // The child container, to facilite the wrapping
    Item {
        id: childWrapper
        property variant child
        opacity: 0 // because it has an OpacityMask, see below

        anchors.fill: parent;

        function setWrappedChild(appWindow) {
            appWindow.parent = childWrapper;
            childWrapper.child = appWindow;
            childWrapper.children = [ appWindow ];
            appWindow.anchors.fill = childWrapper;
        }
    }

    // OpacityMask to make the rounded corners
    Rectangle {
        id: cornerMaskSource
        anchors.fill: parent
        color: "black"
        radius: cornerRadius
        visible: false
    }
    DropShadow {
        anchors.fill: parent
        horizontalOffset: 5
        verticalOffset: 5
        radius: 2.0
        samples: 4
        fast: true
        color: "#80000000"
        source: cornerMaskSource // use a canonical shape
    }
    OpacityMask {
        anchors.fill: parent
        source: childWrapper
        maskSource: cornerMaskSource
    }
    state: windowState == 2 ? "fullscreen" : windowState == 1 ? "maximized" : "card"
    onFirstCardDisplayDoneChanged: if( firstCardDisplayDone === true ) {
                                       startupAnimation();
                                   }

    states: [
        State {
           name: "unintialized"
        },
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

    ParallelAnimation {
        id: newParentAnimation
        running: false

        property alias targetNewParent: parentChangeAnimation.newParent
        property alias targetWidth: widthTargetAnimation.to
        property alias targetHeight: heightTargetAnimation.to

        ParentAnimation {
            id: parentChangeAnimation
            target: windowContainer
        }
        NumberAnimation {
            id: coordTargetAnimation
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
        NumberAnimation {
            id: scaleTargetAnimation
            target: windowContainer
            properties: "scale"; to: 1; duration: 300
        }
    }

    function setWrappedChild(appWindow) {
        childWrapper.setWrappedChild(appWindow);
    }

    function setNewParent(newParent) {
        newParentAnimation.targetNewParent = newParent;
        newParentAnimation.targetWidth = newParent.width;
        newParentAnimation.targetHeight = newParent.height;
        newParentAnimation.start();
    }

    function startupAnimation() {
        // do the whole startup animation
        // first: show as card in the cardview
        windowManager.restoreWindowToCard(windowContainer);
        newParentAnimation.complete(); // force animation to complete now
        windowManager.setCurrentMaximizedWindow(windowContainer);
    }
}
