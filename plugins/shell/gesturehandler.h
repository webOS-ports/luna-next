/*
 * Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2016 Nikolay Nizov <nizovn@gmail.com>
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

#ifndef GESTUREHANDLER_H
#define GESTUREHANDLER_H

#include <QQuickItem>
#include <QTimer>

namespace luna
{

class TapGestureRecognizer;
class ScreenEdgeFlickGestureRecognizer;

class GestureHandler: public QQuickItem
{
	Q_OBJECT
	Q_PROPERTY(qreal fingerSize WRITE setFingerSize MEMBER mFingerSize)
	Q_PROPERTY(qreal minimalFlickLength WRITE setMinimalFlickLength MEMBER mMinFlick)
	Q_PROPERTY(int timeout WRITE setTimeout MEMBER mTimeout)

public:
	enum GestureType { TapGesture, ScreenEdgeFlickGesture, OtherGesture};
	Q_ENUM(GestureType)

	explicit GestureHandler(QQuickItem *parent = 0);
	~GestureHandler();
	void setFingerSize(qreal size);
	void setTimeout(int timeout);
	void setMinimalFlickLength(qreal length);

signals:
	void touchBegin();
	void touchEnd();
	void gestureEvent(GestureType gestureType, QPoint pos, bool timeout);

private:
	TapGestureRecognizer *mTapRecognizer;
	ScreenEdgeFlickGestureRecognizer *mScreenEdgeFlickRecognizer;
	qreal mFingerSize;
	qreal mMinFlick;
	int mTimeout;
};

class TapGestureRecognizer: public QQuickItem
{
	Q_OBJECT

public:
	explicit TapGestureRecognizer(QQuickItem *parent = 0);
	virtual bool eventFilter(QObject *watched, QEvent *event);
	void setFingerSize(qreal fingerSize) {mFingerSize = fingerSize;};

signals:
	void tapGesture(GestureHandler::GestureType gestureType, QPoint pos, bool timeout);

private: 
	qreal mFingerSize;
};

class ScreenEdgeFlickGestureRecognizer: public QQuickItem
{
	Q_OBJECT

public:
	explicit ScreenEdgeFlickGestureRecognizer(QQuickItem *parent = 0);
	virtual bool eventFilter(QObject *watched, QEvent *event);
	void setFingerSize(qreal fingerSize) {mFingerSize = fingerSize;};
	void setTimeout(int timeout) {mTimer.setInterval(timeout);};
	void setMinimalFlickLength(qreal length) {mMinFlick = length;};

signals:
	void screenEdgeFlickGesture(GestureHandler::GestureType gestureType, QPoint pos, bool timeout);
	void touchBegin();
	void touchEnd();

private slots:
	void slotTimeout();

private:
	QTimer mTimer;
	qreal mFingerSize;
	qreal mMinFlick;
	QPoint mStartPos;
};

} // namespace luna

#endif // GESTUREHANDLER_H

