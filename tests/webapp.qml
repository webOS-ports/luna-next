import QtQuick 2.0
import QtWebKit 3.0
import QtWebKit.experimental 1.0

Rectangle {
    id: root

    WebView {
        id: webView
        objectName: "webView"

        clip: false

        anchors.fill: parent

        experimental.preferences.fullScreenEnabled: false
        experimental.preferences.webGLEnabled: false
        experimental.preferences.webAudioEnabled: true
        experimental.preferredMinimumContentsWidth: parent.width
    }
}
