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

#include "ledsadapter.h"
#include <QtDebug>

namespace luna
{

LedsAdapter::LedsAdapter()
{
	m_leds = CoreNaviLeds::instance();
	if (!m_leds)
		qWarning() << "CoreNaviLeds were not successfully opened";
}

void LedsAdapter::stopAll() const
{
	if (!m_leds) return;
	m_leds->stopAll();
}

void LedsAdapter::ledPulsate(int led, int brightness, int startDelay, int FadeIn, int FadeOut, int FadeOutDelay, int RepeatDelay, int repeat) const
{
	if (!m_leds) return;
	m_leds->ledPulsate(led, brightness, startDelay, FadeIn, FadeOut, FadeOutDelay, RepeatDelay, repeat);
}

void LedsAdapter::ledSet(int brightness) const
{
	if (!m_leds) return;
	m_leds->ledSet(m_leds->Center(), brightness);
}

} // namespace luna
