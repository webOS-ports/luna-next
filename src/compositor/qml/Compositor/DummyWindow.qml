import QtQuick 2.0
import QtQuick.Controls 1.0

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
            CheckBox {
                text: qsTr("Breakfast")
            }
            CheckBox {
                text: qsTr("Lunch")
            }
            CheckBox {
                text: qsTr("Dinner")
            }
        }

    }
}
