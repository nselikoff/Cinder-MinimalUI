#pragma once

#include "UIElement.h"
#include "UIController.h"

namespace MinimalUI {
	
	class Label : public UIElement {
	public:
		Label( UIController *aUIController, const std::string &aName, const std::string &aParamString );
		static UIElementRef create( UIController *aUIController, const std::string &aName, const std::string &aParamString );
		void draw();
		void update() { }
		void setPositionAndBounds();
		
	private:
		bool mNarrow;
		
		static int DEFAULT_WIDTH;
	};
		
}