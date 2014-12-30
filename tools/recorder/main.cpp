/***************************************************************************
**
** Copyright (C) 2014 Jolla Ltd.
** Contact: Giulio Camuffo <giulio.camuffo@jollamobile.com>
**
** This file is part of lipstick-recorder.
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <stdlib.h>
#include <QGuiApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QFile>
#include <QDebug>

#include "recorder.h"

int main(int argc, char *argv[])
{
    setenv("XDG_RUNTIME_DIR", "/tmp/luna-session", 0);

    QGuiApplication app(argc, argv);
    QCoreApplication::setApplicationName("luna-recorder");
    QCoreApplication::setApplicationVersion("0.2");

    QCommandLineParser parser;
    parser.setApplicationDescription("Luna Screencast recorder");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption fileOpt(QStringList() << "f" << "file",
                               "Output filename", "file");
    parser.addOption(fileOpt);

    QCommandLineOption numberOfFramesOpt(QStringList() << "n" << "number-of-frames",
                                         "Number of frames to record", "number-of-frames");
    parser.addOption(numberOfFramesOpt);

    parser.process(app);

    if (!parser.isSet(fileOpt)) {
        qCritical() << "File name for output needs to be set";
        exit(1);
    }

    const QString fileName = parser.value(fileOpt);
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical() << "Failed to open output file";
        exit(1);
    }

    unsigned int numberOfFrames = 0;
    if (parser.isSet(numberOfFramesOpt))
        numberOfFrames = parser.value(numberOfFramesOpt).toInt();

    Recorder recorder(&file, numberOfFrames);
    return app.exec();
}
