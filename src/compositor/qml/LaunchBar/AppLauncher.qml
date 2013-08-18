import QtQuick 2.0

import "../LunaSysAPI" as LunaSysAPI

Rectangle {
    id: appLauncher

    property Item itemAboveLauncher
    property Item itemUnderLauncher

    state: "hidden"
    visible: false
    anchors.top: itemUnderLauncher.top

    states: [
        State {
            name: "hidden"
            AnchorChanges { target: appLauncher; anchors.bottom: undefined; anchors.top: itemUnderLauncher.top }
            PropertyChanges { target: appLauncher; visible: false }
        },
        State {
            name: "visible"
            AnchorChanges { target: appLauncher; anchors.bottom: itemUnderLauncher.top; anchors.top: itemAboveLauncher.bottom }
            PropertyChanges { target: appLauncher; visible: true }
        }
    ]

    transitions: [
        Transition {
            to: "visible"
            reversible: true

            SequentialAnimation {
                PropertyAction { target: appLauncher; property: "visible" }
                AnchorAnimation { duration: 300 }
            }
        }
    ]

    gradient: Gradient {
        GradientStop { position: 0.00; color: "#2f2f2f"; }
        GradientStop { position: 1.00; color: "#606060"; }
    }
    LunaSysAPI.ApplicationModel {
        id: appsModel
    }

    GridView {
        anchors.fill: parent
        model: appsModel

        delegate: Text {
            color: "white"
            text: index + ":" + title
        }
    }

    function toggleDisplay()
    {
        if( state === "visible" )
            state = "hidden"
        else
            state = "visible"
    }
}
