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

#include "renderingtimes.h"
#include <QtCore/qmath.h>

RenderingTimes::RenderingTimes(QQuickItem* parent) :
    QQuickItem(parent),
    m_period(1000),
    m_graphModel(new GraphModel(this)),
    m_timerType(RenderTimer::Automatic),
    m_needsNewTimer(true),
    m_window(NULL),
    m_oddFrame(false),
    m_oddFrameRenderTime(0)
{
    /* Forward samplesChanged signal from graphModel */
    QObject::connect(m_graphModel, &GraphModel::samplesChanged,
                     this, &RenderingTimes::samplesChanged);

    /* Periodically append render time of the most costly frame rendered.
       The period is period / samples */
    QObject::connect(this, &RenderingTimes::frameRendered,
                     this, &RenderingTimes::onFrameRendered);
}

int RenderingTimes::period() const
{
    return m_period;
}

void RenderingTimes::setPeriod(int period)
{
    if (period != m_period) {
        m_period = period;
        Q_EMIT periodChanged();
    }
}

int RenderingTimes::samples() const
{
    return m_graphModel->samples();
}

void RenderingTimes::setSamples(int samples)
{
    m_graphModel->setSamples(samples);
}

GraphModel* RenderingTimes::graphModel() const
{
    return m_graphModel;
}

RenderTimer::TimerType RenderingTimes::timerType() const
{
    return m_timerType;
}

void RenderingTimes::setTimerType(RenderTimer::TimerType timerType)
{
    if (timerType != m_timerType) {
        m_timerType = timerType;
        // setup/teardown of RenderTimers need to happen in the rendering thread
        m_needsNewTimer = true;
        Q_EMIT timerTypeChanged();
    }
}

// FIXME: can be replaced with connecting to windowChanged() signal introduced in Qt5.2
void RenderingTimes::itemChange(ItemChange change, const ItemChangeData & value)
{
    if (change == QQuickItem::ItemSceneChange) {
        connectToWindow(value.window);
    }
    QQuickItem::itemChange(change, value);
}

void RenderingTimes::connectToWindow(QQuickWindow* window)
{
    if (window != m_window) {
        if (m_window != NULL) {
            QObject::disconnect(m_window, &QQuickWindow::sceneGraphInitialized,
                                this, &RenderingTimes::onSceneGraphInitialized);
            QObject::disconnect(m_window, &QQuickWindow::sceneGraphInvalidated,
                                this, &RenderingTimes::onSceneGraphInvalidated);
            QObject::disconnect(m_window, &QQuickWindow::beforeRendering,
                                this, &RenderingTimes::onBeforeRendering);
            QObject::disconnect(m_window, &QQuickWindow::afterRendering,
                                this, &RenderingTimes::onAfterRendering);
            QObject::disconnect(m_window, &QQuickWindow::frameSwapped,
                                this, &RenderingTimes::onFrameSwapped);
        }

        m_window = window;

        if (m_window != NULL) {
            QObject::connect(m_window, &QQuickWindow::sceneGraphInitialized,
                             this, &RenderingTimes::onSceneGraphInitialized,
                             Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::sceneGraphInvalidated,
                             this, &RenderingTimes::onSceneGraphInvalidated,
                             Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::beforeRendering,
                                this, &RenderingTimes::onBeforeRendering, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::afterRendering,
                                this, &RenderingTimes::onAfterRendering, Qt::DirectConnection);
            QObject::connect(m_window, &QQuickWindow::frameSwapped,
                                this, &RenderingTimes::onFrameSwapped, Qt::DirectConnection);
        }
    }
}

void RenderingTimes::onSceneGraphInitialized()
{
    m_renderingTimer.setup(m_timerType);
}

void RenderingTimes::onSceneGraphInvalidated()
{
    m_renderingTimer.teardown();
}

void RenderingTimes::onBeforeRendering()
{
    if (m_needsNewTimer) {
        m_renderingTimer.teardown();
        m_renderingTimer.setup(m_timerType);
        m_needsNewTimer = false;
    }
    m_renderingTimer.start();
}

void RenderingTimes::onAfterRendering()
{
    Q_EMIT frameRendered(m_renderingTimer.stop());
}

void RenderingTimes::onFrameSwapped()
{
}

void RenderingTimes::onFrameRendered(qint64 renderTime)
{
    if (!m_oddFrame) {
        appendRenderTime(m_oddFrameRenderTime);
        appendRenderTime(renderTime);
    } else {
        m_oddFrameRenderTime = renderTime;
    }
    m_oddFrame = !m_oddFrame;
}


void RenderingTimes::appendRenderTime(qint64 renderTime)
{
    int renderTimeInMs = qCeil((qreal)renderTime / 1000000);
    int width = ((qreal)samples() / m_period) * renderTimeInMs;
    m_graphModel->appendValue(width, renderTimeInMs);
}
