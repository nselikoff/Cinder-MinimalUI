/*
 Adapted from ciUI's Moving Graph by @brucelane with permission from @rezaali
 https://github.com/rezaali/ciUI
*/

#pragma once

#include "UIElement.h"
#include "UIController.h"

namespace MinimalUI {

	class MovingGraph : public UIElement {
	public:
		MovingGraph( UIController *aUIController, const std::string &aName, float *aValueToLink, const std::string &aParamString );
		static UIElementRef create( UIController *aUIController, const std::string &aName, float *aValueToLink, const std::string &aParamString );

		void draw();
		void update();

	protected:
		float mMin;
		float mMax;
		int mScreenMin;
		int mScreenMax;
		float *mLinkedValue;
		std::vector<float> mBuffer;
		int mBufferSize; 
        ci::Shape2d mShape;
		float mScale, mInc; 

		static int DEFAULT_HEIGHT;
		static int DEFAULT_WIDTH;
	};

}