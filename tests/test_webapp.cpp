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

#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QQmlContext>
#include <QScreen>
#include <QDebug>

#include <QtWebKit/private/qquickwebpage_p.h>
#include <QtWebKit/private/qquickwebview_p.h>

int main(int argc, char **argv)
{
    QUrl appIndexUrl;

    QGuiApplication app(argc, argv);

    QStringList arguments = app.arguments();
    for (int i = 0; i < arguments.count(); ++i) {
        QString parameter = arguments.at(i);
        if (parameter == "-a") {
            if (i + 1 >= arguments.count())
                qFatal("-a requires and argument");
            appIndexUrl = QUrl(arguments.at(i + 1));
            i++;
        }
    }

    if (appIndexUrl.isEmpty()) {
        qWarning("Not path to app index page specified!");
        exit(0);
    }

    QQuickView appView;
    appView.setTitle("Luna Test web application launcher");
    appView.setFlags(Qt::Window);
    appView.setResizeMode(QQuickView::SizeRootObjectToView);
    appView.setSource(QUrl("qrc:///webapp.qml"));
    appView.setGeometry(QRect(QPoint(0, 0), QGuiApplication::primaryScreen()->size()));

    QObject *webViewItem = appView.rootObject()->findChild<QObject*>("webView");
    if (webViewItem)
        webViewItem->setProperty("url", appIndexUrl);

    appView.show();

    return app.exec();
}
