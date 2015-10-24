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

#include <QtCore/qmath.h>
#include <Settings.h>
#include "units.h"

#define ENV_GRID_UNIT_PX "GRID_UNIT_PX"
#define DEFAULT_GRID_UNIT_PX 8

namespace luna
{

float Units::mGridUnit = DEFAULT_GRID_UNIT_PX;

static float getEnvFloat(const char* name, float defaultValue)
{
    QByteArray stringValue = qgetenv(name);
    bool ok;
    float value = stringValue.toFloat(&ok);
    return ok ? value : defaultValue;
}

Units::Units()
{
    // This will eventually rewrite the static value, but within the same process we should always get the same value
    mGridUnit = getEnvFloat(ENV_GRID_UNIT_PX, Settings::LunaSettings()->gridUnit);
}

float Units::length(int lengthAt132DPI)
{
    return (lengthAt132DPI * Settings::LunaSettings()->layoutScale);
}

float Units::gridUnit()
{
    return mGridUnit;
}

void Units::setGridUnit(float gridUnit)
{
    mGridUnit = gridUnit;
    Q_EMIT gridUnitChanged();
}

float Units::dp(float value)
{
    return _dp(value);
}

float Units::_dp(float value)
{
    const float ratio = mGridUnit / DEFAULT_GRID_UNIT_PX;
    if (value <= 2.0)
        // for values under 2dp, return only multiples of the value
        return qRound(value * qFloor(ratio));
    return qRound(value * ratio);
}

float Units::gu(float value)
{
    return qRound(value * mGridUnit);
}

} // luna
