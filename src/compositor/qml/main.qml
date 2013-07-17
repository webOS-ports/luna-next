/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtGraphicalEffects 1.0

import "CardView" as CardView
import "StatusBar" as StatusBar
import "LaunchBar" as LaunchBar
import "GestureArea" as GestureArea
import "NotificationArea" as NotificationArea
import "Compositor" as Compositor
import "Compositor/compositor.js" as CompositorLogic

Item {
    id: root

    property real screenwidth: 1280
    property real screenheight: 800
    property real screenSizeInInch: 14   // corresponds to my desktop

    property real screenDPI: (Math.sqrt(screenwidth*screenwidth + screenheight*screenheight) / (screenSizeInInch))

    width: screenwidth
    height: screenheight

    property alias maximizedWindowContainer: maximizedWindowContainer
    property alias fullscreenWindowContainer: fullscreenWindowContainer
    property alias notificationArea: notificationsContainer.notificationArea

    property variant currentActiveWindow

    // background
    Rectangle {
        id: background
        anchors.top: statusBarDisplay.bottom
        anchors.bottom: gestureAreaDisplay.top
        anchors.left: root.left
        anchors.right: root.right

        color: "black"

        z: -1; // the background item should always be behind other components

        Compositor.RoundedItem {
            anchors.fill: parent

            radius: 20
            content: Image {
                id: backgroundImage

                anchors.fill: parent
                fillMode: Image.PreserveAspectCrop
                source: "background.jpg"
                asynchronous: true
                smooth: true
                sourceSize: Qt.size(background.width, background.height)
            }
        }
    }

    // cardview
    CardView.CardView {
        id: cardViewDisplay

        anchors.top: root.top
        anchors.bottom: gestureAreaDisplay.top
        anchors.left: root.left
        anchors.right: root.right

        z: 0

        Behavior on opacity {
            NumberAnimation { duration: 100 }
        }
    }

    // bottom area: launcher bar
    LaunchBar.LaunchBar {
        id: launchBarDisplay

        anchors.bottom: gestureAreaDisplay.top
        anchors.left: root.left
        anchors.right: root.right
        height: computeFromLength(80);

        z: 1 // on top of cardview
    }

    // top area: status bar
    StatusBar.StatusBar {
        id: statusBarDisplay

        anchors.top: root.top
        anchors.left: root.left
        anchors.right: root.right
        height: computeFromLength(30);

        z: 2 // can only be hidden by a fullscreen window
    }

    // notification area
    NotificationArea.NotificationsContainer {
        id: notificationsContainer

        anchors.bottom: gestureAreaDisplay.top
        anchors.left: root.left
        anchors.right: root.right

        z: 2 // can only be hidden by a fullscreen window
    }

    // maximized window container
    Item {
        id: maximizedWindowContainer

        property variant currentMaximizedWindow

        anchors.top: statusBarDisplay.bottom
        anchors.bottom: notificationsContainer.top
        anchors.left: root.left
        anchors.right: root.right

        z: 2

        onChildrenChanged: currentMaximizedWindow = children[0]
    }

    // gesture area
    GestureArea.GestureArea {
        id: gestureAreaDisplay

        anchors.bottom: root.bottom
        anchors.left: root.left
        anchors.right: root.right
        height: computeFromLength(16);

        z: 3 // the gesture area is in front of everything, like the fullscreen window

        onClickGestureArea:{
            if(fullscreenWindowContainer.currentFullscreenWindow) {
                fullscreenWindowContainer.currentFullscreenWindow.windowState=0
            }
            else if(maximizedWindowContainer.currentMaximizedWindow) {
                maximizedWindowContainer.currentMaximizedWindow.windowState=0
            }
        }
    }

    // fullscreen window container
    Item {
        id: fullscreenWindowContainer

        property variant currentFullscreenWindow

        anchors.top: root.top
        anchors.bottom: gestureAreaDisplay.top
        anchors.left: root.left
        anchors.right: root.right

        z: 3 // in front of everything

        onChildrenChanged: currentFullscreenWindow = children[0]
    }

    function windowAdded(appWindow) {
        var newWindowContainer = CompositorLogic.addWindow(appWindow);
        cardViewDisplay.addCard(newWindowContainer);
    }

    function windowResized(window) {
    }

    function setCurrentMaximizedWindow(window) {
        // switch the state to maximized
        window.windowState = 1;
    }
    function setCurrentFullscreenWindow(window) {
        // switch the state to fullscreen
        window.windowState = 2;
    }
    function restoreWindowToCard(window) {
        // switch the state to card
        window.windowState = 0;
    }

    function windowDestroyed(appWindow) {
        // var windowContainer = appWindow.parent;
        // cardViewDisplay.removeWindow(windowContainer);
    }

    function removeWindow(window) {
        /*
        var windowContainer = window.parent;
        CompositorLogic.removeWindow(windowContainer);
        windowContainer.destroy();
        */
        cardViewDisplay.removeWindow(window);
        windowContainer.destroy();
    }

    // Utility to convert a pixel length expressed at DPI=132 to
    // a pixel length expressed in our DPI
    function computeFromLength(lengthAt132DPI) {
        return (lengthAt132DPI * (root.screenDPI / 132.0));
    }

    function addNotification(notif) {
        notificationsContainer.addNotification(notif);
    }

    function startApp(appName) {
        // Simulate the creation of a new window
        var windowComponent = Qt.createComponent("Compositor/DummyWindow.qml");
        var window = windowComponent.createObject(root);
        windowAdded(window);
    }
}
