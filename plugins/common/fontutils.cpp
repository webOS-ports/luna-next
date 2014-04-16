/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "fontutils.h"
#include "units.h"

namespace luna
{

/*!
 * \qmltype FontUtils
 * \instantiates UCFontUtils
 * \inqmlmodule Ubuntu.Components 0.1
 * \ingroup ubuntu
 * \brief FontUtils is a \l {http://qt-project.org/doc/qt-5.0/qtqml/qqmlcontext.html#details} {context property},
 * which provides utility functions for font manipulations.
 *
 * FontUtils cannot be instantiated and it is already available as a context
 * property, i.e. 'FontUtils'  is an instance  that can be used from anywhere in
 * the code.
 *
 * Example of a Label implementation which uses the default base scale for the font
 * size:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 *
 * Text {
 *     property string fontSize: "medium"
 *     font.pixelSize: FontUtils.sizeToPixels(fontSize)
 * }
 * \endqml
 *
 * Another example of a custom text input component exposing a fontSize property and a base
 * font unit size to scale its font:
 * \qml
 * import QtQuick 2.0
 * import Ubuntu.Components 0.1
 *
 * TextInput {
 *     property string fontSize: "small"
 *     property int baseFontUnits: units.dp(20)
 *     font.pixelSize: FontUtils.modularScale(fontSize) * units.dp(baseFontUnits)
 * }
 * \endqml
 *
 */

/*!
 * \qmlmethod real FontUtils::sizeToPixels(string size)
 * The function calculates the pixel size of a given scale. The size scale can be
 * one of the strings specified at modularScale function. On failure returns 0.
 */
qreal FontUtils::sizeToPixels(const QString &size)
{
    return modularScale(size) * Units::instance()->dp(14);
}

/*!
 * \qmlmethod real FontUtils::modularScale(string size)
 * The function returns the number interpretation of a given font scale. The scale
 * can have one of the following values:
 * \list
 * \li \b{xx-small}
 * \li \b{x-small}
 * \li \b{small}
 * \li \b{medium}
 * \li \b{large}
 * \li \b{x-large}
 * \endlist
 */
qreal FontUtils::modularScale(const QString &size)
{
    if (size == "xx-small") {
        return 0.606;
    } else if (size == "x-small") {
        return 0.707;
    } else if (size == "small") {
        return 0.857;
    } else if (size == "medium") {
        return 1.0;
    } else if (size == "large") {
        return 1.414;
    } else if (size == "x-large") {
        return 2.328;
    }
    return 0.0;
}

} // namespace luna
