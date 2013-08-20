import QtQuick 2.0
import LunaNext 0.1

Item {
    id: windowManager

    property Item compositor

    property Item gestureArea
    property Item cardView
    property Item statusBar
    property Item notificationsContainer

    property Item currentActiveWindow

    property real cornerRadius: 40

    property alias maximizedWindowContainer: maximizedWindowContainer
    property alias fullscreenWindowContainer: fullscreenWindowContainer

    QtObject {
        id: localProperties

        property int nextWinId: 0;

        function getNextWinId() {
            nextWinId++;
            return nextWinId;
        }
    }

    signal windowContainerCreated(variant window, int winId);

    ListModel {
        // This model contains the list of the windows that are managed by the compositor.
        // Each window is actually a "WindowContainer", whose child is the app's window.
        // It has only one property: "window", of type variant
        id: listWindowsModel

        function getIndexFromProperty(modelProperty, propertyValue) {
            var i=0;
            for(i=0; i<listWindowsModel.count;i++) {
                var item=get(i);
                if(item && item[modelProperty] === propertyValue) {
                    return i;
                }
            }

            console.log("Couldn't find window Id:" + winId);
            return -1;
        }
    }

    // maximized window container
    Item {
        id: maximizedWindowContainer

        anchors.top: statusBarDisplay.bottom
        anchors.bottom: notificationsContainer.top
        anchors.left: root.left
        anchors.right: root.right

        z: 2
    }

    // fullscreen window container
    Item {
        id: fullscreenWindowContainer

        anchors.top: root.top
        anchors.bottom: gestureAreaDisplay.top
        anchors.left: root.left
        anchors.right: root.right

        z: 3 // in front of everything
    }

    Connections {
        target: cardView
        onCardRemoved: {
            removeWindow(window);
        }
    }

    function windowAdded(appWindow) {
        // Create the window container
        var windowContainerComponent = Qt.createComponent("WindowContainer.qml");
        var windowContainer = windowContainerComponent.createObject(root);
        windowContainer.windowManager = windowManager;
        windowContainer.cornerRadius = cornerRadius

        // Bind the container with its app window
        windowContainer.setWrappedChild(appWindow);

        // insert the window at the beginning of the list
        var winId = localProperties.getNextWinId(); // very simple identifier
        listWindowsModel.insert(0, {"window": windowContainer, "winId": winId});

        // emit the signal
        windowContainerCreated(windowContainer, winId);
    }

    function windowResized(appWindow) {
    }

    function windowDestroyed(appWindow) {
        var windowContainer = appWindow.parent;
        var index = listWindowsModel.getIndexFromProperty('window', windowContainer);
        if( index >= 0 )
        {
            listWindowsModel.remove(index);
            windowContainer.destroy();
        }
    }

    function removeWindow(window) {
        var index = listWindowsModel.getIndexFromProperty('window', window);
        if( index >= 0 )
        {
            listWindowsModel.remove(index);
            cardView.removeWindow(window);
            window.destroy();
        }
    }

    function setWindowState(windowContainer, windowState) {
        if( windowState === WindowState.Maximized ) {
            setCurrentMaximizedWindow(windowContainer);
        }
        else if(windowState === WindowState.Fullscreen) {
            setCurrentFullscreenWindow(windowContainer);
        }
        else {
            restoreWindowToCard(windowContainer);
        }
    }

    function setCurrentMaximizedWindow(windowContainer) {
        // switch the state to maximized
        windowContainer.windowState = WindowState.Maximized;
        currentActiveWindow = windowContainer;

        windowContainer.setNewParent(maximizedWindowContainer, false);

        if (windowContainer.child) {
            // take focus for receiving input events
            windowContainer.child.takeFocus();
        }
    }
    function setCurrentFullscreenWindow(windowContainer) {
        // switch the state to fullscreen
        windowContainer.windowState = WindowState.Fullscreen;
        currentActiveWindow = windowContainer;

        windowContainer.setNewParent(fullscreenWindowContainer, false);

        if (windowContainer.child) {
            // take focus for receiving input events
            windowContainer.child.takeFocus();
        }
    }
    function restoreWindowToCard(windowContainer) {
        // switch the state to card
        windowContainer.windowState = WindowState.Carded;
        currentActiveWindow = null;

        windowContainer.setNewParent(windowContainer.cardViewParent, true);

        // we're back to card view so no card should have the focus
        // for the keyboard anymore
        if( compositor )
            compositor.clearKeyboardFocus();
    }
}
