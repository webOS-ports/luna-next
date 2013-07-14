import QtQuick 2.0
import QtQuick.Layouts 1.0

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

    RowLayout {
        id: launcherRow

        anchors.fill: launchBarDisplay

        Repeater {
            model: launcherListModel
            delegate: Image {
                id: launcherIcon
                fillMode: Image.PreserveAspectFit
                height: parent.height
                source: icon

                Layout.fillWidth: true
                Layout.alignment: Qt.AlignCenter

                MouseArea {
                    anchors.fill: parent
                    onClicked: root.startApp("myApp")
                }
            }
        }
    }
}
