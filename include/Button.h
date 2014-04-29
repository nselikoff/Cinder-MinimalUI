#pragma once

#include "UIElement.h"
#include "UIController.h"

namespace MinimalUI {
	
	class Button : public UIElement {
	public:
		Button( UIController *aUIController, const std::string &aName, const std::function<void( bool )>& aEventHandler, const std::string &aParamString );
		static UIElementRef create( UIController *aUIController, const std::string &aName, const std::function<void( bool )>& aEventHandler, const std::string &aParamString );
		
		void draw();
		void update();
		void press();
		void release();
		void handleMouseUp( const ci::Vec2i &aMousePos );
		void addEventHandler( const std::function<void( bool )>& aEventHandler );
		void callEventHandlers();
		
		void setPressed( const bool &aPressed ) { mPressed = aPressed; }
		
	private:
		std::vector< std::function<void( bool )> > mEventHandlers;
		bool mPressed;
		bool mStateless;
		bool mExclusive;
		bool mCallbackOnRelease;
		bool mContinuous;
		
		static int DEFAULT_WIDTH;
		static int DEFAULT_HEIGHT;
	};
	
	class LinkedButton : public Button {
	public:
		LinkedButton( UIController *aUIController, const std::string &aName, const std::function<void( bool )>& aEventHandler, bool *aLinkedState, const std::string &aParamString );
		static UIElementRef create( UIController *aUIController, const std::string &aName, const std::function<void( bool )>& aEventHandler, bool *aLinkedState, const std::string &aParamString );
		
		void update();
	private:
		bool *mLinkedState;
	};

}