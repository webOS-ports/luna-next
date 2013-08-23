/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

import QtQuick 2.0
import LunaNext 0.1

Item {
    LunaService {
        id: service
        name: "org.webosports.webappmanager"
        usePrivateBus: true
    }

    function wifiSetStatusResponse(data) {
        console.log("Wifi should be activated now");
        console.log("response: " + data);
    }

    Component.onCompleted: {
        var count = 0;

        service.call("luna://com.palm.wifi/setstate", "{\"state\":\"enabled\"}", wifiSetStatusResponse);

        var findNetworksCall = service.subscribe("luna://com.palm.wifi/findnetworks", "{\"subscribe\":true}", function(data) {
            console.log("Got networks response: " + data);

            if (count == 3) {
                // we're subscribed but don't want status updates anymore so cancel the call
                findNetworksCall.cancel();

                service.call("luna://com.palm.wifi/getstatus", "{}", function(data) {
                    console.log("Got state response: " + data);
                    // single call so no cancel needed
                });
            }

            count++;
        }, function(errorMessage) {
            console.log("Got error: " + errorMessage);
        });

        console.log("Calling luna://com.palm.applicationManager/listApps ...");
        var listAppsCall = service.call("luna://com.palm.applicationManager/listApps", "{}", function(data) {
            console.log("Got listApps response: " + data);

            var appList = JSON.parse(data);
            console.log(appList.apps);
        }, function(errorMessage) { });
    }
}
