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

#ifndef PERFORMANCE_METRICS_H
#define PERFORMANCE_METRICS_H

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>
#include <QtCore/QElapsedTimer>
#include "graphmodel.h"
#include "rendertimer.h"

class RenderingTimes : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(int period READ period WRITE setPeriod NOTIFY periodChanged)
    Q_PROPERTY(int samples READ samples WRITE setSamples NOTIFY samplesChanged)
    Q_PROPERTY(GraphModel* graphModel READ graphModel NOTIFY graphModelChanged)
    Q_PROPERTY(RenderTimer::TimerType timerType READ timerType WRITE setTimerType NOTIFY timerTypeChanged)

    Q_ENUMS(RenderTimer::TimerType)

public:
    explicit RenderingTimes(QQuickItem* parent = 0);

    // getters
    int period() const;
    int samples() const;
    GraphModel* graphModel() const;
    RenderTimer::TimerType timerType() const;

    // setters
    void setPeriod(int period);
    void setSamples(int samples);
    void setTimerType(RenderTimer::TimerType timerType);

Q_SIGNALS:
    void periodChanged();
    void samplesChanged();
    void graphModelChanged();
    void timerTypeChanged();
    void frameRendered(qint64 renderTime);

protected:
    void itemChange(ItemChange change, const ItemChangeData & value);

private Q_SLOTS:
    void connectToWindow(QQuickWindow* window);
    void onSceneGraphInitialized();
    void onSceneGraphInvalidated();
    void onBeforeRendering();
    void onAfterRendering();
    void onFrameSwapped();
    void onFrameRendered(qint64 renderTime);

private:
    void appendRenderTime(qint64 renderTime);

private:
    int m_period;
    GraphModel* m_graphModel;
    RenderTimer::TimerType m_timerType;
    bool m_needsNewTimer;
    RenderTimer m_renderingTimer;
    QQuickWindow* m_window;
    bool m_oddFrame;
    qint64 m_oddFrameRenderTime;
};

#endif // PERFORMANCE_METRICS_H
