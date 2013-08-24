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

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <QObject>

namespace luna
{

class SettingsAdapter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool tabletUi READ tabletUi CONSTANT)
    Q_PROPERTY(qreal dpi READ dpi CONSTANT)
    Q_PROPERTY(qreal compatDpi READ compatDpi CONSTANT)
    Q_PROPERTY(int displayWidth READ displayWidth CONSTANT)
    Q_PROPERTY(int displayHeight READ displayHeight CONSTANT)
    Q_PROPERTY(int displayFps READ displayFps CONSTANT)
    Q_PROPERTY(int showReticle READ showReticle CONSTANT)
    Q_PROPERTY(int splashIconSize READ splashIconSize CONSTANT)

public:
    SettingsAdapter();

    bool tabletUi() const;
    qreal dpi() const;
    qreal compatDpi() const;
    int displayWidth() const;
    int displayHeight() const;
    bool displayFps() const;
    bool showReticle() const;
    int splashIconSize() const;
};

} // namespace luna

#endif
