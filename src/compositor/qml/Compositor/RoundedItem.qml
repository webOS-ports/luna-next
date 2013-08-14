import QtQuick 2.0

Item {
    property real cornerRadius

    Image {
        anchors.top: parent.top
        anchors.left: parent.left
        width: cornerRadius
        height: cornerRadius
        source: "../images/Wm-corner-top-left@4x.png"
        sourceSize: Qt.size(cornerRadius, cornerRadius)
    }
    Image {
        anchors.top: parent.top
        anchors.right: parent.right
        width: cornerRadius
        height: cornerRadius
        source: "../images/Wm-corner-top-right@4x.png"
        sourceSize: Qt.size(cornerRadius, cornerRadius)
    }
    Image {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: cornerRadius
        height: cornerRadius
        source: "../images/Wm-corner-bottom-left@4x.png"
        sourceSize: Qt.size(cornerRadius, cornerRadius)
    }
    Image {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: cornerRadius
        height: cornerRadius
        source: "../images/Wm-corner-bottom-right@4x.png"
        sourceSize: Qt.size(cornerRadius, cornerRadius)
    }
}
