#pragma once

#include "cinder/gl/gl.h"

#include "UIElement.h"
#include "UIController.h"

namespace MinimalUI {
	
	class Image : public UIElement {
	public:
		Image( UIController *aUIController, const std::string &aName, ci::ImageSourceRef aImage, const std::string &aParamString );
		static UIElementRef create( UIController *aUIController, const std::string &aName, ci::ImageSourceRef aImage, const std::string &aParamString );
		void draw();
		void update() { }
		void setPositionAndBounds();
		
	private:
		
	};
	
}