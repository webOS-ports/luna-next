import QtQuick 2.0

Rectangle {
    id: windowContainer

    property variant child;

    // that part should be moved to a window manager, or maybe to the card view interface
    property variant maximizedWindowParent;
    property variant normalParent;

    radius: 20
    gradient: Gradient {
        GradientStop { position: 0.0; color: "red" }
        GradientStop { position: 1.0; color: "black" }
    }

    state: "normal"

    states: [
        State {
           name: "normal"
           ParentChange {
               target: windowContainer
               parent: normalParent
               x: 0; y: 0; width: normalParent.width; height: normalParent.height
           }
           PropertyChanges {
               target: child
               anchors.margins: 20
           }
        },
         State {
            name: "maximized"
            ParentChange {
                target: windowContainer
                parent: maximizedWindowParent
                x: 0; y: 0; width: maximizedWindowParent.width; height: maximizedWindowParent.height
            }
            PropertyChanges {
                target: child
                anchors.margins: 0
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"; to: "maximized"
            SequentialAnimation {
                PropertyAction { target: maximizedWindowParent; property: "opacity"; value: 1 }
                ParentAnimation {
                    NumberAnimation { properties: "x,y,width,height"; duration: 1000 }
                    NumberAnimation { properties: "anchors.margins"; duration: 1000 }
                }
            }
        },
        Transition {
            from: "maximized"; to: "*"
            SequentialAnimation {
                ParentAnimation {
                    NumberAnimation { properties: "x,y,width,height"; duration: 1000 }
                    NumberAnimation { properties: "anchors.margins"; duration: 1000 }
                }
                PropertyAction { target: maximizedWindowParent; property: "opacity"; value: 0 }
            }
        }
    ]
}
