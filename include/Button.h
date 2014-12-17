#pragma once

#include "UIElement.h"
#include "UIController.h"

namespace MinimalUI {
	
	class Button : public UIElement {
	public:
		Button( UIControllerRef parent, const std::string &aName, ci::JsonTree json );
		static UIElementRef create( UIControllerRef parent, const std::string &aName, ci::JsonTree json );

		Button( UIControllerRef parent, const std::string &aName, const std::function<void( bool )>& aEventHandler, ci::JsonTree json );
		static UIElementRef create( UIControllerRef parent, const std::string &aName, const std::function<void( bool )>& aEventHandler, ci::JsonTree json );
		
		void init();
		
		void drawImpl();
		void update();
		void press();
		void release();
		void handleMouseUp( const ci::ivec2 &aMousePos );
		void addEventHandler( const std::function<void( bool )>& aEventHandler );
		void callEventHandlers();
		
		void setPressed( const bool &aPressed ) { mPressed = aPressed; }
		bool isPressed() const { return mPressed; }
		
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
		LinkedButton( UIControllerRef parent, const std::string &aName, const std::function<void( bool )>& aEventHandler, bool *aLinkedState, ci::JsonTree json );
		static UIElementRef create( UIControllerRef parent, const std::string &aName, const std::function<void( bool )>& aEventHandler, bool *aLinkedState, ci::JsonTree json );
		
		void update();
	private:
		bool *mLinkedState;
	};

}