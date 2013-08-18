import QtQuick 2.0

QtObject {
    property string name
    property string method
    property bool usePrivateBus: false

    function call(serviceURI, jsonArgs, returnFct) {
        if( serviceURI === "luna://com.palm.applicationManager/listApps" ) {
            listApps_call(jsonArgs, returnFct);
        }
    }

    function listApps_call(jsonArgs, returnFct) {
        returnFct({"returnValue": true,
                    "apps": [
             { "title": "calendar", "id": "com.palm.calendar" },
             { "title": "email", "id": "com.palm.email" },
             { "title": "calculator", "id": "com.palm.calc", "showInSearch": false },
             { "title": "snowshoe", "id": "snowshoe" }
           ]});
    }
}
