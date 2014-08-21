/*
 * Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2014 Nikolay Nizov <nizovn@gmail.com>
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

#include <QGuiApplication>
#include <QTouchEvent>

#include "reticlehandler.h"

namespace luna
{

ReticleHandler::ReticleHandler(QQuickItem *parent) :
	QQuickItem(parent)
{
	qApp->installEventFilter(this);
}

bool ReticleHandler::eventFilter(QObject *, QEvent *event)
{
	static bool show = true;
	switch (event->type()) {
	case QEvent::TouchBegin:
		show = true;
		break;
	case QEvent::TouchUpdate:
		show = false;
		break;
	case QEvent::TouchCancel:
		show = false;
		break;
	case QEvent::TouchEnd:
		{
			if (show) {
				QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
				const QTouchEvent::TouchPoint &touchPoint = touchEvent->touchPoints().first();
				QPointF fpoint = touchPoint.pos();
				int x = static_cast<int>(fpoint.x());
				int y = static_cast<int>(fpoint.y());
				QPoint point(x,y);
				emit reticleEvent(point);
			};
			show = false;
		}
		break;
	}
	return false;
}

} // namespace luna
