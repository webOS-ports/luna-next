/*
 * Copyright (C) 2015 Simon Busch <morphis@gravedo.de>
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

#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <QObject>
#include <QTimer>
#include <QEvent>

class VolumeKeys : public QObject
{
    Q_OBJECT
public:
    explicit VolumeKeys(QObject *parent = 0);
    ~VolumeKeys();

    virtual bool eventFilter(QObject *watched, QEvent *event);

signals:
    void volumeUp();
    void volumeDown();

private slots:
    void changeVolume();

private:
    QTimer mKeyRepeatDelayTimer;
    QTimer mKeyRepeatTimer;
    bool mVolumeUp;

    void stopKeyRepeat();
};

#endif // VOLUMECONTROL_H
