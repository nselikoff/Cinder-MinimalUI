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
		
		void handleMouseDown( const ci::ivec2 &aMousePos, const bool isRight );
		void handleMouseDrag( const ci::ivec2 &aMousePos );
		void updatePosition( const int &aPos );
		
	protected:
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
		Slider2D( UIController *aUIController, const std::string &aName, ci::vec2 *aValueToLink, const std::string &aParamString );
		static UIElementRef create( UIController *aUIController, const std::string &aName, ci::vec2 *aValueToLink, const std::string &aParamString );
		
		void draw();
		void update();
		
		void handleMouseDown( const ci::ivec2 &aMousePos, const bool isRight );
		void handleMouseDrag( const ci::ivec2 &aMousePos );
		void updatePosition( const ci::ivec2 &aPos );
		
	private:
		ci::vec2 mMin;
		ci::vec2 mMax;
		ci::ivec2 mScreenMin;
		ci::ivec2 mScreenMax;
		ci::vec2 mValue;
		ci::vec2 *mLinkedValue;
		ci::vec2 mDefaultValue;
 
		static int DEFAULT_HEIGHT;
		static int DEFAULT_WIDTH;
		static int DEFAULT_HANDLE_HALFWIDTH;
	};

	class SliderCallback : public Slider {
	public:
		SliderCallback( UIController *aUIController, const std::string &aName, float *aValueToLink, const std::function<void()>& aEventHandler, const std::string &aParamString );
		static UIElementRef create( UIController *aUIController, const std::string &aName, float *aValueToLink, const std::function<void()>& aEventHandler, const std::string &aParamString );
		
		void handleMouseDown( const ci::ivec2 &aMousePos, const bool isRight );
		
		void addEventHandler( const std::function<void()>& aEventHandler );
		void callEventHandlers();
		
	private:
		std::vector< std::function<void()> > mEventHandlers;
	};
}