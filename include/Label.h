#pragma once

#include "UIElement.h"
#include "UIController.h"

namespace MinimalUI {
	
	class Label : public UIElement {
	public:
		Label( UIControllerRef parent, const std::string &aName, ci::JsonTree json );
		static UIElementRef create( UIControllerRef parent, const std::string &aName, ci::JsonTree json );
		void draw();
		void update() { }
		void setPositionAndBounds();
		
	private:
		bool mNarrow;
		
		static int DEFAULT_WIDTH;
	};
		
}