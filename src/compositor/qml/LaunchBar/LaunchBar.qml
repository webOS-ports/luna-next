import QtQuick 2.0

Item {
    id: launchBarDisplay

    // background of quick laucnh
    Rectangle {
        anchors.fill: launchBarDisplay
        opacity: 0.8
        gradient: Gradient {
            GradientStop { position: 0.0; color: "grey" }
            GradientStop { position: 1.0; color: "white" }
        }
    }

    // list of icons
    ListModel {
        id: launcherListModel

        ListElement {
            icon: "list-add.png"
        }
    }

    ListView {
        id: launcherListView

        anchors.fill: launchBarDisplay
        orientation: ListView.Horizontal

        model: launcherListModel
        delegate: Image {
            id: launcherIcon
            fillMode: Image.PreserveAspectFit
            height: parent.height
            source: icon

            MouseArea {
                anchors.fill: parent
                onClicked: root.startApp("myApp")
            }
        }
    }
}
