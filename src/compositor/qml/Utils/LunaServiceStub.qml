import QtQuick 2.0

QtObject {
    property string name
    property string method

    signal responseReply(var arguments)

    function call(jsonArgs) {
        if( name === "com.palm.applicationManager" && method === "listApplications" ) {
            responseReply({"returnValue": true,
                        "apps": [
                 { "title": "calendar", "id": "com.palm.calendar" },
                 { "title": "email", "id": "com.palm.email" },
                 { "title": "calculator", "id": "com.palm.calc", "showInSearch": false },
                 { "title": "snowshoe", "id": "snowshoe" }
               ]});
        }
    }
}
