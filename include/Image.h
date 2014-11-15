#pragma once

#include "cinder/gl/gl.h"

#include "UIElement.h"
#include "UIController.h"

namespace MinimalUI {
	
	class Image : public UIElement {
	public:
		Image( UIControllerRef parent, const std::string &aName, ci::ImageSourceRef aImage, ci::JsonTree json );
		static UIElementRef create( UIControllerRef parent, const std::string &aName, ci::ImageSourceRef aImage, ci::JsonTree json );
		void draw();
		void update() { }
		void setPositionAndBounds();
		
	private:
		
	};
	
}