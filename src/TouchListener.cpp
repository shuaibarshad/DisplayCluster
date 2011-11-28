#include "TouchListener.h"
#include "main.h"
#include "DisplayGroupGraphicsViewProxy.h"
#include "DisplayGroupGraphicsView.h"

TouchListener::TouchListener()
{
    graphicsViewProxy_ = new DisplayGroupGraphicsViewProxy(g_displayGroup);

    client_.addTuioListener(this);
    client_.connect();
}

void TouchListener::addTuioObject(TUIO::TuioObject *tobj)
{

}

void TouchListener::updateTuioObject(TUIO::TuioObject *tobj)
{

}

void TouchListener::removeTuioObject(TUIO::TuioObject *tobj)
{

}

void TouchListener::addTuioCursor(TUIO::TuioCursor *tcur)
{
    QPointF point(tcur->getX(), tcur->getY());

    // figure out what kind of click this is
    int clickType = 0; // 0: no click, 1: single click, 2: double click

    QPointF delta1 = point - lastClickPoint1_;

    if(sqrtf(delta1.x()*delta1.x() + delta1.y()*delta1.y()) < DOUBLE_CLICK_DISTANCE && lastClickTime1_.elapsed() < DOUBLE_CLICK_TIME)
    {
        clickType = 1;

        QPointF delta2 = point - lastClickPoint2_;

        if(sqrtf(delta2.x()*delta2.x() + delta2.y()*delta2.y()) < DOUBLE_CLICK_DISTANCE && lastClickTime2_.elapsed() < DOUBLE_CLICK_TIME)
        {
            clickType = 2;
        }
    }

    // reset last click information
    lastClickTime2_ = lastClickTime1_;
    lastClickPoint2_ = lastClickPoint1_;

    lastClickTime1_.restart();
    lastClickPoint1_ = point;

    // create the mouse event
    QGraphicsSceneMouseEvent * event = NULL;
    
    if(clickType == 2)
    {
        event = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
    }
    else if(clickType == 1)
    {
        event = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMousePress);
    }

    if(event != NULL)
    {
        // set event parameters
        event->setScenePos(point);

        if(tcur->getCursorID() == 0)
        {
            event->setButton(Qt::LeftButton);
            event->setButtons(Qt::LeftButton);
        }
        else if(tcur->getCursorID() == 1)
        {
            event->setButton(Qt::RightButton);
            event->setButtons(Qt::RightButton);
        }

        // use alt keyboard modifier to indicate this is a touch event
        event->setModifiers(Qt::AltModifier);

        // post the event (thread-safe)
        QApplication::postEvent(graphicsViewProxy_->getGraphicsView()->scene(), event);
    }

    // reset last point
    lastPoint_ = point;
}

void TouchListener::updateTuioCursor(TUIO::TuioCursor *tcur)
{
    // if more than one cursor is down, only accept cursor 1 for right movements
    if(client_.getTuioCursors().size() > 1 && tcur->getCursorID() != 1)
    {
        return;
    }

    QPointF point(tcur->getX(), tcur->getY());

    // create the mouse event
    QGraphicsSceneMouseEvent * event = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseMove);

    // set event parameters
    event->setScenePos(point);
    event->setLastScenePos(lastPoint_);
    event->setButton(Qt::NoButton);

    // use alt keyboard modifier to indicate this is a touch event
    event->setModifiers(Qt::AltModifier);

    if(tcur->getCursorID() == 0)
    {
        event->setButtons(Qt::LeftButton);
    }
    else if(tcur->getCursorID() == 1)
    {
        event->setButtons(Qt::RightButton);
    }

    // post the event (thread-safe)
    QApplication::postEvent(graphicsViewProxy_->getGraphicsView()->scene(), event);

    // reset last point
    lastPoint_ = point;
}

void TouchListener::removeTuioCursor(TUIO::TuioCursor *tcur)
{
    QPointF point(tcur->getX(), tcur->getY());

    // create the mouse event
    QGraphicsSceneMouseEvent * event = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseRelease);

    // set event parameters
    event->setScenePos(point);

    // use alt keyboard modifier to indicate this is a touch event
    event->setModifiers(Qt::AltModifier);

    // note that we shouldn't call setButtons() here, since the release will only trigger an "ungrab" of the mouse
    // if there are no other buttons set
    if(tcur->getCursorID() == 0)
    {
        event->setButton(Qt::LeftButton);
    }
    else if(tcur->getCursorID() == 1)
    {
        event->setButton(Qt::RightButton);
    }

    // post the event (thread-safe)
    QApplication::postEvent(graphicsViewProxy_->getGraphicsView()->scene(), event);

    // reset last point
    lastPoint_ = point;
}

void TouchListener::refresh(TUIO::TuioTime frameTime)
{

}
