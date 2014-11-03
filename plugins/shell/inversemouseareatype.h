/*
 * Copyright 2012 Canonical Ltd.
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
 */
#ifndef INVERSEMOUSEAREATYPE_H
#define INVERSEMOUSEAREATYPE_H

#include <private/qquickmousearea_p.h>
#include <QtCore/QPointer>

class QQuickItem;
class InverseMouseAreaType : public QQuickMouseArea
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *sensingArea READ sensingArea WRITE setSensingArea NOTIFY sensingAreaChanged)
    Q_PROPERTY(bool topmostItem READ topmostItem WRITE setTopmostItem NOTIFY topmostItemChanged)
public:
    explicit InverseMouseAreaType(QQuickItem *parent = 0);
    ~InverseMouseAreaType();

    Q_INVOKABLE virtual bool contains(const QPointF &point) const;

protected:
    void itemChange(ItemChange, const ItemChangeData &);
    void componentComplete();
    bool eventFilter(QObject *, QEvent *);

    // override mouse events
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);


private: // getter/setter
    QQuickItem *sensingArea() const;
    void setSensingArea(QQuickItem *sensing);
    bool topmostItem() const;
    void setTopmostItem(bool value);
    QEvent * mapEventToArea(QObject *target, QEvent *event, QPoint &point);

Q_SIGNALS:
    void sensingAreaChanged();
    void topmostItemChanged();

private Q_SLOTS:
    void update();
    void resetFilterOnWindowUpdate(QQuickWindow *win);
    
private:
    bool m_ready:1;
    bool m_topmostItem:1;
    bool m_filteredEvent:1;
    QPointer<QObject> m_filterHost;
    QPointer<QQuickItem> m_sensingArea;
    int m_touchId;

    void updateEventFilter(bool enable);
};

#endif // INVERSEMOUSEAREATYPE_H
