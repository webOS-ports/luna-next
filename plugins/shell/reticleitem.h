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


#ifndef RETICLEITEM_H
#define RETICLEITEM_H

#include <QQuickPaintedItem>
#include <QPointer>
#include <QPixmap>
#include <QAnimationGroup>

namespace luna
{

class ReticleItem : public QQuickPaintedItem
{
	Q_OBJECT
    Q_PROPERTY(QUrl imagePath WRITE setImagePath)
    Q_PROPERTY(qreal initialScale MEMBER mInitialScale)

public:
	ReticleItem(QQuickItem *parent = 0);
	virtual ~ReticleItem();

    virtual void componentComplete();

	QUrl imagePath() const;
	void setImagePath(const QUrl& path);

	virtual void paint(QPainter* painter);

public Q_SLOTS:
	void startAt(const QPoint& pos);

private Q_SLOTS:
	void animationFinished();

private:
	QPointer<QAnimationGroup> mAnimation;
	QPixmap mPixmap;
    QUrl mImagePath;
    qreal mInitialScale;
    bool mReady;
};

} // namespace luna

#endif
