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
		MovingGraph(UIController *aUIController, const std::string &aName, float *aValueToLink, const std::string &aParamString);
		MovingGraph(UIController *aUIController, const std::string &aName, float *aValueToLink, const std::function<void(bool)>& aEventHandler, const std::string &aParamString);

		static UIElementRef create(UIController *aUIController, const std::string &aName, float *aValueToLink, const std::string &aParamString);
		static UIElementRef create(UIController *aUIController, const std::string &aName, float *aValueToLink, const std::function<void(bool)>& aEventHandler, const std::string &aParamString);

		void init();
		void draw();
		void update();
		void press();
		void release();
		void handleMouseUp(const ci::Vec2i &aMousePos);
		void addEventHandler(const std::function<void(bool)>& aEventHandler);
		void callEventHandlers();

		void setPressed(const bool &aPressed) { mPressed = aPressed; }

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
	private:
		std::vector< std::function<void(bool)> > mEventHandlers;
		bool mPressed;
		bool mStateless;
		bool mExclusive;
		bool mCallbackOnRelease;
		bool mContinuous;
	};

}