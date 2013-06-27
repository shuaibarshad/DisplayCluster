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

#include <QtWebKit/QWebFrame>
#include <QtWebKit/QWebView>

Texture::Texture(std::string uri)
{
    // defaults
    textureBound_ = false;

    gotContent_ = false;

    // assign values
    uri_ = uri;


    uri = "http://www.greensock.com/gsap-js/";
    const QUrl url(  QString::fromStdString( uri ));
    if(url.isValid() && !url.isLocalFile())
    {
        webView_ = new QWebView;
        webView_->load( url );
        connect( webView_->page(), SIGNAL(repaintRequested(QRect)), this, SLOT(gotContent()));
        imageWidth_ = imageHeight_ = 0;
    }
    else
    {
        QImage image;
        image = QImage(uri_.c_str());

        if(image.isNull() == true)
        {
            put_flog(LOG_ERROR, "error loading %s", uri_.c_str());
            return;
        }

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

    if( webView_ && gotContent_ )
    {
        QWebPage* page = webView_->page();
        page->setViewportSize( page->mainFrame()->contentsSize());
        if( !page->viewportSize().isEmpty())
        {
            QImage image = QImage( page->viewportSize(), QImage::Format_RGB32 );

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
                image = QGLWidget::convertToGLFormat(image);
                glBindTexture( GL_TEXTURE_2D, textureId_ );
                glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, image.width(),
                                 image.height(), GL_RGBA, GL_UNSIGNED_BYTE,
                                 image.bits( ));
            }
            //gotContent_ = false;
        }
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
        glTexCoord2f(tX,1.-tY);
        glVertex2f(0.,0.);

        glTexCoord2f(tX+tW,1.-tY);
        glVertex2f(1.,0.);

        glTexCoord2f(tX+tW,1.-(tY+tH));
        glVertex2f(1.,1.);

        glTexCoord2f(tX,1.-(tY+tH));
        glVertex2f(0.,1.);

        glEnd();

        glPopAttrib();
    }
}
