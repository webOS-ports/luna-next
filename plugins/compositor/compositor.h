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
#include <QWaylandWlShellSurface>
#include <5.8.0/QtWaylandCompositor/private/qwlextendedsurface_p.h>

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
    Q_PROPERTY(QWaylandSurface* fullscreenSurface READ fullscreenSurface WRITE setFullscreenSurface NOTIFY fullscreenSurfaceChanged)
    Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged)

public:
    Compositor();
    virtual ~Compositor();

    void create() Q_DECL_OVERRIDE;

    QWaylandSurface *fullscreenSurface() const { return mFullscreenSurface; }
    void setFullscreenSurface(QWaylandSurface *surface);

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
    void fullscreenSurfaceChanged();
    void windowsChanged();
    void recordingChanged();

private slots:
    void onSurfaceCreated(QWaylandSurface *surface);
    void onSurfaceMappedChanged();
    void onSurfaceDamaged(const QRegion &);
    void onSurfaceRaised();
    void onSurfaceLowered();
    void onSurfaceAboutToBeDestroyed(QWaylandSurface *surface);
    void windowIsReady();

    void onWlShellSurfaceCreated(QWaylandWlShellSurface *shellSurface);
    void onExtendedSurfaceReady(QtWayland::ExtendedSurface *extSurface, QWaylandSurface *surface);

private:
    friend class WindowModel;

    QWaylandSurface *mFullscreenSurface;
    unsigned int mNextWindowId;
    QHash<unsigned int, CompositorWindow*> mWindows;
    QList<WindowModel*> mWindowModels;
    RecorderManager *mRecorder;
    unsigned int mRecorderCounter;

    static Compositor *mInstance;

    QtWayland::SurfaceExtensionGlobal *mSurfaceExtension;

private:
    void readContent();
    bool hasProcessMultipleWindows(quint64 processId);
};

} // namespace luna

#endif
