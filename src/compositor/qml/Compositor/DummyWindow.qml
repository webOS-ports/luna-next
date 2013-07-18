import QtQuick 2.0

// this should be a plugin import
import "compositor.js" as CompositorLogic

Item {
    id: dummyWindow
    property alias scale: windowRectangle.scale

    Rectangle {
        id: windowRectangle

        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "black" }
        }

        Column {
            Text {
                text: "Current mode: " + (dummyWindow.parent?dummyWindow.parent.state:"undefined")

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        var currentState = CompositorLogic.getAppWindowState(dummyWindow);
                        // switch to the next state
                        currentState = (currentState+1) % 3;
                        CompositorLogic.setAppWindowState(dummyWindow, currentState);
                    }
                }
            }
            Text {
                text: "Add notification"

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        var newNotif = {
                            "icon": "../images/glow.png",
                            "content": "this is a new notification from DummyWindow"
                        };
                        CompositorLogic.addNotification(newNotif);
                    }
                }
            }
        }
    }
}
