import QtQuick 2.0

Item {
    id: launchBarDisplay

    signal toggleLauncherDisplay

    height: root.computeFromLength(80);

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
            icon: "../images/default-app-icon.png"
        }
        ListElement {
            icon: "../images/default-app-icon.png"
        }
        ListElement {
            icon: "../images/default-app-icon.png"
        }
        ListElement {
            icon: "../images/default-app-icon.png"
        }
    }

    ListView {
        id: launcherRow

        anchors.fill: launchBarDisplay
        orientation: ListView.Horizontal

        model: launcherListModel
        delegate: Item {
            width: launchBarDisplay.width/(launcherListModel.count+1)
            height: launchBarDisplay.height

            Image {
                id: launcherIcon
                fillMode: Image.PreserveAspectFit
                anchors.centerIn: parent
                height: parent.height
                source: icon

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.startApp("myApp")
                }
            }
        }

        footer: Item {
            width: launchBarDisplay.width/(launcherListModel.count+1)
            height: launchBarDisplay.height

            Image {
                id: appsIcon
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                height: parent.height
                source: "../images/empty-launcher.png"

                MouseArea {
                    anchors.fill: parent
                    onClicked: launchBarDisplay.toggleLauncherDisplay()
                }
            }
        }
    }
}
