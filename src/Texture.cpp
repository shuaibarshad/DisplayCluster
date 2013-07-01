/*********************************************************************/
/* Copyright (c) 2011 - 2012, The University of Texas at Austin.     */
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

#include "Texture.h"
#include "main.h"
#include "log.h"

#ifdef qtwebkit
#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebView>
#else
#include "berkelium/Berkelium.hpp"
#include "berkelium/Window.hpp"
#include "berkelium/WindowDelegate.hpp"
#include "berkelium/Context.hpp"
#endif

#ifndef qtwebkit
#define DEBUG_PAINT false

bool mapOnPaintToTexture(
    Berkelium::Window *wini,
    const unsigned char* bitmap_in, const Berkelium::Rect& bitmap_rect,
    size_t num_copy_rects, const Berkelium::Rect *copy_rects,
    int dx, int dy,
    const Berkelium::Rect& scroll_rect,
    unsigned int dest_texture,
    unsigned int dest_texture_width,
    unsigned int dest_texture_height,
    bool ignore_partial,
    char* scroll_buffer) {

    glBindTexture(GL_TEXTURE_2D, dest_texture);

    const int kBytesPerPixel = 4;

    // If we've reloaded the page and need a full update, ignore updates
    // until a full one comes in.  This handles out of date updates due to
    // delays in event processing.
    if (ignore_partial) {
        if (bitmap_rect.left() != 0 ||
            bitmap_rect.top() != 0 ||
            bitmap_rect.right() != dest_texture_width ||
            bitmap_rect.bottom() != dest_texture_height) {
            return false;
        }

        glTexImage2D(GL_TEXTURE_2D, 0, kBytesPerPixel, dest_texture_width, dest_texture_height, 0,
            GL_BGRA, GL_UNSIGNED_BYTE, bitmap_in);
        ignore_partial = false;
        return true;
    }


    // Now, we first handle scrolling. We need to do this first since it
    // requires shifting existing data, some of which will be overwritten by
    // the regular dirty rect update.
    if (dx != 0 || dy != 0) {
        // scroll_rect contains the Rect we need to move
        // First we figure out where the the data is moved to by translating it
        Berkelium::Rect scrolled_rect = scroll_rect.translate(-dx, -dy);
        // Next we figure out where they intersect, giving the scrolled
        // region
        Berkelium::Rect scrolled_shared_rect = scroll_rect.intersect(scrolled_rect);
        // Only do scrolling if they have non-zero intersection
        if (scrolled_shared_rect.width() > 0 && scrolled_shared_rect.height() > 0) {
            // And the scroll is performed by moving shared_rect by (dx,dy)
            Berkelium::Rect shared_rect = scrolled_shared_rect.translate(dx, dy);

            int wid = scrolled_shared_rect.width();
            int hig = scrolled_shared_rect.height();
            if (DEBUG_PAINT) {
              std::cout << "Scroll rect: w=" << wid << ", h=" << hig << ", ("
                        << scrolled_shared_rect.left() << "," << scrolled_shared_rect.top()
                        << ") by (" << dx << "," << dy << ")" << std::endl;
            }
            int inc = 1;
            char *outputBuffer = scroll_buffer;
            // source data is offset by 1 line to prevent memcpy aliasing
            // In this case, it can happen if dy==0 and dx!=0.
            char *inputBuffer = scroll_buffer+(dest_texture_width*1*kBytesPerPixel);
            int jj = 0;
            if (dy > 0) {
                // Here, we need to shift the buffer around so that we start in the
                // extra row at the end, and then copy in reverse so that we
                // don't clobber source data before copying it.
                outputBuffer = scroll_buffer+(
                    (scrolled_shared_rect.top()+hig+1)*dest_texture_width
                    - hig*wid)*kBytesPerPixel;
                inputBuffer = scroll_buffer;
                inc = -1;
                jj = hig-1;
            }

            // Copy the data out of the texture
            glGetTexImage(
                GL_TEXTURE_2D, 0,
                GL_BGRA, GL_UNSIGNED_BYTE,
                inputBuffer
            );

            // Annoyingly, OpenGL doesn't provide convenient primitives, so
            // we manually copy out the region to the beginning of the
            // buffer
            for(; jj < hig && jj >= 0; jj+=inc) {
                memcpy(
                    outputBuffer + (jj*wid) * kBytesPerPixel,
//scroll_buffer + (jj*wid * kBytesPerPixel),
                    inputBuffer + (
                        (scrolled_shared_rect.top()+jj)*dest_texture_width
                        + scrolled_shared_rect.left()) * kBytesPerPixel,
                    wid*kBytesPerPixel
                );
            }

            // And finally, we push it back into the texture in the right
            // location
            glTexSubImage2D(GL_TEXTURE_2D, 0,
                shared_rect.left(), shared_rect.top(),
                shared_rect.width(), shared_rect.height(),
                GL_BGRA, GL_UNSIGNED_BYTE, outputBuffer
            );
        }
    }

    if (DEBUG_PAINT) {
      std::cout << (void*)wini << " Bitmap rect: w="
                << bitmap_rect.width()<<", h="<<bitmap_rect.height()
                <<", ("<<bitmap_rect.top()<<","<<bitmap_rect.left()
                <<") tex size "<<dest_texture_width<<"x"<<dest_texture_height
                <<std::endl;
    }
    for (size_t i = 0; i < num_copy_rects; i++) {
        int wid = copy_rects[i].width();
        int hig = copy_rects[i].height();
        int top = copy_rects[i].top() - bitmap_rect.top();
        int left = copy_rects[i].left() - bitmap_rect.left();
        if (DEBUG_PAINT) {
            std::cout << (void*)wini << " Copy rect: w=" << wid << ", h=" << hig << ", ("
                      << top << "," << left << ")" << std::endl;
        }
        for(int jj = 0; jj < hig; jj++) {
            memcpy(
                scroll_buffer + jj*wid*kBytesPerPixel,
                bitmap_in + (left + (jj+top)*bitmap_rect.width())*kBytesPerPixel,
                wid*kBytesPerPixel
                );
        }

        // Finally, we perform the main update, just copying the rect that is
        // marked as dirty but not from scrolled data.
        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        copy_rects[i].left(), copy_rects[i].top(),
                        wid, hig,
                        GL_BGRA, GL_UNSIGNED_BYTE, scroll_buffer
            );
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

class GLTextureWindow : public Berkelium::WindowDelegate {
public:
    GLTextureWindow(unsigned int _w, unsigned int _h, bool _usetrans, Texture* t )
     : width(_w),
       height(_h),
       texture(t),
       needs_full_refresh(true)
    {
        // Create texture to hold rendered view
        glGenTextures(1, &t->textureId_);
        glBindTexture(GL_TEXTURE_2D, t->textureId_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        scroll_buffer = new char[width*(height+1)*4];

        Berkelium::Context *context = Berkelium::Context::create();
        bk_window = Berkelium::Window::create(context);
        delete context;
        bk_window->setDelegate(this);
        bk_window->resize(width, height);
        bk_window->setTransparent(_usetrans);
    }

    ~GLTextureWindow() {
        glDeleteTextures(1, &texture->textureId_);
        delete [] scroll_buffer;
        bk_window->destroy();
    }

    Berkelium::Window* getWindow() {
        return bk_window;
    }

    void clear() {
        // Black out the page
        unsigned char black = 0;
        glBindTexture(GL_TEXTURE_2D, texture->textureId_);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, 1, 1, 0,
            GL_LUMINANCE, GL_UNSIGNED_BYTE, &black);

        needs_full_refresh = true;
    }

    void bind() {
        glEnable (GL_BLEND);
        glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, texture->textureId_);
    }

    void release() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    virtual void onPaint(Berkelium::Window *wini,
        const unsigned char *bitmap_in, const Berkelium::Rect &bitmap_rect,
        size_t num_copy_rects, const Berkelium::Rect *copy_rects,
        int dx, int dy, const Berkelium::Rect &scroll_rect) {

        bool updated = mapOnPaintToTexture(
            wini, bitmap_in, bitmap_rect, num_copy_rects, copy_rects,
            dx, dy, scroll_rect,
            texture->textureId_, width, height, needs_full_refresh, scroll_buffer
        );
        if (updated) {
            needs_full_refresh = false;
        }
    }

    virtual void onAddressBarChanged(Berkelium::Window *win, Berkelium::URLString newURL) {
        std::cout << (void*)win << "*** onAddressBarChanged " << newURL << std::endl;
    }
    virtual void onStartLoading(Berkelium::Window *win, Berkelium::URLString newURL) {
        std::cout << (void*)win << "*** onStartLoading " << newURL << std::endl;
    }
    virtual void onLoad(Berkelium::Window *win) {
        std::wcout << (void*)win << L"*** onLoad " << std::endl;
    }
    virtual void onCrashedWorker(Berkelium::Window *win) {
        std::wcout << (void*)win << L"*** onCrashedWorker " << std::endl;
    }
    virtual void onCrashedPlugin(Berkelium::Window *win, Berkelium::WideString pluginName) {
        std::wcout << L"*** onCrashedPlugin " << pluginName << std::endl;
    }
    virtual void onProvisionalLoadError(Berkelium::Window *win, Berkelium::URLString url,
                                        int errorCode, bool isMainFrame) {
        std::cout << "*** onProvisionalLoadError " << url << ": " << errorCode;
        if (isMainFrame) std::cout << " (main frame)";
        std::cout << std::endl;
    }
    virtual void onConsoleMessage(Berkelium::Window *win, Berkelium::WideString message,
                                  Berkelium::WideString sourceId, int line_no) {
        std::wcout << L"*** onConsoleMessage " << message << L" from "
                   << sourceId << L" line " << line_no << std::endl;
    }
    virtual void onScriptAlert(Berkelium::Window *win, Berkelium::WideString message,
                              Berkelium::WideString defaultValue, Berkelium::URLString url,
                              int flags, bool &success, Berkelium::WideString &value) {
        std::wcout << L"*** onScriptAlert " << message << std::endl;
    }
    virtual void onNavigationRequested(Berkelium::Window *win, Berkelium::URLString newURL,
                                       Berkelium::URLString referrer, bool isNewWindow,
                                       bool &cancelDefaultAction) {
        std::cout << (void*)win << "*** onNavigationRequested " << newURL << " by " << referrer
                  << (isNewWindow?"  (new window)" : " (same window)") << std::endl;
    }
    virtual void onLoadingStateChanged(Berkelium::Window *win, bool isLoading) {
        std::cout << (void*)win << "*** onLoadingStateChanged "
                  << (isLoading?"started":"stopped") << std::endl;
    }
    virtual void onTitleChanged(Berkelium::Window *win, Berkelium::WideString title) {
        std::wcout << L"*** onTitleChanged " << title << std::endl;
    }
    virtual void onTooltipChanged(Berkelium::Window *win, Berkelium::WideString text) {
        std::wcout << L"*** onTooltipChanged " << text << std::endl;
    }
    virtual void onCrashed(Berkelium::Window *win) {
        std::cout << "*** onCrashed " << std::endl;
    }
    virtual void onUnresponsive(Berkelium::Window *win) {
        std::cout << "*** onUnresponsive " << std::endl;
    }
    virtual void onResponsive(Berkelium::Window *win) {
        std::cout << "*** onResponsive " << std::endl;
    }
    virtual void onCreatedWindow(Berkelium::Window *win, Berkelium::Window *newWindow,
                                 const Berkelium::Rect &initialRect) {
        std::cout << (void*)win << "*** onCreatedWindow " << (void*)newWindow<<" "
                  << initialRect.mLeft << "," << initialRect.mTop << ": "
                  << initialRect.mWidth << "x" << initialRect.mHeight << std::endl;
        if (initialRect.mWidth < 1 || initialRect.mHeight < 1) {
            newWindow->resize(this->width, this->height);
        }
        newWindow->setDelegate(this);
    }
    virtual void onWidgetCreated(Berkelium::Window *win, Berkelium::Widget *newWidget, int zIndex) {
        std::cout << "*** onWidgetCreated " << newWidget << " index " << zIndex << std::endl;
    }
    virtual void onWidgetResize(Berkelium::Window *win, Berkelium::Widget *wid, int newWidth, int newHeight) {
        std::cout << "*** onWidgetResize " << wid << " "
                  << newWidth << "x" << newHeight << std::endl;
    }
    virtual void onWidgetMove(Berkelium::Window *win, Berkelium::Widget *wid, int newX, int newY) {
        std::cout << "*** onWidgetMove " << wid << " "
                  << newX << "," << newY << std::endl;
    }
    virtual void onShowContextMenu(Berkelium::Window *win,
                                   const Berkelium::ContextMenuEventArgs& args) {
        std::cout << "*** onShowContextMenu at " << args.mouseX << "," << args.mouseY;
        std::cout << "; type: ";
        switch (args.mediaType) {
          case Berkelium::ContextMenuEventArgs::MediaTypeImage:
            std::cout << "image";
            break;
          case Berkelium::ContextMenuEventArgs::MediaTypeVideo:
            std::cout << "video";
            break;
          case Berkelium::ContextMenuEventArgs::MediaTypeAudio:
            std::cout << "audio";
            break;
          default:
            std::cout << "other";
        }
        if (args.isEditable || args.editFlags) {
            std::cout << " (";
            if (args.isEditable)
                std::cout << "editable; ";
            if (args.editFlags & Berkelium::ContextMenuEventArgs::CanUndo)
                std::cout << "Undo, ";
            if (args.editFlags & Berkelium::ContextMenuEventArgs::CanRedo)
                std::cout << "Redo, ";
            if (args.editFlags & Berkelium::ContextMenuEventArgs::CanCut)
                std::cout << "Cut, ";
            if (args.editFlags & Berkelium::ContextMenuEventArgs::CanCopy)
                std::cout << "Copy, ";
            if (args.editFlags & Berkelium::ContextMenuEventArgs::CanPaste)
                std::cout << "Paste, ";
            if (args.editFlags & Berkelium::ContextMenuEventArgs::CanDelete)
                std::cout << "Delete, ";
            if (args.editFlags & Berkelium::ContextMenuEventArgs::CanSelectAll)
                std::cout << "Select All";
            std::cout << ")";
        }
        std::cout << std::endl;
        if (args.linkUrl.length())
            std::cout << "        Link URL: " << args.linkUrl << std::endl;
        if (args.srcUrl.length())
            std::cout << "        Source URL: " << args.srcUrl << std::endl;
        if (args.pageUrl.length())
            std::cout << "        Page URL: " << args.pageUrl << std::endl;
        if (args.frameUrl.length())
            std::cout << "        Frame URL: " << args.frameUrl << std::endl;
        if (args.selectedText.length())
            std::wcout << L"        Selected Text: " << args.selectedText << std::endl;
    }

//    virtual void onJavascriptCallback(Berkelium::Window *win, void* replyMsg, Berkelium::URLString url, Berkelium::WideString funcName, Berkelium::Script::Variant *args, size_t numArgs) {
//        std::cout << "*** onJavascriptCallback at URL " << url << ", "
//                  << (replyMsg?"synchronous":"async") << std::endl;
//        std::wcout << L"    Function name: " << funcName << std::endl;
//        for (size_t i = 0; i < numArgs; i++) {
//            Berkelium::WideString jsonStr = Berkelium::toJSON(args[i]);
//            std::wcout << L"    Argument " << i << ": ";
//            if (args[i].type() == Berkelium::Script::Variant::JSSTRING) {
//                std::wcout << L"(string) " << args[i].toString() << std::endl;
//            } else {
//                std::wcout << jsonStr << std::endl;
//            }
//            Berkelium::Script::toJSON_free(jsonStr);
//        }
//        if (replyMsg) {
//            win->synchronousScriptReturn(replyMsg, numArgs ? args[0] : Berkelium::Script::Variant());
//        }
//    }

    /** Display a file chooser dialog, if necessary. The value to be returned should go ______.
     * \param win  Window instance that fired this event.
     * \param mode  Type of file chooser expected. See FileChooserType.
     * \param title  Title for dialog. "Open" or "Save" should be used if empty.
     * \param defaultFile  Default file to select in dialog.
     */
    virtual void onRunFileChooser(Berkelium::Window *win, int mode, Berkelium::WideString title, Berkelium::FileString defaultFile) {
        std::wcout << L"*** onRunFileChooser type " << mode << L", title " << title << L":" << std::endl;
#ifdef _WIN32
        std::wcout <<
#else
        std::cout <<
#endif
            defaultFile << std::endl;

        win->filesSelected(NULL);
    }

    virtual void onExternalHost(
        Berkelium::Window *win,
        Berkelium::WideString message,
        Berkelium::URLString origin,
        Berkelium::URLString target)
    {
        std::cout << "*** onExternalHost at URL from "<<origin<<" to "<<target<<":"<<std::endl;
        std::wcout << message<<std::endl;
    }

    Berkelium::Window* window() const {
        return bk_window;
    }

private:
    // The Berkelium window, i.e. our web page
    Berkelium::Window* bk_window;
    // Width and height of our window.
    unsigned int width, height;
    Texture* texture;
    // Bool indicating when we need to refresh the entire image
    bool needs_full_refresh;
    // Buffer used to store data for scrolling
    char* scroll_buffer;
};
#endif

Texture::Texture( const std::string& uri )
{
    // defaults
    textureBound_ = false;

    // assign values
    uri_ = uri;
    webView_ = 0;

    QImage image(uri_.c_str());

    if(image.isNull())
    {
#ifdef qtwebkit
        const QUrl url(  QString::fromStdString( uri ));
        webView_ = new QWebView;
        webView_->load( url );
        imageWidth_ = imageHeight_ = 0;
#else
        imageWidth_ = 1920;
        imageHeight_ = 1200;
        webView_ = new GLTextureWindow( 1920, 1200, false, this );
        webView_->window()->navigateTo(Berkelium::URLString::point_to(uri.data(), uri.length()));
#endif
    }
    else
    {
        image = QGLWidget::convertToGLFormat(image);

        // save image dimensions
        imageWidth_ = image.width();
        imageHeight_ = image.height();

        // generate new texture
        textureId_ = g_mainWindow->getGLWindow()->bindTexture(image, GL_TEXTURE_2D, GL_RGBA, QGLContext::DefaultBindOption);
        textureBound_ = true;
    }
}

Texture::~Texture()
{
    delete webView_;
    // delete bound texture
    if(textureBound_ == true)
    {
        glDeleteTextures(1, &textureId_); // it appears deleteTexture() below is not actually deleting the texture from the GPU...
        g_mainWindow->getGLWindow()->deleteTexture(textureId_);
        textureBound_ = false;
    }
}

void Texture::getDimensions(int &width, int &height)
{
    width = imageWidth_;
    height = imageHeight_;
}

void Texture::render(float tX, float tY, float tW, float tH)
{
    updateRenderedFrameCount();

    if( webView_ )
    {
#ifdef qtwebkit
        QWebPage* page = webView_->page();
        page->setViewportSize( page->mainFrame()->contentsSize());
        if( !page->viewportSize().isEmpty())
        {
            QImage image = QImage( page->viewportSize(), QImage::Format_ARGB32 );

            QPainter painter( &image );
            page->mainFrame()->render( &painter );
            painter.end();
            if( !textureBound_ || imageWidth_ != image.width() || imageHeight_ != image.height() )
            {
                imageWidth_ = image.width();
                imageHeight_ = image.height();
                textureId_ = g_mainWindow->getGLWindow()->bindTexture(image, GL_TEXTURE_2D, GL_RGBA, QGLContext::InvertedYBindOption);
                textureBound_ = true;
            }
            else
            {
                glBindTexture( GL_TEXTURE_2D, textureId_ );
                glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, image.width(),
                                 image.height(), GL_RGBA, GL_UNSIGNED_BYTE,
                                 image.bits( ));
            }
        }
#else
        Berkelium::update();

        webView_->bind();
        textureBound_ = true;
#endif
    }

    if(textureBound_ == true)
    {
        // draw the texture
        glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureId_);

        // on zoom-out, clamp to edge (instead of showing the texture tiled / repeated)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBegin(GL_QUADS);

        // note we need to flip the y coordinate since the textures are loaded upside down
        glTexCoord2f(tX,tY);
        glVertex2f(0.,0.);

        glTexCoord2f(tX+tW,tY);
        glVertex2f(1.,0.);

        glTexCoord2f(tX+tW,tY+tH);
        glVertex2f(1.,1.);

        glTexCoord2f(tX,tY+tH);
        glVertex2f(0.,1.);

        glEnd();

        glPopAttrib();
    }

    if( webView_ )
    {
#ifndef qtwebkit
        webView_->release();
        textureBound_ = false;
#endif
    }
}
