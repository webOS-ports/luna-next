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

#include <QCoreApplication>
#include <QKeyEvent>
#include "volumekeys.h"

VolumeKeys::VolumeKeys(QObject *parent) :
    QObject(parent),
    mVolumeUp(false)
{
    mKeyRepeatDelayTimer.setSingleShot(true);
    mKeyRepeatDelayTimer.setInterval(700);
    mKeyRepeatTimer.setInterval(80);
    connect(&mKeyRepeatDelayTimer, SIGNAL(timeout()), &mKeyRepeatTimer, SLOT(start()));
    connect(&mKeyRepeatTimer, SIGNAL(timeout()), this, SLOT(changeVolume()));

    qApp->installEventFilter(this);
}

VolumeKeys::~VolumeKeys()
{
}

void VolumeKeys::stopKeyRepeat()
{
    mKeyRepeatDelayTimer.stop();
    mKeyRepeatTimer.stop();
}

void VolumeKeys::changeVolume()
{
    if (mVolumeUp)
        emit volumeUp();
    else
        emit volumeDown();
}

bool VolumeKeys::eventFilter(QObject *, QEvent *event)
{
    if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_VolumeUp || keyEvent->key() == Qt::Key_VolumeDown) {
            if (event->type() == QEvent::KeyPress) {
                // Key down: set which way to change the volume on each repeat, start the repeat delay timer and change the volume once
                mVolumeUp = keyEvent->key() == Qt::Key_VolumeUp;
                if (!mKeyRepeatDelayTimer.isActive() && !mKeyRepeatTimer.isActive()) {
                    mKeyRepeatDelayTimer.start();
                    changeVolume();
                }
            } else {
                // Key up: stop any key repeating in progress and the repeat delay timer
                stopKeyRepeat();
            }
            return true;
        }
    }
    return false;
}
