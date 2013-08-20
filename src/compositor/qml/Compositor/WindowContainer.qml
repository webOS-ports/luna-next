import QtQuick 2.0
import QtGraphicalEffects 1.0
import LunaNext 0.1

Item {
    id: windowContainer

    // the window app that will be wrapped in this window container
    property alias child: childWrapper.child
    // a backlink to the window manager instance
    property variant windowManager

    //   Available window states:
    //    * Carded
    //    * Maximized
    //    * Fullscreen
    property int windowState: WindowState.Carded
    property bool firstCardDisplayDone: false

    // that part should be moved to a window manager, or maybe to the card view interface
    property variant cardViewParent

    // this is the radius that should be applied to the corners of this window container
    property real cornerRadius: 20

    // The child container, to facilite the wrapping
    Item {
        id: childWrapper
        property variant child

        anchors.fill: parent;

        function setWrappedChild(appWindow) {
            appWindow.parent = childWrapper;
            childWrapper.child = appWindow;
            childWrapper.children = [ appWindow ];
            appWindow.anchors.fill = childWrapper;
        }
    }

    // Rounded corners
    RoundedItem {
        id: cornerStaticMask
        anchors.fill: parent
        visible: false
        cornerRadius: windowContainer.cornerRadius
    }
    CornerShader {
        id: cornerShader
        anchors.fill: parent
        sourceItem: null
        radius: cornerRadius
        visible: false
    }
    state: windowState === WindowState.Fullscreen ? "fullscreen" : windowState === WindowState.Maximized ? "maximized" : "card"
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
        property bool useShaderForNewParent: false

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

        onStarted: {
            windowContainer.anchors.fill = undefined;
            if( useShaderForNewParent )
            {
                cornerShader.sourceItem = childWrapper;
                cornerShader.visible = true;
                cornerStaticMask.visible = false;
            }
        }

        onStopped: {
            windowContainer.anchors.fill = targetNewParent;
            if( !useShaderForNewParent )
            {
                cornerShader.sourceItem = null;
                cornerShader.visible = false;
                cornerStaticMask.visible = true;
            }
        }
    }

    function setWrappedChild(appWindow) {
        childWrapper.setWrappedChild(appWindow);
    }

    function setNewParent(newParent, useShader) {
        newParentAnimation.targetNewParent = newParent;
        newParentAnimation.targetWidth = newParent.width;
        newParentAnimation.targetHeight = newParent.height;
        newParentAnimation.useShaderForNewParent = useShader;
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
