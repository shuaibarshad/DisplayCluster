#ifndef CONTENT_WINDOW_H
#define CONTENT_WINDOW_H

#include "ContentWindowInterface.h"
#include <QtGui>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/weak_ptr.hpp>

class Content;
class DisplayGroup;
class ContentWindowGraphicsItem;

class ContentWindow : public ContentWindowInterface, public boost::enable_shared_from_this<ContentWindow> {

    public:

        ContentWindow() { } // no-argument constructor required for serialization
        ContentWindow(boost::shared_ptr<Content> content);

        boost::shared_ptr<Content> getContent();

        boost::shared_ptr<DisplayGroup> getDisplayGroup();
        void setDisplayGroup(boost::shared_ptr<DisplayGroup> displayGroup);

        // re-implemented ContentWindowInterface slots
        void moveToFront(ContentWindowInterface * source=NULL);
        void close(ContentWindowInterface * source=NULL);

        // GLWindow rendering
        void render();

        // button dimensions
        void getButtonDimensions(float &width, float &height);

        // interfaces

        // regular pointer used since Qt will own the object
        ContentWindowGraphicsItem * getContentWindowGraphicsItem();

    protected:
        friend class boost::serialization::access;

        template<class Archive>
        void serialize(Archive & ar, const unsigned int version)
        {
            ar & content_;
            ar & displayGroup_;
            ar & contentWidth_;
            ar & contentHeight_;
            ar & x_;
            ar & y_;
            ar & w_;
            ar & h_;
            ar & centerX_;
            ar & centerY_;
            ar & zoom_;
            ar & selected_;
        }

    private:

        boost::shared_ptr<Content> content_;

        boost::weak_ptr<DisplayGroup> displayGroup_;
};

#endif