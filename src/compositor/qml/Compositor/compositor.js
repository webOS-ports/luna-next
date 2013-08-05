/// Window services
/// Manages the list of the windows displayed in LSM

// list of the windows (which contain the app window as a child)
var windowList = null;

// add app window window to the compositor
function addWindow(appWindow)
{
    if(!appWindow) return null;

    if (windowList == null)
        windowList = new Array(0);

    var windowContainerComponent = Qt.createComponent("WindowContainer.qml");
    var windowContainer = windowContainerComponent.createObject(root);

    appWindow.parent = windowContainer;
    windowContainer.child = appWindow;
    windowContainer.root = root;
    windowContainer.children = [ appWindow ];
    appWindow.anchors.fill = windowContainer;

    windowList.push(windowContainer);

    return windowContainer;
}

// remove a window from the compositor
function removeWindow(window)
{
    var i;
    for (i = 0; i < windowList.length; ++i) {
        if (windowList[i] === window)
            break;
    }
    for (; i < windowList.length-1; ++i) {
        windowList[i] = windowList[i+1];
    }
    window[i] = null;

    window.destroy();
}

// Add a notification in the notification area.
// The notif parameter should contain the following members:
//  * icon: the path to the icon resource to show
//  * content: the html content to display in open mode
function addNotification(notif)
{
    return root.addNotification(notif);
}

/// App window service
/// Useful services to help the applications request some
/// info/changes on their window
function getAppWindowState(appWindow)
{
    var windowContainer = appWindow.parent;
    return windowContainer.windowState;
}

function setAppWindowState(appWindow, state)
{
    var windowContainer = appWindow.parent;

    root.setWindowState(windowContainer, state);
}

function getWindowMaximizedSize(window)
{
    var maxWindowSize;
    maxWindowSize.height = maximizedWindowContainer.height;
    maxWindowSize.width = maximizedWindowContainer.width;
}
