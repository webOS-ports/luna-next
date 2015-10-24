/*
 * Copyright 2013 Canonical Ltd.
 * Copyright 2015 Herman van Hazendonk <github.com@herrie.org>
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
    return modularScale(size) * Units::_dp(14);
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
    if (size == "6pt") {
        return 0.4286;
    } else if( size == "7pt") {
        return 0.50;
    } else if (size == "xx-small" || size == "8pt") {
        return 0.5714;
    } else if (size == "9pt") {
        return 0.6429;
    } else if (size == "x-small" || size == "10pt") {
        return 0.7143;
    } else if (size == "11pt") {
        return 0.7857;
    } else if (size == "small" || size == "12pt") {
        return 0.8571;
    } else if (size == "13pt") {
        return 0.9286;
    } else if (size == "medium" || size == "14pt") {
        return 1.0;
    } else if (size == "15pt") {
        return 1.0714;
    } else if (size == "16pt") {
        return 1.1429;
    } else if (size == "17pt") {
        return 1.2143;
    } else if (size == "18pt") {
        return 1.2857;
    } else if (size == "large" || size == "20pt") {
        return 1.4286;
    } else if (size == "22pt") {
        return 1.5714;
    } else if (size == "24pt") {
        return 1.7143;
    } else if (size == "26pt") {
        return 1.8571;
    } else if (size == "28pt") {
        return 2.0;
    } else if (size == "30pt") {
        return 2.1429;
    } else if (size == "x-large" || size == "32pt") {
        return 2.2857;
    } else if (size == "36pt") {
        return 2.5714;
    } else if (size == "48pt") {
        return 3.4286;
    } else if (size == "72pt") {
        return 5.1429;
    }
    return 0.0;
}

} // namespace luna
