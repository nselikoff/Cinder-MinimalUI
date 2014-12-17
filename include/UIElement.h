#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Text.h"
#include "cinder/Json.h"
#include "UIController.h"

namespace MinimalUI {
	
	class UIController;

	class UIElement : public std::enable_shared_from_this<UIElement> {
	public:
		UIElement( UIControllerRef parent, const std::string &aName, ci::JsonTree json );
		virtual ~UIElement() { }
		
		void offsetInsertPosition();
		void setPositionAndBounds();
		
		void renderNameTexture();
		
		std::string getGroup() const { return mGroup; }
		
		void setLocked( const bool &locked ) { mLocked = locked; }
		
		UIControllerRef getParent() const { return mParent; }

		ci::ivec2 getPosition() const { return ci::app::toPixels( mPosition ); }
		void setPosition( const ci::ivec2 &aPosition ) { mPosition = aPosition; }
		
		ci::ivec2 getSize() const { return ci::app::toPixels( mSize ); }
		void setSize( const ci::ivec2 &aSize ) { mSize = aSize; }
		
		ci::Area getBounds() const { return ci::app::toPixels( mBounds ); }
		void setBounds( const ci::Area &aBounds ) { mBounds = aBounds; }
		
		bool hasParam( const std::string &aName ) const { return mParams.hasChild( aName ); }
		template <class T>
		T getParam( const std::string &aName ) const { return mParams[aName].getValue<T>(); }
		
		bool isActive() const { return mActive; }
		void setActive( const bool &aActive ) { mActive = aActive; }
		void deactivate() { mActive = false; }
		bool isLocked() const { return mLocked; }
		
		ci::gl::TextureRef getBackgroundTexture() const { return mBackgroundTexture; }
		void setBackgroundTexture( const ci::gl::TextureRef aBackgroundTexture ) { mBackgroundTexture = aBackgroundTexture; }
		
		ci::ColorA getBackgroundColor() const { return mBackgroundColor; }
		void setBackgroundColor( const ci::ColorA &aBackgroundColor ) { mBackgroundColor = aBackgroundColor; }
		
		ci::ColorA getForegroundColor() const { return mForegroundColor; }
		void setForegroundColor( const ci::ColorA &aForegroundColor ) { mForegroundColor = aForegroundColor; }

		ci::ColorA getNameColor() const { return mNameColor; }
		void setNameColor( const ci::ColorA &aNameColor ) { mNameColor = aNameColor; renderNameTexture(); }	  

		std::string getName() const { return mName; }
		void setName( const std::string &aName ) { mName = aName; renderNameTexture(); }

		ci::gl::TextureRef getNameTexture() const { return mNameTexture; }
		void setNameTexture( const ci::gl::TextureRef aTexture ) { mNameTexture = aTexture; }
		void drawBackground();
		void drawLabel();
		
		virtual void draw();
		virtual void drawImpl() = 0;
		virtual void update() = 0;
		
		virtual void press() { }
		virtual void release() { }
		virtual void handleMouseDown( const ci::ivec2 &aMousePos, const bool isRight ) { }
		virtual void handleMouseUp( const ci::ivec2 &aMousePos ) { }
		virtual void handleMouseDrag( const ci::ivec2 &aMousePos ) { }
		
		static int DEFAULT_HEIGHT;
		
	protected:
		ci::ivec2 mPosition;
		ci::ivec2 mSize;
		ci::Area mBounds;
		bool mFirstDraw;
		
	private:
		// disable copy and operator=
		UIElement(const UIElement&);
		UIElement & operator=(const UIElement&);
		
		void mouseDown( ci::app::MouseEvent &event );
		void mouseUp( ci::app::MouseEvent &event );
		void mouseDrag( ci::app::MouseEvent &event );
		
		ci::signals::scoped_connection mCbMouseDown, mCbMouseUp, mCbMouseDrag;

		UIControllerRef mParent;
		ci::JsonTree mParams;
		std::string mName;
		std::string mGroup;
		ci::Font mFont;
		ci::TextBox::Alignment mAlignment;
		ci::gl::TextureRef mBackgroundTexture;
		ci::gl::TextureRef mNameTexture;
		ci::ColorA mBackgroundColor, mForegroundColor, mNameColor;
		bool mActive;
		bool mLocked;
		bool mIcon;
		bool mClear;

	};
	
}