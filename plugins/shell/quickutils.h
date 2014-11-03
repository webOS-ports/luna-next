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
 *
 */

#ifndef QUICKUTILS_P_H
#define QUICKUTILS_P_H

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtQuick/QQuickView>

class QQuickItem;
class QQmlEngine;
class QQmlComponent;
class QuickUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *rootObject READ rootObject NOTIFY rootObjectChanged)
    Q_PROPERTY(QString inputMethodProvider READ inputMethodProvider)
    Q_PROPERTY(bool touchScreenAvailable READ touchScreenAvailable NOTIFY touchScreenAvailableChanged)
public:
    static QuickUtils& instance()
    {
        static QuickUtils instance;
        return instance;
    }

    QQuickItem *rootObject();
    Q_INVOKABLE QQuickItem *rootItem(QObject *object);
    QString inputMethodProvider() const;
    bool touchScreenAvailable() const;

    Q_INVOKABLE static QString className(QObject *item);
    QObject* createQmlObject(const QUrl &url, QQmlEngine *engine);

Q_SIGNALS:
    void rootObjectChanged();
    void activated();
    void deactivated();
    void touchScreenAvailableChanged();

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    explicit QuickUtils(QObject *parent = 0);
    QPointer<QQuickView> m_rootView;

    void lookupQuickView();
};


#endif // QUICKUTILS_P_H
