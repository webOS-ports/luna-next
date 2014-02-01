/*
 * Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
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

#include "windowmodel.h"
#include "compositor.h"
#include "compositorwindow.h"
#include "windowtype.h"

namespace luna
{

WindowModel::WindowModel() :
    mWindowTypeFilter(WindowType::Card),
    mComponentComplete(false)
{
    Compositor::instance()->registerWindowModel(this);
}

WindowModel::~WindowModel()
{
    Compositor::instance()->unregisterWindowModel(this);
}

void WindowModel::classBegin()
{
}

void WindowModel::componentComplete()
{
    refresh();
    mComponentComplete = true;
}

void WindowModel::setWindowTypeFilter(int windowType)
{
    mWindowTypeFilter = windowType;
    if (mComponentComplete)
        refresh();
}

unsigned int WindowModel::windowTypeFilter() const
{
    return mWindowTypeFilter;
}

void WindowModel::refresh()
{
    beginResetModel();

    mWindows.clear();

    foreach(CompositorWindow *window, Compositor::instance()->mWindows.values()) {
        if (window->windowType() == mWindowTypeFilter)
            mWindows.append(window->winId());
    }

    endResetModel();
}

void WindowModel::addWindow(CompositorWindow *window)
{
    if (!window)
        return;

    beginInsertRows(QModelIndex(), mWindows.count(), mWindows.count());
    mWindows.append(window->winId());
    endInsertRows();
}

void WindowModel::removeWindow(CompositorWindow *window)
{
    int index = mWindows.indexOf(window->winId());
    if (index == -1)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    mWindows.removeAt(index);
    endRemoveRows();
}

int WindowModel::rowCount(const QModelIndex &) const
{
    return mWindows.count();
}

QVariant WindowModel::data(const QModelIndex &index, int role) const
{
    int currentIndex = index.row();
    if (currentIndex < 0 || currentIndex >= mWindows.count())
        return QVariant();

    if (role != (Qt::UserRole + 1))
        return QVariant();

    return QVariant::fromValue(static_cast<QQuickItem*>(Compositor::instance()->windowForId(mWindows[currentIndex])));
}

QHash<int, QByteArray> WindowModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::UserRole + 1] = "window";
    return roles;
}

} // namespace luna
