import QtQuick 2.0

Rectangle {
    id: windowContainer

    property variant child
    property variant root

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
           ParentChange {
               target: windowContainer
               parent: cardViewParent
               x: 0; y: 0
               width: cardViewParent.width
               height: cardViewParent.height
           }
        },
        State {
            name: "maximized"
            ParentChange {
                target: windowContainer
                parent: root.maximizedWindowContainer
                x: 0; y: 0
                width: root.maximizedWindowContainer.width
                height: root.maximizedWindowContainer.height
            }
        },
        State {
           name: "fullscreen"
           ParentChange {
               target: windowContainer
               parent: root.fullscreenWindowContainer
               x: 0; y: 0
               width: root.fullscreenWindowContainer.width
               height: root.fullscreenWindowContainer.height
           }
       }
    ]

    transitions: [
        Transition {
            from: "*"; to: "fullscreen,maximized,card"
            SequentialAnimation {
                ParentAnimation {
                    NumberAnimation { properties: "x,y,width,height"; duration: 1000 }
                }
            }
        }
    ]
}
