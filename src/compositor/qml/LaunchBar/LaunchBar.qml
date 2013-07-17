import QtQuick 2.0
// import QtQuick.Layouts 1.0 // Note: Qt 5.1 only

Item {
    id: launchBarDisplay

    // background of quick laucnh
    Rectangle {
        anchors.fill: launchBarDisplay
        opacity: 0.2
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
        ListElement {
            icon: "list-add.png"
        }
        ListElement {
            icon: "list-add.png"
        }
        ListElement {
            icon: "list-add.png"
        }
    }

    GridView {
        id: launcherRow

        anchors.fill: launchBarDisplay
        cellWidth : parent.width/4
        cellHeight : parent.height

        model: launcherListModel
        delegate: Item {
            width: launcherRow.cellWidth;
            height: launcherRow.cellHeight

            Image {
                id: launcherIcon
                fillMode: Image.PreserveAspectFit
                anchors.centerIn: parent.Center
                height: parent.height
                source: icon

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.startApp("myApp")
                }
            }
        }
    }
}
