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
 *
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#ifndef CPUUSAGE_H
#define CPUUSAGE_H

#include <sys/times.h>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include "graphmodel.h"

class CpuUsage : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(GraphModel* graphModel READ graphModel NOTIFY graphModelChanged)
    Q_PROPERTY(int period READ period WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(int samplingInterval READ samplingInterval WRITE setSamplingInterval NOTIFY samplingIntervalChanged)

public:
    explicit CpuUsage(QQuickItem* parent = 0);

    // getters
    GraphModel* graphModel() const;
    int period() const;
    int samplingInterval() const;

    // setters
    void setPeriod(int period);
    void setSamplingInterval(int samplingInterval);

Q_SIGNALS:
    void graphModelChanged();
    void periodChanged();
    void samplingIntervalChanged();

protected:
    void itemChange(ItemChange change, const ItemChangeData & value);

private Q_SLOTS:
    void connectToWindow(QQuickWindow* window);
    void onFrameRendered();
    void appendCpuTime();

private:
    QQuickWindow* m_window;
    GraphModel* m_graphModel;
    int m_period;
    int m_samplingInterval;
    QTimer m_timer;
    float m_timingFactor;
    struct tms m_previousTimes;
    clock_t m_previousClock;
    int m_timeAtLastFrame;
};

#endif // CPUUSAGE_H
