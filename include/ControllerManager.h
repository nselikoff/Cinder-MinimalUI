#pragma once

#include "Resources.h"
#include "cinder/app/AppNative.h"
#include "cinder/DataSource.h"
#include "cinder/Json.h"
#include <vector>

#include "UIController.h"
#include "UIElement.h"
#include "Slider.h"
#include "Button.h"
#include "Label.h"
#include "Image.h"
#include "Graph.h"

namespace MinimalUI {

	class ControllerManager {

	public:
		ControllerManager();
		static UIControllerRef create( ci::DataSourceRef dataSource, ci::app::WindowRef aWindow = ci::app::App::get()->getWindow() );

	private:
	
	};
	
}