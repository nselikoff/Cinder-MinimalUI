#pragma once

#include "Resources.h"
#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Timeline.h"
#include <vector>

using namespace ci;
using namespace ci::app;
using namespace std;


namespace MinimalUI {
    
    class UIElement;
    
    typedef std::shared_ptr<class UIController> UIControllerRef;
    typedef std::shared_ptr<class UIElement> UIElementRef;

    class UIController {
    public:

        static int DEFAULT_PANEL_WIDTH;
        static int DEFAULT_MARGIN_LARGE;
        static int DEFAULT_MARGIN_SMALL;
        static int DEFAULT_UPDATE_FREQUENCY;
        static int DEFAULT_FBO_WIDTH;
        static ci::ColorA DEFAULT_STROKE_COLOR;
        static ci::ColorA ACTIVE_STROKE_COLOR;

        UIController( ci::app::WindowRef window, const std::string &aParamString );
        static UIControllerRef create( const std::string &aParamString = "{}", ci::app::WindowRef aWindow = ci::app::App::get()->getWindow() );
        
        void mouseDown( ci::app::MouseEvent &event );

        void addSlider( const std::string &aName, float *aValueToLink, const std::string &aParamString = "{}" );
        void addSlider2D( const std::string &aName, ci::Vec2f *aValueToLink, const std::string &aParamString = "{}" );
        void addToggleSlider( const std::string &aSliderName, float *aValueToLink, const std::string &aButtonName, const std::function<void( bool )>& aEventHandler, const std::string &aSliderParamString = "{}", const std::string &aButtonParamString = "{}" );
        void addButton( const std::string &aName, const std::function<void( bool )>& aEventHandler, const std::string &aParamString = "{}" );
        void addLinkedButton( const std::string &aName, const std::function<void( bool )>& aEventHandler, bool *aLinkedState, const std::string &aParamString = "{}" );
        void addLabel( const std::string &aName, const std::string &aParamString = "{}" );
        void addSeparator() { mInsertPosition.x = mMarginLarge; mInsertPosition.y += mMarginLarge; }
        
        void offsetInsertPosition( const ci::Vec2i &offset ) { mInsertPosition += offset; }
        void resetInsertPosition( const int &offsetY ) { mInsertPosition = ci::Vec2i( mMarginLarge, mInsertPosition.y + offsetY ); }

        void draw();
        void update();
        void resize();
        
        void show();
        void hide();
        bool isVisible() { return mVisible; }
        
        void releaseGroup( const std::string &aGroup );

        ci::app::WindowRef getWindow() { return mWindow; }
        ci::Vec2i getInsertPosition() { return mInsertPosition; }
        ci::Font getFont( const std::string &aStyle );
        int getDepth() { return mDepth + mUIElements.size(); }
        int getWidth() { return mWidth; }
        ci::Vec2i getPosition() { return mPosition; }
        void setHeight() { mHeightSpecified = true; mHeight = mInsertPosition.y + mMarginLarge; resize(); }
        
    private:
        
        void setupFbo();
        
        ci::app::WindowRef mWindow;
        ci::signals::scoped_connection mCbMouseDown;
        std::string mParamString;

        bool mVisible;
        bool mCentered;
        bool mForceInteraction;
        bool mHeightSpecified;
        
        int mDepth;
        int mMarginLarge;
        
        std::vector<UIElementRef> mUIElements;
        int mWidth, mHeight, mX, mY;
        ci::Area mBounds;
        ci::Vec2i mPosition;
        ci::Vec2i mInsertPosition;
        ci::ColorA mPanelColor;
        ci::Font mLabelFont, mSmallLabelFont, mIconFont, mHeaderFont, mBodyFont, mFooterFont;
        
        ci::gl::Fbo mFbo;
        ci::gl::Fbo::Format mFormat;
        ci::Anim<float> mAlpha;
    };
    
}