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

#include <QCoreApplication>

#include "compositorwindow.h"

namespace luna
{

CompositorWindow::CompositorWindow(unsigned int id, QWaylandSurface *surface, QQuickItem *parent)
    : QWaylandSurfaceItem(surface, parent),
      mId(id),
      mClosed(false),
      mRemovePosted(false)
{
}

unsigned int CompositorWindow::id() const
{
    return mId;
}

bool CompositorWindow::checkIsWebAppMgr()
{
    if (!surface())
        return false;

    QFile procExeEntry(QString("/proc/%0/exe").arg(surface()->processId()));
    if (!procExeEntry.exists())
        return false;

    // FIXME make path to WebAppMgr configurable
    return procExeEntry.symLinkTarget() == "/usr/sbin/WebAppMgr";
}

void CompositorWindow::setClosed(bool closed)
{
    mClosed = closed;
}

void CompositorWindow::tryRemove()
{
    if (mClosed && !mRemovePosted) {
        mRemovePosted = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    }
}

bool CompositorWindow::event(QEvent *event)
{
    bool handled = QWaylandSurfaceItem::event(event);

    if (event->type() == QEvent::User) {
        mRemovePosted = false;
        delete this;
    }

    return handled;
}

} // namespace luna
