/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2015 Nikolay Nizov <nizovn@gmail.com>
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

#ifndef LEDS_H_
#define LEDS_H_

#include <QObject>
#include <CoreNaviLeds.h>

namespace luna
{

class LedsAdapter : public QObject
{
	Q_OBJECT

public:
	LedsAdapter();
	Q_INVOKABLE void stopAll() const;
	Q_INVOKABLE void ledPulsate(int led, int brightness, int startDelay, int FadeIn, int FadeOut, int FadeOutDelay, int RepeatDelay, int repeat) const;
	Q_INVOKABLE void ledSet(int brightness) const;
private:
	CoreNaviLeds* m_leds;
};

} // namespace luna

#endif
