/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import LunaNext.Common 0.1
import LunaNext.Performance 0.1 as Performance

/*!
    \qmltype PerformanceOverlay
    \inqmlmodule Ubuntu.Performance 0.1
    \ingroup ubuntu-performance-metrics
    \brief Overlay displaying performance related metrics (rendering time, CPU usage, etc.)

    PerformanceOverlay displays various performance related indicators to help
    developers detect issues in their application.

    Examples:
    \qml
        PerformanceOverlay {
            active: true
        }
    \endqml
*/
Item {
    id: overlay

    anchors.fill: parent

    Component {
        id: overlayContent

        Item {
            MouseArea {
                anchors.fill: content
                drag.target: content
                onClicked: overlay.active = false
            }

            Item {
                id: content
                x: parent.width - width - Units.gu(2)
                y: Units.gu(10)
                width: Units.gu(38)
                height: childrenRect.height

                Performance.RenderingTimes {
                    id: renderingTimes
                    period: 2000
                    samples: renderingTimeGraph.width
                    timerType: Performance.RenderingTimes.Trivial
                }

                Performance.CpuUsage {
                    id: cpuUsage
                    period: 5000
                    samplingInterval: 200
                }

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: Units.gu(1)

                    BarGraph {
                        id: renderingTimeGraph
                        anchors.left: parent.left
                        anchors.right: parent.right
                        model: renderingTimes.graphModel
                        maximumValue: 20
                        threshold: 16
                        labels: [{"color": "darkorange", "value": 10, "label": "10 ms"},
                                {"color": "red", "value": 16, "label": "16 ms"}]
                        labelFormat: "%1 ms"
                    }

                    BarGraph {
                        id: cpuUsageGraph
                        anchors.left: parent.left
                        anchors.right: parent.right
                        model: cpuUsage.graphModel
                        maximumValue: 100
                        threshold: 75
                        labels: [{"color": "green", "value": 25, "label": "25%"},
                                 {"color": "darkorange", "value": 50, "label": "50%"},
                                 {"color": "red", "value": 75, "label": "75%"}]
                        labelFormat: "%1 %"
                    }
                }
            }

        }
    }

    /*!
       Whether or not the PerformanceOverlay is displayed.
    */
    property bool active: true

    MouseArea {
        property var timeLastPress
        property int delayBetweenPresses: 500
        property int pressCount: 0
        property int pressCountToActivate: 4

        anchors.fill: parent
        enabled: performanceOverlayEnabled

        onPressed: {
            mouse.accepted = false;
            var timePress = new Date().getTime();

            if (timeLastPress && timePress - timeLastPress > delayBetweenPresses) {
                pressCount = 0;
            }

            pressCount += 1;

            if (pressCount >= pressCountToActivate) {
                overlay.active = true;
                pressCount = 0;
            }

            timeLastPress = timePress;
        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        active: overlay.active
        asynchronous: true
        sourceComponent: overlayContent
    }
}
