/*
 Adapted from ciUI's Moving Graph by @brucelane with permission from @rezaali
 https://github.com/rezaali/ciUI
*/

#include "Graph.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

int MovingGraph::DEFAULT_HEIGHT = UIElement::DEFAULT_HEIGHT;
int MovingGraph::DEFAULT_WIDTH = 96;

// common initialization
void MovingGraph::init()
{
	// initialize unique variables
	mMin = hasParam("min") ? getParam<float>("min") : 0.0f;
	mMax = hasParam("max") ? getParam<float>("max") : 1.0f;
	mPressed = hasParam("pressed") ? getParam<bool>("pressed") : false;
	mStateless = hasParam("stateless") ? getParam<bool>("stateless") : true;
	mExclusive = hasParam("exclusive") ? getParam<bool>("exclusive") : false;
	mCallbackOnRelease = hasParam("callbackOnRelease") ? getParam<bool>("callbackOnRelease") : true;
	mContinuous = hasParam("continuous") ? getParam<bool>("continuous") : false;

	// set size
	int x = hasParam("width") ? getParam<int>("width") : MovingGraph::DEFAULT_WIDTH;
	int y = hasParam("height") ? getParam<int>("height") : MovingGraph::DEFAULT_HEIGHT;
	setSize(ivec2(x, y));
	
	// set position and bounds
	setPositionAndBounds();
	mScreenMin = mBounds.getX1();
	mScreenMax = mBounds.getX2();
	
	mBufferSize = 128;
	mScale = mBounds.getHeight() * 0.5f;
	mInc = mBounds.getWidth() / ((float)mBufferSize - 1.0f);
	
	renderNameTexture();
	// set screen value
	update();
}

// without event handler
MovingGraph::MovingGraph(UIControllerRef parent, const string &aName, float *aValueToLink, JsonTree json)
: UIElement(parent, aName, json), mLinkedValue(aValueToLink)
{
	init();
}

// with event handler
MovingGraph::MovingGraph(UIControllerRef parent, const string &aName, float *aValueToLink, const std::function<void(bool)>& aEventHandler, JsonTree json)
	: UIElement(parent, aName, json), mLinkedValue(aValueToLink)
{
	// initialize unique variables
	addEventHandler(aEventHandler);

	init();
}

// without event handler
UIElementRef MovingGraph::create(UIControllerRef parent, const string &aName, float *aValueToLink, JsonTree json)
{
	return shared_ptr<MovingGraph>(new MovingGraph(parent, aName, aValueToLink, json));
}

// with event handler
UIElementRef MovingGraph::create(UIControllerRef parent, const string &aName, float *aValueToLink, const std::function<void(bool)>& aEventHandler, JsonTree json)
{
	return shared_ptr<MovingGraph>(new MovingGraph(parent, aName, aValueToLink, aEventHandler, json));
}

void MovingGraph::draw()
{
	// set the color
	if ( isActive() && mEventHandlers.size() > 0 ) {
		gl::color(UIController::ACTIVE_STROKE_COLOR);
	}
	else if (mPressed) {
		gl::color(UIController::DEFAULT_STROKE_COLOR);
	}
	else {
		gl::color(getBackgroundColor());
	}
	// draw the button background
	gl::drawSolidRect(getBounds());

	// draw the background
	drawBackground();

	// draw the graph
	if ( isActive() && mEventHandlers.size() > 0 ) {
		gl::color(UIController::ACTIVE_STROKE_COLOR);
	}
	else {
		gl::color(UIController::DEFAULT_STROKE_COLOR);
	}
	// draw the outer rect
	gl::drawStrokedRect(getBounds());

	gl::pushMatrices();
	gl::translate( mBounds.getX1(), mBounds.getY1() + mScale );

	// active color for moving graph
	gl::color(UIController::ACTIVE_STROKE_COLOR);
	mShape.clear();
	mShape.moveTo( 0.0f, lmap<float>( mBuffer[0], mMin, mMax, mScale, -mScale ) );
	for (int i = 1; i < mBuffer.size(); i++)
	{			
		mShape.lineTo( mInc * (float)i, lmap<float>( mBuffer[i], mMin, mMax, mScale, -mScale ) );
	}
	gl::draw( mShape );
	gl::popMatrices();
	// draw the label
	drawLabel();
}

void MovingGraph::press()
{
	if ( !mPressed ) {
		mPressed = true;
		callEventHandlers();
	}
}

void MovingGraph::release()
{
	if (mPressed) {
		mPressed = false;
		if (mCallbackOnRelease) {
			callEventHandlers();
		}
	}
}

void MovingGraph::handleMouseUp(const ivec2 &aMousePos)
{
	if ( mEventHandlers.size() == 0 )
		return;
	
	if (mStateless) {
		// mPressed should always be false if it's a stateless button; just call the handler
		callEventHandlers();

	}
	else {
		if (mPressed) {
			// if the button is in an exclusive group and it's already pressed, don't do anything
			if (!mExclusive) {
				// release the button
				mPressed = false;
				callEventHandlers();
			}
		}
		else {
			// if the button is an exclusive group and isn't pressed, release all the buttons in the group first
			if (mExclusive) {
				getParent()->releaseGroup(getGroup());
			}
			// press the button
			mPressed = true;
			callEventHandlers();
		}
	}
}

void MovingGraph::addEventHandler(const std::function<void(bool)>& aEventHandler)
{
	mEventHandlers.push_back(aEventHandler);
}

void MovingGraph::callEventHandlers()
{
	for (int i = 0; i < mEventHandlers.size(); i++) {
		mEventHandlers[i](mPressed);
	}
	getParent()->setDirty();

}

void MovingGraph::update()
{
	if (mContinuous && mStateless && isActive()) {
		callEventHandlers();
	}	
	mBuffer.push_back( *mLinkedValue );

	if( mBuffer.size() >= mBufferSize )
	{
		mBuffer.erase( mBuffer.begin(), mBuffer.begin() + 1 );
	}
}
