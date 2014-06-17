/*********************************************************************/
/* Copyright (c) 2013, EPFL/Blue Brain Project                       */
/*                     Raphael Dumusc <raphael.dumusc@epfl.ch>       */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/*   1. Redistributions of source code must retain the above         */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer.                                                  */
/*                                                                   */
/*   2. Redistributions in binary form must reproduce the above      */
/*      copyright notice, this list of conditions and the following  */
/*      disclaimer in the documentation and/or other materials       */
/*      provided with the distribution.                              */
/*                                                                   */
/*    THIS  SOFTWARE IS PROVIDED  BY THE  UNIVERSITY OF  TEXAS AT    */
/*    AUSTIN  ``AS IS''  AND ANY  EXPRESS OR  IMPLIED WARRANTIES,    */
/*    INCLUDING, BUT  NOT LIMITED  TO, THE IMPLIED  WARRANTIES OF    */
/*    MERCHANTABILITY  AND FITNESS FOR  A PARTICULAR  PURPOSE ARE    */
/*    DISCLAIMED.  IN  NO EVENT SHALL THE UNIVERSITY  OF TEXAS AT    */
/*    AUSTIN OR CONTRIBUTORS BE  LIABLE FOR ANY DIRECT, INDIRECT,    */
/*    INCIDENTAL,  SPECIAL, EXEMPLARY,  OR  CONSEQUENTIAL DAMAGES    */
/*    (INCLUDING, BUT  NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE    */
/*    GOODS  OR  SERVICES; LOSS  OF  USE,  DATA,  OR PROFITS;  OR    */
/*    BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF    */
/*    LIABILITY, WHETHER  IN CONTRACT, STRICT  LIABILITY, OR TORT    */
/*    (INCLUDING NEGLIGENCE OR OTHERWISE)  ARISING IN ANY WAY OUT    */
/*    OF  THE  USE OF  THIS  SOFTWARE,  EVEN  IF ADVISED  OF  THE    */
/*    POSSIBILITY OF SUCH DAMAGE.                                    */
/*                                                                   */
/* The views and conclusions contained in the software and           */
/* documentation are those of the authors and should not be          */
/* interpreted as representing official policies, either expressed   */
/* or implied, of The University of Texas at Austin.                 */
/*********************************************************************/

#include "WebkitPixelStreamer.h"

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebView>
#include <QtWebKit/QWebElement>
#include <QKeyEvent>

#include <QTimer>

#include "log.h"
#include "WebkitAuthenticationHelper.h"
#include "WebkitHtmlSelectReplacer.h"

#define WEBPAGE_MIN_WIDTH      640
#define WEBPAGE_MIN_HEIGHT     512

#define WEBPAGE_DEFAULT_ZOOM   2.0

WebkitPixelStreamer::WebkitPixelStreamer(const QSize& webpageSize, const QString& url)
    : PixelStreamer()
    , authenticationHelper_(new WebkitAuthenticationHelper(webView_))
    , selectReplacer_(new WebkitHtmlSelectReplacer(webView_))
    , interactionModeActive_(false)
    , initialWidth_( std::max( webpageSize.width(), WEBPAGE_MIN_WIDTH ))
{
    setSize( webpageSize * WEBPAGE_DEFAULT_ZOOM );
    webView_.setZoomFactor(WEBPAGE_DEFAULT_ZOOM);

    QWebSettings* settings = webView_.settings();
    settings->setAttribute( QWebSettings::AcceleratedCompositingEnabled, true );
    settings->setAttribute( QWebSettings::JavascriptEnabled, true );
    settings->setAttribute( QWebSettings::PluginsEnabled, true );
    settings->setAttribute( QWebSettings::LocalStorageEnabled, true );
#if QT_VERSION >= 0x040800
    settings->setAttribute( QWebSettings::WebGLEnabled, true );
#endif

    setUrl(url);

    connect(&timer_, SIGNAL(timeout()), this, SLOT(update()));
    timer_.start(30);
}

WebkitPixelStreamer::~WebkitPixelStreamer()
{
    timer_.stop();
}

void WebkitPixelStreamer::setUrl(QString url)
{
    QMutexLocker locker(&mutex_);

    webView_.load( QUrl(url) );
}

const QWebView* WebkitPixelStreamer::getView() const
{
    return &webView_;
}

void WebkitPixelStreamer::processEvent(dc::Event dcEvent)
{
    QMutexLocker locker(&mutex_);

    switch(dcEvent.type)
    {
    case Event::EVT_CLICK:
        processClickEvent(dcEvent);
        break;
    case Event::EVT_PRESS:
        processPressEvent(dcEvent);
        break;
    case Event::EVT_MOVE:
        processMoveEvent(dcEvent);
        break;
    case Event::EVT_WHEEL:
        processWheelEvent(dcEvent);
        break;
    case Event::EVT_RELEASE:
        processReleaseEvent(dcEvent);
        break;
    case Event::EVT_SWIPE_LEFT:
        webView_.back();
        break;
    case Event::EVT_SWIPE_RIGHT:
        webView_.forward();
        break;
    case Event::EVT_KEY_PRESS:
        processKeyPress(dcEvent);
        break;
    case Event::EVT_KEY_RELEASE:
        processKeyRelease(dcEvent);
        break;
    case Event::EVT_VIEW_SIZE_CHANGED:
        processViewSizeChange(dcEvent);
        break;
    default:
        break;
    }
}

void WebkitPixelStreamer::processClickEvent(const Event &clickEvent)
{
    // TODO History navigation (until swipe gestures are fixed)
    if (clickEvent.mouseX < 0.02)
    {
        webView_.back();
        return;
    }
    if (clickEvent.mouseX > 0.98)
    {
        webView_.forward();
        return;
    }

    processPressEvent(clickEvent);
    processReleaseEvent(clickEvent);

    const QWebHitTestResult& hitResult = performHitTest( clickEvent );
    if( !hitResult.isNull() && !hitResult.linkUrl().isEmpty( ))
        webView_.load( hitResult.linkUrl( ));
}

void WebkitPixelStreamer::processPressEvent(const Event &pressEvent)
{
    const QWebHitTestResult& hitResult = performHitTest(pressEvent);

    if(hitResult.isNull() || isWebGLElement(hitResult.element()))
    {
        interactionModeActive_ = true;
    }

    const QPoint& pointerPos = getPointerPosition(pressEvent);

    QMouseEvent myEvent(QEvent::MouseButtonPress, pointerPos,
                        Qt::LeftButton, Qt::LeftButton,
                        (Qt::KeyboardModifiers)pressEvent.modifiers);

    webView_.page()->event(&myEvent);
}


void WebkitPixelStreamer::processMoveEvent(const Event &moveEvent)
{
    const QPoint& pointerPos = getPointerPosition(moveEvent);

    if( interactionModeActive_ )
    {
        QMouseEvent myEvent(QEvent::MouseMove, pointerPos,
                            Qt::LeftButton, Qt::LeftButton,
                            (Qt::KeyboardModifiers)moveEvent.modifiers);

        webView_.page()->event(&myEvent);
    }
    else
    {
        QWebFrame *pFrame = webView_.page()->frameAt(pointerPos);
        if (!pFrame)
            return;

        int dx = moveEvent.dx * webView_.page()->viewportSize().width();
        int dy = moveEvent.dy * webView_.page()->viewportSize().height();

        pFrame->scroll(-dx,-dy);
    }
}

void WebkitPixelStreamer::processReleaseEvent(const Event &releaseEvent)
{
    const QPoint& pointerPos = getPointerPosition(releaseEvent);

    QMouseEvent myEvent(QEvent::MouseButtonRelease, pointerPos,
                        Qt::LeftButton, Qt::LeftButton,
                        (Qt::KeyboardModifiers)releaseEvent.modifiers);

    webView_.page()->event(&myEvent);

    interactionModeActive_ = false;
}

void WebkitPixelStreamer::processWheelEvent(const Event &wheelEvent)
{
    const QWebHitTestResult& hitResult = performHitTest(wheelEvent);

    if(!hitResult.isNull() && isWebGLElement(hitResult.element()))
    {
        QWheelEvent myEvent(hitResult.pos(), (int)wheelEvent.dy, Qt::NoButton,
                            (Qt::KeyboardModifiers)wheelEvent.modifiers,
                            Qt::Vertical);

        webView_.page()->event(&myEvent);
    }
}

void WebkitPixelStreamer::processKeyPress(const Event& keyEvent)
{
    QKeyEvent myEvent(QEvent::KeyPress, keyEvent.key,
                      (Qt::KeyboardModifiers)keyEvent.modifiers,
                      QString::fromStdString(keyEvent.text)
                      );
    webView_.page()->event(&myEvent);
}

void WebkitPixelStreamer::processKeyRelease(const Event &keyEvent)
{
    QKeyEvent myEvent(QEvent::KeyRelease, keyEvent.key,
                      (Qt::KeyboardModifiers)keyEvent.modifiers,
                      QString::fromStdString(keyEvent.text)
                      );
    webView_.page()->event(&myEvent);
}

void WebkitPixelStreamer::processViewSizeChange(const Event &sizeEvent)
{
    setSize( QSize((int)sizeEvent.dx, (int)sizeEvent.dy) );
    recomputeZoomFactor();
}

void WebkitPixelStreamer::setSize(const QSize& webpageSize)
{
    QSize newSize( std::max(webpageSize.width(), WEBPAGE_MIN_WIDTH), std::max(webpageSize.height(), WEBPAGE_MIN_HEIGHT) );

    webView_.page()->setViewportSize( newSize );
}

void WebkitPixelStreamer::recomputeZoomFactor()
{
    webView_.setZoomFactor( qreal(size().width()) / qreal(initialWidth_) );
}

QSize WebkitPixelStreamer::size() const
{
    return webView_.page()->viewportSize();
}

void WebkitPixelStreamer::update()
{
    QMutexLocker locker(&mutex_);

    QWebPage* page = webView_.page();
    if( !page->viewportSize().isEmpty())
    {
        if (image_.size() != page->viewportSize())
            image_ = QImage( page->viewportSize(), QImage::Format_ARGB32 );

        QPainter painter( &image_ );
        page->mainFrame()->render( &painter );
        painter.end();

        emit imageUpdated(image_);
    }
}

QWebHitTestResult WebkitPixelStreamer::performHitTest(const Event &dcEvent) const
{
    const QPoint& pointerPos = getPointerPosition(dcEvent);
    QWebFrame *pFrame = webView_.page()->frameAt(pointerPos);
    return pFrame ? pFrame->hitTestContent(pointerPos) : QWebHitTestResult();
}

QPoint WebkitPixelStreamer::getPointerPosition(const Event &dcEvent) const
{
    QWebPage* page = webView_.page();

    int x = dcEvent.mouseX * page->viewportSize().width();
    int y = dcEvent.mouseY * page->viewportSize().height();

    x = std::max(0, std::min(x, page->viewportSize().width()-1));
    y = std::max(0, std::min(y, page->viewportSize().height()-1));

    return QPoint(x, y);
}

bool WebkitPixelStreamer::isWebGLElement(const QWebElement& element) const
{
    return element.tagName() == "CANVAS";
}
