/*
 * Copyright 2012 Canonical Ltd.
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
 */

#include "inversemouseareatype.h"
#include <QtGui/QGuiApplication>
#include "quickutils.h"

/*!
  \qmltype InverseMouseArea
  \instantiates InverseMouseAreaType
  \inqmlmodule Ubuntu.Components 1.1
  \ingroup ubuntu
  \brief The InverseMouseArea captures mouse events happening outside of a given
  area.

  A typical use case is hiding of a popup or tooltip when the user presses or taps
  outside of the popup or tooltip area. The following example illustrates the use
  of InverseMouseArea in a Popup.

  Popup.qml
  \qml
  import QtQuick 2.0
  import Ubuntu.Components 1.1

  Rectangle {
      anchors.centerIn: parent
      width: 200; height: 200
      color: "darkgray"
      radius: 10
      InverseMouseArea {
         anchors.fill: parent
         acceptedButtons: Qt.LeftButton
         onPressed: parent.destroy()
      }
  }
  \endqml

  In the Popup above the mouse presses happening outside the area will cause
  closing and destroying the popup.

  \qml
  import QtQuick 2.0
  import Ubuntu.Components 1.1

  Item {
      width: 640
      height: 480
      Button {
          id: button
          text: "Press me"
          onClicked: {
              var component = Qt.createComponent("Popup.qml");
              var obj = component.create(parent);
              obj.visible = true;
          }
      }
  }
  \endqml

  By default the InverseMouseArea sensing area (the area from which the mouse
  clicks will be taken) is the application's root component, or the Window in
  which the topmost parent component of the mouse area resides. This area can be
  however altered to a different area by setting the \l sensingArea property to
  a component which is either a parent of the mouse area or a sibling of it.
  \qml
  import QtQuick 2.0
  import Ubuntu.Components 1.1

  Item {
      width: units.gu(40)
      height: units.gu(71)

      MouseArea {
          anchors.fill: parent
          onClicked: console.log("clicked on the root component")
      }

      Rectangle {
          id: blueRect
          width: units.gu(30)
          height: units.gu(51)
          anchors.centerIn: parent
          color: "blue"

          Rectangle {
              width: units.gu(20)
              height: units.gu(20)
              anchors.centerIn: parent
              color: "red"

              InverseMouseArea {
                  anchors.fill: parent
                  sensingArea: blueRect
                  onClicked: console.log("clicked on the blue rect")
              }
          }
      }
  }
  \endqml
  In this example the inverse mouse area will get mouse presses only when those
  happen on the blue rectangle area. When clicked outside of the blue rectangle
  or inside the red rectangle, the mouse area covering the root item will get
  click signals.

  InverseMouseArea, being derived from MouseArea respects the stacking and z-order
  of the components. This should be taken into account when combining it with
  MouseAreas within the same level of the component hierarchy or when combined with
  MouseArea siblings. In these cases it is recommended to have the InverseMouseArea
  declared as last component, having it in this way as last one oin the component
  stack.
  \qml
   Item {
      id: page
      width: units.gu(40)
      height: units.gu(71)

      Rectangle {
          id: label
          anchors {
              horizontalCenter: parent.horizontalCenter
              top: parent.top
          }
          height: units.gu(5)
          width: parent.width
          color: "red"

          MouseArea {
              anchors.fill: parent
              z: 1
              onPressed: console.log("red band")
          }
      }

      Rectangle {
          anchors {
              top: label.bottom
              topMargin: units.gu(2)
              horizontalCenter: parent.horizontalCenter
          }
          height: units.gu(10)
          width: parent.width
          color: "green"

          Button {
              id: button
              anchors.centerIn: parent
              text: "I'm a button, I do nothing."
          }

          MouseArea {
              anchors.fill: parent
              onPressed: console.log("green band")
          }

          InverseMouseArea {
              anchors.fill: button
              onPressed: console.log("all over except button")
          }
      }
  }
  \endqml
  When this is not enough, and you want to grab all the mouse events that have
  been sent to the sensingArea, you can use \l topmostItem to place the mouse
  area above all the components that were instantiated under the sensing area.
  Beware that setting this property will no longer consider neither the z-order
  nor the component stack order anymore, as it will steal all the mouse events
  from the component set as \l sensingArea.
  \qml
  Item {
      width: units.gu(40)
      height: units.gu(71)

      Rectangle {
          id: firstRect
          anchors {
              left: parent.left
              top: parent.top
              leftMargin: units.gu(10)
              topMargin: units.gu(10)
          }
          width: units.gu(15)
          height: width

          color: "blue"
          InverseMouseArea {
              anchors.fill: parent
              objectName: "IMA"
              topmostItem: true
              onPressed: print("IMA")
          }
      }

      Rectangle {
          anchors {
              left: firstRect.right
              top: firstRect.bottom
          }

          width: units.gu(10)
          height: width
          color: "red"
          MouseArea {
              anchors.fill: parent
              objectName: "MA"
              onPressed: print("MA")
          }
      }
  }
  \endqml
 */

/*!
  \internal
 */
InverseMouseAreaType::InverseMouseAreaType(QQuickItem *parent) :
    QQuickMouseArea(parent),
    m_ready(false),
    m_topmostItem(false),
    m_sensingArea(QuickUtils::instance().rootItem(this))
{
    /*
     * QQuickMouseArea overrides enabledChanged() signal, therefore we must make sure
     * we connect to the proper signal.
     */
    QObject::connect(this, &QQuickMouseArea::enabledChanged,
                     this, &InverseMouseAreaType::update);
    QObject::connect(this, &QQuickMouseArea::visibleChanged,
                     this, &InverseMouseAreaType::update);
    /*
     * Also connect to windowChanged() to get the proper window to filter
     */
    QObject::connect(this, &QQuickMouseArea::windowChanged,
                     this, &InverseMouseAreaType::resetFilterOnWindowUpdate);
}

InverseMouseAreaType::~InverseMouseAreaType()
{
}

void InverseMouseAreaType::updateEventFilter(bool enable)
{
    m_filteredEvent = false;
    if (!enable && m_filterHost) {
        m_filterHost->removeEventFilter(this);
        m_filterHost.clear();

    } else if (enable) {
        QQuickWindow *currentWindow = window();
        if (!currentWindow || (m_filterHost == currentWindow)) {
            return;
        }

        if (m_filterHost) {
            m_filterHost->removeEventFilter(this);
        }
        currentWindow->installEventFilter(this);
        m_filterHost = currentWindow;
    }
}


/*!
  \internal
  Slot connected to enabledChanged signal to update the state of the handler.
 */
void InverseMouseAreaType::update()
{
    if (!m_ready) {
        return;
    }
    // update sensing area
    if (!m_sensingArea) {
        m_sensingArea = QuickUtils::instance().rootItem(this);
    }
    updateEventFilter(isEnabled() && isVisible() && m_topmostItem);
    QQuickMouseArea::update();
}
/*!
  \internal
  Slot connected to windowChanged signal to refresh filter host.
 */
void InverseMouseAreaType::resetFilterOnWindowUpdate(QQuickWindow *win)
{
    Q_UNUSED(win);
    updateEventFilter(m_topmostItem);
}

void InverseMouseAreaType::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemParentHasChanged && data.item) {
        update();
    }
    QQuickMouseArea::itemChange(change, data);
}

void InverseMouseAreaType::componentComplete()
{
    QQuickMouseArea::componentComplete();
    // by now the parents should have been resolved so we can look after the
    // topmost component for the sensingArea in case it has not been set yet
    m_ready = true;
    update();
}

/*
 * Translate mouse, wheel and hover event positions to component's local coordinates.
 */
QEvent *InverseMouseAreaType::mapEventToArea(QObject *target, QEvent *event, QPoint &point)
{
    QQuickItem *targetItem = qobject_cast<QQuickItem*>(target);
    // target can be the QQuickView or QQuickWindow, then we need to get the root item from it
    if (!targetItem) {
        QQuickView *view = qobject_cast<QQuickView*>(target);
        if (view) {
            targetItem = view->rootObject();
        } else {
            QQuickWindow *window = qobject_cast<QQuickWindow*>(target);
            if (window) {
                targetItem = window->contentItem();
            }
        }
    }
    Q_ASSERT(target);
    m_filteredEvent = true;

    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseMove:
        if (targetItem != this) {
            // translate coordinates to local
            QMouseEvent *ev = static_cast<QMouseEvent*>(event);
            QMouseEvent *mev = new QMouseEvent(ev->type(),
                                   mapFromScene(ev->windowPos()),
                                   ev->windowPos(),
                                   ev->screenPos(),
                                   ev->button(), ev->buttons(), ev->modifiers());
            point = mev->pos();
            return mev;
        }
        break;
    case QEvent::Wheel:
        if (targetItem != this) {
            QWheelEvent *ev = static_cast<QWheelEvent*>(event);
            QWheelEvent *wev = new QWheelEvent(mapFromScene(ev->globalPos()), ev->globalPos(),
                                               ev->delta(), ev->buttons(), ev->modifiers(), ev->orientation());
            point = wev->pos();
            return wev;
        }
        break;
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove: {
        if (targetItem && targetItem != this) {
            QHoverEvent *ev = static_cast<QHoverEvent*>(event);
            QPointF spos = targetItem->mapToScene(ev->posF());
            QPointF sopos = targetItem->mapToScene(ev->oldPosF());
            QHoverEvent *hev = new QHoverEvent(ev->type(),
                                               mapFromScene(spos),
                                               mapFromScene(sopos),
                                               ev->modifiers());
            point = hev->pos();
            return hev;
        }
        } break;
    // convert touch events into mouse events and continue handling as such
    case QEvent::TouchBegin: {
        QTouchEvent *tev = static_cast<QTouchEvent*>(event);
        QList<QTouchEvent::TouchPoint> points = tev->touchPoints();
        const QTouchEvent::TouchPoint &primaryPoint = points.first();
        m_touchId = primaryPoint.id();
        QPointF pos = targetItem->mapToScene(primaryPoint.pos());
        QMouseEvent *mev = new QMouseEvent(QEvent::MouseButtonPress,
                        mapFromScene(pos),
                        primaryPoint.scenePos(),
                        primaryPoint.screenPos(),
                        Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
        point = mev->pos();
        return mev;
        } break;
    case QEvent::TouchUpdate: {
        QTouchEvent *tev = static_cast<QTouchEvent*>(event);
        QList<QTouchEvent::TouchPoint> points = tev->touchPoints();
        const QTouchEvent::TouchPoint &primaryPoint = points.first();
        QPointF pos = targetItem->mapToScene(primaryPoint.pos());
        QMouseEvent *mev = new QMouseEvent(QEvent::MouseMove,
                        mapFromScene(pos),
                        primaryPoint.scenePos(),
                        primaryPoint.screenPos(),
                        Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        point = mev->pos();
        return mev;
        } break;
    case QEvent::TouchEnd: {
        QMouseEvent *mev = 0;
        QTouchEvent *tev = static_cast<QTouchEvent*>(event);
        QList<QTouchEvent::TouchPoint> points = tev->touchPoints();
        for (int i = 0; i < points.count(); i++) {
            const QTouchEvent::TouchPoint &point = points.at(i);
            if (point.id() != m_touchId) {
                continue;
            }
            QPointF pos = targetItem->mapToScene(point.pos());
            mev = new QMouseEvent(QEvent::MouseButtonRelease,
                            mapFromScene(pos),
                            point.scenePos(),
                            point.screenPos(),
                            Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            break;
        }
        point = mev->pos();
        return mev;
        } break;
    default:
        m_filteredEvent = false;
        point = QPoint();
        break;
    }
    return event;
}

bool InverseMouseAreaType::eventFilter(QObject *object, QEvent *event)
{
    if (object != this)
    {
        bool captured = true;
        QPoint point;
        QEvent *mappedEvent = mapEventToArea(object, event, point);

        switch (mappedEvent->type()) {
        case QEvent::MouseButtonPress:
            mousePressEvent(static_cast<QMouseEvent*>(mappedEvent));
            break;
        case QEvent::MouseButtonRelease:
            mouseReleaseEvent(static_cast<QMouseEvent*>(mappedEvent));
            break;
        case QEvent::MouseButtonDblClick:
            mouseDoubleClickEvent(static_cast<QMouseEvent*>(mappedEvent));
            break;
        case QEvent::MouseMove:
            mouseMoveEvent(static_cast<QMouseEvent*>(mappedEvent));
            break;
        case QEvent::Wheel:
            wheelEvent(static_cast<QWheelEvent*>(mappedEvent));
            break;
        case QEvent::HoverEnter:
            hoverEnterEvent(static_cast<QHoverEvent*>(mappedEvent));
            break;
        case QEvent::HoverLeave:
            hoverLeaveEvent(static_cast<QHoverEvent*>(mappedEvent));
            break;
        case QEvent::HoverMove:
            hoverMoveEvent(static_cast<QHoverEvent*>(mappedEvent));
            break;
        default:
            captured = false;
            break;
        }
        if (mappedEvent != event) {
            event->setAccepted(mappedEvent->isAccepted());
            delete mappedEvent;
        }
        m_filteredEvent = false;
        if (captured && event->isAccepted() && contains(point)) {
            // consume the event
            return true;
        }
    }
    return false;
}

void InverseMouseAreaType::mousePressEvent(QMouseEvent *event)
{
    // overload QQuickMouseArea mousePress event as the original one sets containsMouse
    // to true automatically, however ion our case this can be false in case the press
    // happens inside the "hole"
    if (!m_topmostItem || (m_topmostItem && m_filteredEvent && contains(event->localPos()))) {
        QQuickMouseArea::mousePressEvent(event);
    } else {
        // we do not consume the mouse event
        // this is not the same as setting the accepted flag in the signal handler
        // as in this way we only tell to the event handling that the event can be
        // propagated to other filters
        event->setAccepted(false);
    }
}

void InverseMouseAreaType::mouseDoubleClickEvent(QMouseEvent *event)
{
    // same as with mousePressEvent
    if (!m_topmostItem || (m_topmostItem && m_filteredEvent && contains(event->localPos()))) {
        QQuickMouseArea::mouseDoubleClickEvent(event);
    } else {
        // we do not consume the mouse event
        // this is not the same as setting the accepted flag in the signal handler
        // as in this way we only tell to the event handling that the event can be
        // propagated to other filters
        event->setAccepted(false);
    }
}

/*!
  \qmlproperty Item InverseMouseArea::sensingArea
  This property holds the sensing area of the inverse mouse area. By default it
  is the root item but it can be set to any other area. The area can be reset
  to the root item by setting null to the property.
 */
QQuickItem *InverseMouseAreaType::sensingArea() const
{
    return m_sensingArea;
}
void InverseMouseAreaType::setSensingArea(QQuickItem *sensing)
{
    if (!sensing)
        sensing = QuickUtils::instance().rootItem(this);
    if (sensing != m_sensingArea) {
        // clear previous filter
        updateEventFilter(false);
        m_sensingArea = sensing;
        updateEventFilter(m_topmostItem);
        Q_EMIT sensingAreaChanged();
    }
}

/*!
  \qmlproperty bool InverseMouseArea::topmostItem
  The property specifies whether the InverseMouseArea should be above all components
  taking all mouse, wheel and hover events from the application's or from the area
  specified by the \l sensingArea (true), or only from the siblings (false).
  The default value is false.
  */
bool InverseMouseAreaType::topmostItem() const
{
    return m_topmostItem;
}
void InverseMouseAreaType::setTopmostItem(bool value)
{
    if (m_topmostItem != value) {
        m_topmostItem = value;
        updateEventFilter(m_topmostItem);
        Q_EMIT topmostItemChanged();
    }
}

/*
 * Overrides QQuickItem's contains method. Point is in local coordinates.
 */
bool InverseMouseAreaType::contains(const QPointF &point) const
{
    QPointF scenePos = mapToScene(point);
    QRectF oskRect = QGuiApplication::inputMethod()->keyboardRectangle();
    bool pointInArea = QQuickMouseArea::contains(point);
    bool pointInOSK = oskRect.contains(scenePos);
    bool pointOutArea = (m_sensingArea && m_sensingArea->contains(m_sensingArea->mapFromScene(scenePos)));
    return !pointInArea && !pointInOSK && pointOutArea;
}
