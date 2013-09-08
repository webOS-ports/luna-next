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

#ifndef LUNA_COMPOSITOR_H_
#define LUNA_COMPOSITOR_H_

#include "qwaylandcompositor.h"
#include "qwaylandsurface.h"
#include "qwaylandsurfaceitem.h"

#include <QQmlContext>
#include <QQuickItem>
#include <QQuickView>
#include <QQmlParserStatus>

#include "compositorwindow.h"

namespace luna
{

class Compositor : public QQuickView, public QWaylandCompositor,
                   public QQmlParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QWaylandSurface* fullscreenSurface READ fullscreenSurface WRITE setFullscreenSurface NOTIFY fullscreenSurfaceChanged)

public:
    Compositor();

    virtual void classBegin();
    virtual void componentComplete();

    QWaylandSurface *fullscreenSurface() const { return mFullscreenSurface; }

    Q_INVOKABLE void clearKeyboardFocus();
    Q_INVOKABLE void closeWindowWithId(int winId);

signals:
    void windowAdded(QVariant window);
    void windowRemoved(QVariant window);
    void windowHidden(QVariant window);
    void fullscreenSurfaceChanged();
    void windowsChanged();

public slots:
    void setFullscreenSurface(QWaylandSurface *surface);

private slots:
    void surfaceMapped();
    void surfaceUnmapped();
    void frameSwappedSlot();

protected:
    void resizeEvent(QResizeEvent *event);
    void surfaceCreated(QWaylandSurface *surface);

    virtual void surfaceAboutToBeDestroyed(QWaylandSurface *surface);

private:
    QWaylandSurface *mFullscreenSurface;
    unsigned int mNextWindowId;
    QHash<unsigned int, CompositorWindow*> mWindows;
};

} // namespace luna

#endif
