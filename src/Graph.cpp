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

// MovingGraph
MovingGraph::MovingGraph(UIController *aUIController, const string &aName, float *aValueToLink, const string &aParamString, const std::function<void(bool)>& aEventHandler)
	: UIElement( aUIController, aName, aParamString )
{
	// initialize unique variables
	mLinkedValue = aValueToLink;
	addEventHandler(aEventHandler);
	mPressed = hasParam("pressed") ? getParam<bool>("pressed") : false;
	mStateless = hasParam("stateless") ? getParam<bool>("stateless") : true;
	mExclusive = hasParam("exclusive") ? getParam<bool>("exclusive") : false;
	mCallbackOnRelease = hasParam("callbackOnRelease") ? getParam<bool>("callbackOnRelease") : true;
	mContinuous = hasParam("continuous") ? getParam<bool>("continuous") : false;
	mMin = hasParam("min") ? getParam<float>("min") : 0.0f;
	mMax = hasParam( "max" ) ? getParam<float>( "max" ) : 1.0f;

	// set size
	int x = hasParam( "width" ) ? getParam<int>( "width" ) : MovingGraph::DEFAULT_WIDTH;
	int y = hasParam( "height" ) ? getParam<int>( "height" ) : MovingGraph::DEFAULT_HEIGHT;
	setSize( Vec2i( x, y ) );

	// set position and bounds
	setPositionAndBounds();
	mScreenMin = mBounds.getX1();
	mScreenMax = mBounds.getX2();

	mBufferSize = 128;
	mScale = mBounds.getHeight() * 0.5f;
	mInc = mBounds.getWidth() / ( (float)mBufferSize - 1.0f );

	renderNameTexture();
	// set screen value
	update();
}

UIElementRef MovingGraph::create(UIController *aUIController, const string &aName, float *aValueToLink, const string &aParamString, const std::function<void(bool)>& aEventHandler)
{
	return shared_ptr<MovingGraph>(new MovingGraph(aUIController, aName, aValueToLink, aParamString, aEventHandler));
}

void MovingGraph::draw()
{
	// draw the outer rect
	gl::color( UIController::DEFAULT_STROKE_COLOR );
	gl::drawStrokedRect( getBounds() );
	gl::drawSolidRect( getBounds() );

	// draw the graph
	gl::color( UIController::ACTIVE_STROKE_COLOR );
	gl::pushMatrices();
	gl::translate( mBounds.getX1(), mBounds.getY1() + mScale );

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
void MovingGraph::release()
{
	if (mPressed) {
		mPressed = false;
		if (mCallbackOnRelease) {
			callEventHandlers();
		}
	}
}

void MovingGraph::handleMouseUp(const Vec2i &aMousePos)
{
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
