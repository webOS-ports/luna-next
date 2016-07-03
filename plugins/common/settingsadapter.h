/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2016 Herman van Hazendonk <github.com@herrie.org>
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

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <QObject>

namespace luna
{

class SettingsAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool tabletUi READ tabletUi CONSTANT)
    Q_PROPERTY(bool showNotificationsAtTop READ showNotificationsAtTop CONSTANT)
    Q_PROPERTY(qreal dpi READ dpi CONSTANT)
    Q_PROPERTY(qreal compatDpi READ compatDpi CONSTANT)
    Q_PROPERTY(int displayWidth READ displayWidth CONSTANT)
    Q_PROPERTY(int displayHeight READ displayHeight CONSTANT)
    Q_PROPERTY(int displayFps READ displayFps CONSTANT)
    Q_PROPERTY(int showReticle READ showReticle CONSTANT)
    Q_PROPERTY(int splashIconSize READ splashIconSize CONSTANT)
    Q_PROPERTY(int gestureAreaHeight READ gestureAreaHeight CONSTANT)
    Q_PROPERTY(int positiveSpaceTopPadding READ positiveSpaceTopPadding CONSTANT)
    Q_PROPERTY(int positiveSpaceBottomPadding READ positiveSpaceBottomPadding CONSTANT)
    Q_PROPERTY(QString fontStatusBar READ fontStatusBar CONSTANT)
    Q_PROPERTY(QString lunaSystemResourcesPath READ lunaSystemResourcesPath CONSTANT)
    Q_PROPERTY(bool hasVolumeButton READ hasVolumeButton CONSTANT)
    Q_PROPERTY(bool hasPowerButton READ hasPowerButton CONSTANT)
    Q_PROPERTY(bool hasHomeButton READ hasHomeButton CONSTANT)
    Q_PROPERTY(bool hasBrightnessControl READ hasBrightnessControl CONSTANT)

    /* Below is used for sounds */
    Q_PROPERTY(QString lunaSystemSoundsPath READ lunaSystemSoundsPath CONSTANT)
    Q_PROPERTY(QString lunaDefaultAlertSound READ lunaDefaultAlertSound CONSTANT)
    Q_PROPERTY(QString lunaDefaultRingtoneSound READ lunaDefaultRingtoneSound CONSTANT)
    Q_PROPERTY(QString lunaSystemSoundAppClose READ lunaSystemSoundAppClose CONSTANT)
    Q_PROPERTY(QString lunaSystemSoundScreenCapture READ lunaSystemSoundScreenCapture CONSTANT)
    Q_PROPERTY(int notificationSoundDuration READ notificationSoundDuration CONSTANT)

public:
    SettingsAdapter();

    bool tabletUi() const;
    bool showNotificationsAtTop () const;
    qreal dpi() const;
    qreal compatDpi() const;
    int displayWidth() const;
    int displayHeight() const;
    bool displayFps() const;
    bool showReticle() const;
    int splashIconSize() const;
    int gestureAreaHeight() const;
    int positiveSpaceTopPadding() const;
    int positiveSpaceBottomPadding() const;
    QString fontStatusBar() const;
    QString lunaSystemResourcesPath() const;
    bool hasVolumeButton () const; 
    bool hasPowerButton () const; 
    bool hasHomeButton () const; 
    bool hasBrightnessControl () const; 
    QString lunaSystemSoundsPath() const;
    QString lunaDefaultAlertSound() const;
    QString lunaDefaultRingtoneSound() const;
    QString lunaSystemSoundAppClose() const;
    QString lunaSystemSoundScreenCapture() const;
    int notificationSoundDuration() const;

};

} // namespace luna

#endif
