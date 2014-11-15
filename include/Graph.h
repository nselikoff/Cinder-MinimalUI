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
		MovingGraph(UIControllerRef parent, const std::string &aName, float *aValueToLink, ci::JsonTree json );
		MovingGraph(UIControllerRef parent, const std::string &aName, float *aValueToLink, const std::function<void(bool)>& aEventHandler, ci::JsonTree json);

		static UIElementRef create(UIControllerRef parent, const std::string &aName, float *aValueToLink, ci::JsonTree json );
		static UIElementRef create(UIControllerRef parent, const std::string &aName, float *aValueToLink, const std::function<void(bool)>& aEventHandler, ci::JsonTree json );

		void init();
		void draw();
		void update();
		void press();
		void release();
		void handleMouseUp(const ci::ivec2 &aMousePos);
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