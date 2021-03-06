#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Text.h"
#include "cinder/Json.h"

namespace MinimalUI {
	
	class UIController;

	class UIElement {
	public:
		UIElement( UIController *aUIController, const std::string &aName, const std::string &aParamString );
		virtual ~UIElement() { }
		
		void offsetInsertPosition();
		void setPositionAndBounds();
		
		void renderNameTexture();
		
		std::string getGroup() const { return mGroup; }
		
		void setLocked( const bool &locked ) { mLocked = locked; }
		
		UIController* getParent() const { return mParent; }

		ci::Vec2i getPosition() const { return ci::app::toPixels( mPosition ); }
		void setPosition( const ci::Vec2i &aPosition ) { mPosition = aPosition; }
		
		ci::Vec2i getSize() const { return ci::app::toPixels( mSize ); }
		void setSize( const ci::Vec2i &aSize ) { mSize = aSize; }
		
		ci::Area getBounds() const { return ci::app::toPixels( mBounds ); }
		void setBounds( const ci::Area &aBounds ) { mBounds = aBounds; }
		
		bool hasParam( const std::string &aName ) const { return mParams.hasChild( aName ); }
		template <class T>
		T getParam( const std::string &aName ) const { return mParams[aName].getValue<T>(); }
		
		bool isActive() const { return mActive; }
		void setActive( const bool &aActive ) { mActive = aActive; }
		void deactivate() { mActive = false; }
		bool isLocked() const { return mLocked; }
		
		ci::gl::Texture getBackgroundTexture() const { return mBackgroundTexture; }
		void setBackgroundTexture( const ci::gl::Texture &aBackgroundTexture ) { mBackgroundTexture = aBackgroundTexture; }
		
		ci::ColorA getBackgroundColor() const { return mBackgroundColor; }
		void setBackgroundColor( const ci::ColorA &aBackgroundColor ) { mBackgroundColor = aBackgroundColor; }
		
		ci::ColorA getForegroundColor() const { return mForegroundColor; }
		void setForegroundColor( const ci::ColorA &aForegroundColor ) { mForegroundColor = aForegroundColor; }

		ci::ColorA getNameColor() const { return mNameColor; }
		void setNameColor( const ci::ColorA &aNameColor ) { mNameColor = aNameColor; renderNameTexture(); }	  

		std::string getName() const { return mName; }
		void setName( const std::string &aName ) { mName = aName; renderNameTexture(); }

		ci::gl::Texture getNameTexture() const { return mNameTexture; }
		void setNameTexture( const ci::gl::Texture &aTexture ) { mNameTexture = aTexture; }
		void drawBackground();
		void drawLabel();
		
		virtual void draw() = 0;
		virtual void update() = 0;
		
		virtual void press() { }
		virtual void release() { }
		virtual void handleMouseDown( const ci::Vec2i &aMousePos, const bool isRight ) { }
		virtual void handleMouseUp( const ci::Vec2i &aMousePos ) { }
		virtual void handleMouseDrag( const ci::Vec2i &aMousePos ) { }
		
		static int DEFAULT_HEIGHT;

	protected:
		ci::Vec2i mPosition;
		ci::Vec2i mSize;
		ci::Area mBounds;
		
	private:
		// disable copy and operator=
		UIElement(const UIElement&);
		UIElement & operator=(const UIElement&);
		
		void mouseDown( ci::app::MouseEvent &event );
		void mouseUp( ci::app::MouseEvent &event );
		void mouseDrag( ci::app::MouseEvent &event );
		
		ci::signals::scoped_connection mCbMouseDown, mCbMouseUp, mCbMouseDrag;

		UIController *mParent;
		ci::JsonTree mParams;
		std::string mName;
		std::string mGroup;
		ci::Font mFont;
		ci::TextBox::Alignment mAlignment;
		ci::gl::Texture mBackgroundTexture;
		ci::gl::Texture mNameTexture;
		ci::ColorA mBackgroundColor, mForegroundColor, mNameColor;
		bool mActive;
		bool mLocked;
		bool mIcon;
		bool mClear;

	};
	
}