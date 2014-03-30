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


#ifndef WINDOWMODEL_H
#define WINDOWMODEL_H

#include <QAbstractListModel>
#include <QQmlParserStatus>

namespace luna
{

class CompositorWindow;

class WindowModel : public QAbstractListModel,
                    public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(unsigned int windowTypeFilter READ windowTypeFilter WRITE setWindowTypeFilter)

public:
    WindowModel();
    ~WindowModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    unsigned int windowTypeFilter() const;
    void setWindowTypeFilter(int windowType);

    void addWindow(CompositorWindow *window);
    void removeWindow(CompositorWindow *window);

    Q_INVOKABLE int getIndexByWindowId(int winId);
    Q_INVOKABLE QVariant getByIndex(int index);
    Q_INVOKABLE QVariant getByWindowId(int winId);

protected:
    virtual void classBegin();
    virtual void componentComplete();

private:
    unsigned int mWindowTypeFilter;
    QList<int> mWindows;
    bool mComponentComplete;

    void refresh();
};

} // namespace luna

#endif // WINDOWMODEL_H
