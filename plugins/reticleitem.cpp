/* @@@LICENSE
*
*      Copyright (c) 2010-2013 LG Electronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* LICENSE@@@ */

#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QPainter>

#include <Settings.h>
#include <AnimationSettings.h>
#include <QtUtils.h>

#include "reticleitem.h"

namespace luna
{

ReticleItem::ReticleItem(QQuickItem *parent)
	: QQuickPaintedItem(parent)
{
	setVisible(false);

	std::string filePath = Settings::LunaSettings()->lunaSystemResourcesPath + "/penindicator-ripple.png";
	mPixmap = QPixmap::fromImage(QImage(qFromUtf8Stl(filePath)));

	setWidth(mPixmap.width());
	setHeight(mPixmap.height());
}

ReticleItem::~ReticleItem()
{
	delete mAnimation;
}

void ReticleItem::startAt(const QPoint& pos)
{
	if (mAnimation)
		mAnimation->stop();

	setX(pos.x());
	setY(pos.y());
	setVisible(true);
	setOpacity(1);
	setScale(1);

	QPropertyAnimation* opacityAnimation = new QPropertyAnimation(this, "opacity");
	opacityAnimation->setDuration(AS(reticleDuration));
	opacityAnimation->setStartValue(1.0);
	opacityAnimation->setEndValue(0.0);
	opacityAnimation->setEasingCurve(AS_CURVE(reticleCurve));

	QPropertyAnimation* scaleAnimation = new QPropertyAnimation(this, "scale");
	scaleAnimation->setDuration(AS(reticleDuration));
	scaleAnimation->setStartValue(1.0);
	scaleAnimation->setEndValue(1.5);
	scaleAnimation->setEasingCurve(AS_CURVE(reticleCurve));

	QParallelAnimationGroup* reticleAnimation = new QParallelAnimationGroup;
	reticleAnimation->addAnimation(opacityAnimation);
	reticleAnimation->addAnimation(scaleAnimation);

	QPropertyAnimation* visibility = new QPropertyAnimation(this, "visible");
	visibility->setEndValue(false);
	visibility->setDuration(0);

	mAnimation = new QSequentialAnimationGroup;
	mAnimation->addAnimation(reticleAnimation);
	mAnimation->addAnimation(visibility);
	mAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void ReticleItem::animationFinished()
{
	setVisible(false);
}

void ReticleItem::paint(QPainter* painter)
{
	painter->drawPixmap(0, 0, mPixmap);
}

} // namespace luna
