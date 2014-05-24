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

#if 0
// enable the following to be able to use QML debugging/profiling. You have to start
// luna-next on the device with the following arguments: -qmljsdebugger=port:3768,block
// (see http://stackoverflow.com/questions/13366495/how-to-qt-qml-debugging-and-or-profiling)
#define QT_QML_DEBUG
#endif

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QQmlComponent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFont>

#include "shellapplication.h"

namespace luna
{

ShellApplication::ShellApplication(int argc, char **argv) :
    QGuiApplication(argc, argv),
    mEngine(this)
{
    mShellStorageDirs << "/usr/palm/luna-next/shells";

    connect(&mEngine, &QQmlEngine::quit, [=]() {
        this->quit();
    });
}

ShellApplication::~ShellApplication()
{
}

bool ShellApplication::create(const QString& name)
{
    QByteArray manifestData;
    QString shellPath;

    foreach (QString dir, mShellStorageDirs) {
        QFile manifestFile(QString("%1/%2/manifest.json").arg(dir).arg(name));
        if (!manifestFile.exists())
            continue;

        if (!manifestFile.open(QIODevice::ReadOnly))
            continue;

        manifestData = manifestFile.readAll();

        QFileInfo manifestFileInfo(manifestFile);
        shellPath = manifestFileInfo.absoluteDir().absolutePath();
    }

    if (shellPath.isEmpty()) {
        qWarning() << "Could not find manifest for shell" << name;
        return false;
    }

    QJsonDocument manifest = QJsonDocument::fromJson(manifestData);
    if (!manifest.isObject()) {
        qWarning() << "Manifest for shell" << name << "is invalid!";
        return false;
    }

    QJsonObject root = manifest.object();
    if (!(root.contains("name") && root.value("name").toString() == name) || !root.contains("main")) {
        qWarning() << "Failed to load shell" << name << "cause of a invalid manifest";
        return false;
    }

    if (root.contains("defaultFont") && root.value("defaultFont").isString()) {
        QFont defaultFont(root.value("defaultFont").toString());
        setFont(defaultFont);
    }

    QString mainPath(QString("%1/%2").arg(shellPath).arg(root.value("main").toString()));
    return load(mainPath);
}

bool ShellApplication::load(const QString& path)
{
    if (path.isEmpty()) {
        qWarning() << "Invalid shell path:" << path;
        return false;
    }

    QQmlComponent shellComponent(&mEngine, QUrl(path));
    if (shellComponent.isError()) {
        qWarning() << "Errors while loading shell from" << path;
        qWarning() << shellComponent.errors();
        return false;
    }

    QObject *shell = shellComponent.beginCreate(mEngine.rootContext());
    if (!shell) {
        qWarning() << "Error creating shell from" << path;
        qWarning() << shellComponent.errors();
        return false;
    }

    shellComponent.completeCreate();

    return true;
}

} // namespace luna
