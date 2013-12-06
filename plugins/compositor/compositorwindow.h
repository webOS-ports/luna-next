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

#ifndef LUNA_COMPOSITORWINDOW_H_
#define LUNA_COMPOSITORWINDOW_H_

#include "qwaylandsurface.h"
#include "qwaylandsurfaceitem.h"

#include "eventtype.h"

namespace luna
{

class CompositorWindow : public QWaylandSurfaceItem
{
    Q_OBJECT
    Q_PROPERTY(int winId READ winId CONSTANT)
    Q_PROPERTY(int windowType READ windowType CONSTANT)
    Q_PROPERTY(QString appId READ appId CONSTANT)

public:
    CompositorWindow(unsigned int winId, QWaylandSurface *surface, QQuickItem *parent = 0);

    unsigned int winId() const;
    unsigned int windowType() const;
    QString appId() const;

    void setClosed(bool closed);
    void tryRemove();

    bool checkIsAllowedToStay();

    Q_INVOKABLE void postEvent(int event);
    Q_INVOKABLE void changeSize(const QSize& size);

protected:
    virtual bool event(QEvent *event);

private:
    unsigned int mId;
    unsigned int mWindowType;
    bool mClosed;
    bool mRemovePosted;
    QString mAppId;
};

} // namespace luna

#endif
