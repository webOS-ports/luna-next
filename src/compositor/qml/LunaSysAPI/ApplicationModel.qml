import QtQuick 2.0
//import LunaNext 0.1
import "../Utils"

ListModel {
    id: applicationModel

    property string filter: "*"
    property QtObject applicationManagerService: LunaServiceStub {
        id: applicationManagerService
        name: "com.palm.applicationManager"
        method: "listApplications"

        onResponseReply: applicationModel.fillFromJSONResult(arguments)
    }

    function applyFilter(newFilter) {
        filter = newFilter;
        refresh()
    }

    function refresh() {
        applicationManagerService.call({"filter": filter});
    }

    function fillFromJSONResult(result) {
        applicationModel.clear();
        if(result.returnValue && result.apps !== undefined) {
            for(var i=0; i<result.apps.length; i++) {
                applicationModel.append(result.apps[i]);
            }
        }
    }

    Component.onCompleted: refresh();
}
