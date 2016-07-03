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

#include <Settings.h>

#include "settingsadapter.h"

namespace luna
{

SettingsAdapter::SettingsAdapter()
{
}

bool SettingsAdapter::tabletUi() const
{
    return Settings::LunaSettings()->tabletUi;
}

bool SettingsAdapter::showNotificationsAtTop() const
{
    return Settings::LunaSettings()->showNotificationsAtTop;
}

qreal SettingsAdapter::dpi() const
{
    return Settings::LunaSettings()->dpi;
}

qreal SettingsAdapter::compatDpi() const
{
    return Settings::LunaSettings()->compatDpi;
}

int SettingsAdapter::displayWidth() const
{
    return Settings::LunaSettings()->displayWidth;
}

int SettingsAdapter::displayHeight() const
{
    return Settings::LunaSettings()->displayHeight;
}

bool SettingsAdapter::displayFps() const
{
    return Settings::LunaSettings()->debug_piranhaDisplayFps;
}

bool SettingsAdapter::showReticle() const
{
    return Settings::LunaSettings()->showReticle;
}

int SettingsAdapter::splashIconSize() const
{
    return Settings::LunaSettings()->splashIconSize;
}

int SettingsAdapter::gestureAreaHeight() const
{
    return Settings::LunaSettings()->gestureAreaHeight;
}

int SettingsAdapter::positiveSpaceTopPadding() const
{
    return Settings::LunaSettings()->positiveSpaceTopPadding;
}

int SettingsAdapter::positiveSpaceBottomPadding() const
{
    return Settings::LunaSettings()->positiveSpaceBottomPadding;
}

QString SettingsAdapter::fontStatusBar() const
{
    return QString::fromStdString(Settings::LunaSettings()->fontStatusBar);
}

QString SettingsAdapter::lunaSystemResourcesPath() const
{
    return QString::fromStdString(Settings::LunaSettings()->lunaSystemResourcesPath);
}

bool SettingsAdapter::hasVolumeButton() const
{
    return Settings::LunaSettings()->hasVolumeButton;
}

bool SettingsAdapter::hasPowerButton() const
{
    return Settings::LunaSettings()->hasPowerButton;
}

bool SettingsAdapter::hasHomeButton() const
{
    return Settings::LunaSettings()->hasHomeButton;
}

bool SettingsAdapter::hasBrightnessControl() const
{
    return Settings::LunaSettings()->hasBrightnessControl;
}

/* Below is used for sounds */

QString SettingsAdapter::lunaSystemSoundsPath() const
{
    return QString::fromStdString(Settings::LunaSettings()->lunaSystemSoundsPath);
}

QString SettingsAdapter::lunaDefaultAlertSound() const
{
    return QString::fromStdString(Settings::LunaSettings()->lunaDefaultAlertSound);
}

QString SettingsAdapter::lunaDefaultRingtoneSound() const
{
    return QString::fromStdString(Settings::LunaSettings()->lunaDefaultRingtoneSound);
}

QString SettingsAdapter::lunaSystemSoundAppClose() const
{
    return QString::fromStdString(Settings::LunaSettings()->lunaSystemSoundAppClose);
}

QString SettingsAdapter::lunaSystemSoundScreenCapture() const
{
    return QString::fromStdString(Settings::LunaSettings()->lunaSystemSoundScreenCapture);
}

int SettingsAdapter::notificationSoundDuration() const
{
    return Settings::LunaSettings()->notificationSoundDuration;
}

} // namespace luna
