#pragma once

#include "UIElement.h"
#include "UIController.h"

namespace MinimalUI {

	class Slider : public UIElement {
	public:
		Slider( UIController *aUIController, const std::string &aName, float *aValueToLink, const std::string &aParamString );
		static UIElementRef create( UIController *aUIController, const std::string &aName, float *aValueToLink, const std::string &aParamString );
		
		void draw();
		void update();
		
		void handleMouseDown( const ci::Vec2i &aMousePos, const bool isRight );
		void handleMouseDrag( const ci::Vec2i &aMousePos );
		void updatePosition( const int &aPos );
		
	private:
		float mMin;
		float mMax;
		int mScreenMin;
		int mScreenMax;
		float mValue;
		float *mLinkedValue;
		float mDefaultValue;
		bool mHandleVisible;
		bool mVertical;

		static int DEFAULT_HEIGHT;
		static int DEFAULT_WIDTH;
		static int DEFAULT_HANDLE_HALFWIDTH;
	};  
   
	class Slider2D : public UIElement {
	public:
		Slider2D( UIController *aUIController, const std::string &aName, ci::Vec2f *aValueToLink, const std::string &aParamString );
		static UIElementRef create( UIController *aUIController, const std::string &aName, ci::Vec2f *aValueToLink, const std::string &aParamString );
		
		void draw();
		void update();
		
		void handleMouseDown( const ci::Vec2i &aMousePos, const bool isRight );
		void handleMouseDrag( const ci::Vec2i &aMousePos );
		void updatePosition( const ci::Vec2i &aPos );
		
	private:
		ci::Vec2f mMin;
		ci::Vec2f mMax;
		ci::Vec2i mScreenMin;
		ci::Vec2i mScreenMax;
		ci::Vec2f mValue;
		ci::Vec2f *mLinkedValue;
		ci::Vec2f mDefaultValue;
 
		static int DEFAULT_HEIGHT;
		static int DEFAULT_WIDTH;
		static int DEFAULT_HANDLE_HALFWIDTH;
	};

}