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

#include <QWaylandQuickCompositor>
#include <QWaylandSurface>
#include <QWaylandXdgSurface>
#include <QWaylandClient>
#include <QtWaylandCompositor/private/qwlextendedsurface_p.h>

#include <QQmlContext>
#include <QQuickItem>
#include <QQuickView>
#include <QQmlParserStatus>
#include <QList>

#include "compositorwindow.h"

namespace luna
{

class WindowModel;
class RecorderManager;

class Compositor : public QWaylandQuickCompositor
{
    Q_OBJECT
    Q_PROPERTY(CompositorWindow* fullscreenWindow READ fullscreenWindow WRITE setFullscreenWindow NOTIFY fullscreenWindowChanged)
    Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged)

public:
    Compositor();
    virtual ~Compositor();

    void create() Q_DECL_OVERRIDE;

    CompositorWindow *fullscreenWindow() const { return mFullscreenWindow; }
    void setFullscreenWindow(CompositorWindow *window);

    bool recording() const { return mRecorderCounter > 0; }
    void setRecording(bool recording);

    Q_INVOKABLE void clearKeyboardFocus();
    Q_INVOKABLE void closeWindowWithId(int winId);

    void registerWindowModel(WindowModel *model);
    void unregisterWindowModel(WindowModel *model);

    static Compositor *instance();

    CompositorWindow* windowForId(int id);

signals:
    void windowAdded(QVariant window);
    void windowRemoved(QVariant window);
    void windowShown(QVariant window);
    void windowHidden(QVariant window);
    void windowRaised(QVariant window);
    void windowLowered(QVariant window);
    void fullscreenWindowChanged();
    void windowsChanged();
    void recordingChanged();

private slots:
    void onSurfaceCreated(QWaylandSurface *surface);
    void onSurfaceMappedChanged();
    void onSurfaceDamaged(const QRegion &);
    void onSurfaceRaised();
    void onSurfaceLowered();
    void onSurfaceDestroyed();
    void onSurfaceAboutToBeDestroyed(QWaylandSurface *surface);
    void windowIsReady();

    //void onXdgSurfaceCreated(QWaylandXdgSurface *shellSurface);
    void onXdgToplevelCreated(QWaylandXdgToplevel *toplevel, QWaylandXdgSurface *shellSurface);
    void onExtendedSurfaceReady(QtWayland::ExtendedSurface *extSurface, QWaylandSurface *surface);

private:
    friend class WindowModel;

    CompositorWindow *mFullscreenWindow;
    unsigned int mNextWindowId;
    QHash<unsigned int, CompositorWindow*> mWindows;
    QList<WindowModel*> mWindowModels;
    RecorderManager *mRecorder;
    unsigned int mRecorderCounter;

    static Compositor *mInstance;

    QtWayland::SurfaceExtensionGlobal *mSurfaceExtension;

private:
    void readContent();
    bool hasProcessMultipleWindows(QWaylandClient* client);
};

} // namespace luna

#endif
