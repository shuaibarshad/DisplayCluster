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

#ifndef DYNAMIC_TEXTURE_H
#define DYNAMIC_TEXTURE_H

#include "FactoryObject.h"
#include "types.h"

#include <QImage>
#include <QFuture>
#include <QRectF>
#include <QGLWidget>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

/**
 * A dynamically loaded large scale image.
 *
 * It can work with two types of image files:
 * (1) A custom precomuted image pyramid (recommended)
 * (2) Direct reading from a large image
 * @see generateImagePyramid()
 */
class DynamicTexture : public boost::enable_shared_from_this<DynamicTexture>, public FactoryObject
{
public:
    /**
     * Constructor
     * @param uri The uri of an image or of a Pyramid metadata file
     * @param parent Internal use: child objects need to keep a weak pointer to their parent
     * @param parentCoordinates Internal use: texture coordinates in the parent texture
     * @param childIndex Internal use: index of the child object
     */
    DynamicTexture(const QString& uri = "", DynamicTexturePtr parent = DynamicTexturePtr(),
                   const QRectF& parentCoordinates = QRectF(), const int childIndex = 0);

    /** Destructor */
    ~DynamicTexture();

    /** The exension of pyramid metadata files */
    static const QString pyramidFileExtension;

    /** The standard suffix for pyramid image folders */
    static const QString pyramidFolderSuffix;

    /**
     * Get the dimensions of the full resolution texture.
     * @param width Returned width
     * @param height Returned height
     */
    void getDimensions(int &width, int &height);

    /**
     * Render the dynamic texture.
     * @param texCoords The area of the full scale texture to render
     * @param loadOnDemand Load the texture if not already available
     * @param considerChildren Attempt to use the children objects for rendering
     */
    void render(const QRectF& texCoords, bool loadOnDemand=true, bool considerChildren=true);

    /**
     * Recursively clear children of this object which have not been rendered recently.
     * @param currentFrameIndex The current frame index
     */
    void clearOldChildren(const uint64_t currentFrameIndex);

    /**
     * Generate an image Pyramid from the current uri and save it to the disk.
     * @param baseFolder The folder in which the metadata and pyramid images will be created.
     */
    bool generateImagePyramid(const QString& baseFolder);

    /**
     * Load the image for this part of the texture
     * @param convertToGLFormat convert the image to GL format for texture upload
     * @throw boost::bad_weak_ptr exception if a parent object is deleted during thread execution
     * @internal asynchronous loading thread needs access to this method
     */
    void loadImage(const bool convertToGLFormat=true);

    /**
     * Decrement the global count of loading threads.
     * @throw boost::bad_weak_ptr exception if a parent object is deleted during thread execution
     * @internal asynchronous loading thread needs access to this method
     */
    void decrementGlobalThreadCount();

private:
    /* for root only: */

    QString uri_;

    QString imagePyramidPath_;
    bool useImagePyramid_;

    int threadCount_;
    QMutex threadCountMutex_;

    QImage fullscaleImage_;

    /* for children only: */

    boost::weak_ptr<DynamicTexture> parent_;
    QRectF imageCoordsInParentImage_;

    /* for all objects: */

    std::vector<int> treePath_; // To construct the image name for each object
    int depth_; // The depth of the object in the image pyramid

    QFuture<void> loadImageThread_; // Future for asychronous image loading
    bool loadImageThreadStarted_; // True if the texture loading has been started

    QImage scaledImage_; // for texture upload to GPU
    GLuint textureId_;
    QSize imageSize_; // full scale image dimensions

    std::vector<DynamicTexturePtr> children_; // Children in the image pyramid
    uint64_t renderChildrenFrameIndex_; // Used for garbage-collecting unused child objects


    /** Is this object the root element. */
    bool isRoot() const;

    /**
     * Get the root object,
     * @return A valid DynamicTexturePtr to the root element
     * @throw boost::bad_weak_ptr exception if the root object is deleted during thread execution
     */
    DynamicTexturePtr getRoot(); // @Child only

    bool readPyramidMetadataFromFile(const QString& uri); // @Root only
    bool makePyramidFolder(const QString& pyramidFolder); // @Root only
    bool writeMetadataFile(const QString& pyramidFolder, const QString& filename) const; // @Root only
    bool writePyramidMetadataFiles(const QString& pyramidFolder) const; // @Root only
    QString getPyramidImageFilename() const; // @All

    QRectF getImageRegionInParentImage(const QRectF& imageRegion) const;

    void loadImageAsync(); // Trigger the loading of the image in a separate thread // @All
    bool loadFullResImage(); // @Root only
    QImage loadImageRegionFromFullResImageFile(const QString& filename); // @Child only
    QImage getImageFromParent(const QRectF& imageRegion, DynamicTexture * start); // @Child only
    void uploadTexture(); // @All

    void renderChildren(const QRectF& texCoords); // @All
    double getProjectedPixelArea(const bool onScreenOnly); // Used to determine children visibility // @All
    void renderTextureBorder(); // @All
    void renderTexturedUnitQuad(const QRectF& texCoords); // @All

    bool getThreadsDoneDescending(); // Used by clearOldChildren() // @Root

    int getGlobalThreadCount(); // Get the global count of loading threads // @Root
    void incrementGlobalThreadCount(); // Increment the global cound of loading threads // @Root

    QRect getRootImageCoordinates(float x, float y, float w, float h); // @TODO-Remove
};

#endif
