import QtQuick 2.0
import LunaNext 0.1

ListModel {
    id: applicationModel

    property string filter: "*"
    property QtObject applicationManagerService: LunaService {
        id: service
        name: "org.webosports.luna"
        usePrivateBus: true
    }

    function applyFilter(newFilter) {
        filter = newFilter;
        refresh()
    }

    function refresh() {
        service.call("luna://com.palm.applicationManager/listApps", {"filter": filter}, fillFromJSONResult, handleError);
    }

    function fillFromJSONResult(result) {
        applicationModel.clear();
        if(result.returnValue && result.apps !== undefined) {
            for(var i=0; i<result.apps.length; i++) {
                applicationModel.append(result.apps[i]);
            }
        }
    }

    function handleError(errorMessage) {
        console.log("Failed to call application manager: " + errorMessage);
    }

    Component.onCompleted: refresh();
}
