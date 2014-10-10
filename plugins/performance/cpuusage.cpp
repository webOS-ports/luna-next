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

#include "cpuusage.h"
#include <unistd.h>

CpuUsage::CpuUsage(QQuickItem *parent) :
    QQuickItem(parent),
    m_window(NULL),
    m_graphModel(new GraphModel(this)),
    m_period(5000),
    m_samplingInterval(500),
    m_timeAtLastFrame(0)
{
    m_timingFactor = 100.0f / sysconf(_SC_NPROCESSORS_ONLN);
    m_previousClock = times(&m_previousTimes);

    QObject::connect(&m_timer, &QTimer::timeout, this, &CpuUsage::appendCpuTime);
    m_timer.setSingleShot(false);
    m_timer.setInterval(m_samplingInterval);
    m_timer.start();
}

GraphModel* CpuUsage::graphModel() const
{
    return m_graphModel;
}

int CpuUsage::period() const
{
    return m_period;
}

void CpuUsage::setPeriod(int period)
{
    if (period != m_period) {
        m_period = period;
        m_graphModel->setSamples(m_period / m_samplingInterval);
        Q_EMIT periodChanged();
    }
}

int CpuUsage::samplingInterval() const
{
    return m_samplingInterval;
}

void CpuUsage::setSamplingInterval(int samplingInterval)
{
    if (samplingInterval != m_samplingInterval) {
        m_samplingInterval = samplingInterval;
        m_graphModel->setSamples(m_period / m_samplingInterval);
        m_timer.setInterval(m_samplingInterval);
        Q_EMIT samplingIntervalChanged();
    }
}

// FIXME: can be replaced with connecting to windowChanged() signal introduced in Qt5.2
void CpuUsage::itemChange(ItemChange change, const ItemChangeData & value)
{
    if (change == QQuickItem::ItemSceneChange) {
        connectToWindow(value.window);
    }
    QQuickItem::itemChange(change, value);
}

void CpuUsage::connectToWindow(QQuickWindow* window)
{
    if (window != m_window) {
        if (m_window != NULL) {
            QObject::disconnect(m_window, &QQuickWindow::beforeSynchronizing,
                                this, &CpuUsage::onFrameRendered);
        }

        if (window != NULL) {
            QObject::connect(window, &QQuickWindow::beforeSynchronizing,
                             this, &CpuUsage::onFrameRendered);
        }

        m_window = window;
    }
}

void CpuUsage::onFrameRendered()
{
    /* A frame has been rendered:
        - if measuring CPU usage is diabled then restart it
        - otherwise store the time of the rendering
    */
    if (!m_timer.isActive()) {
        m_timer.start();
    } else {
        m_timeAtLastFrame = m_timer.remainingTime();
    }
}

void CpuUsage::appendCpuTime()
{
    // if last frame was over 80% of the timer's interval ago, then stop measuring CPU usage
    if (m_timeAtLastFrame >= 0.8 * m_timer.interval()) {
        m_timer.stop();
        return;
    }

    struct tms newTimes;
    clock_t newClock = times(&newTimes);

    float elapsed = (newTimes.tms_utime + newTimes.tms_stime - m_previousTimes.tms_utime - m_previousTimes.tms_stime)
                        / (float)(newClock - m_previousClock);

    memcpy(&m_previousTimes, &newTimes, sizeof(tms));
    m_previousClock = newClock;

    int width = ((qreal)m_graphModel->samples() / m_period) * m_samplingInterval;
    m_graphModel->appendValue(width, elapsed * m_timingFactor);
}
