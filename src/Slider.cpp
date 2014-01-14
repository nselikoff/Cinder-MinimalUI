#include "Slider.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

int Slider::DEFAULT_HEIGHT = UIElement::DEFAULT_HEIGHT;
int Slider::DEFAULT_WIDTH = UIController::DEFAULT_PANEL_WIDTH - UIController::DEFAULT_MARGIN_LARGE * 2;
int Slider::DEFAULT_HANDLE_HALFWIDTH = 8;

int Slider2D::DEFAULT_HEIGHT = 96;
int Slider2D::DEFAULT_WIDTH = 96;
int Slider2D::DEFAULT_HANDLE_HALFWIDTH = 4;

Slider::Slider( UIController *aUIController, const string &aName, float *aValueToLink, const string &aParamString )
	: UIElement( aUIController, aName, aParamString )
{
	// initialize unique variables
	mLinkedValue = aValueToLink;
	mDefaultValue = *aValueToLink;
	mMin = hasParam( "min" ) ? getParam<float>( "min" ) : 0.0f;
	mMax = hasParam( "max" ) ? getParam<float>( "max" ) : 1.0f;

	// set colors
	std::stringstream str;
	string strValue;
	uint32_t hexValue;
	strValue = hasParam( "foregroundColor" ) ? getParam<string>( "foregroundColor" ) : "0xFF12424A"; // should be same as DEFAULT_STROKE_COLOR
	str << strValue;
	str >> std::hex >> hexValue;
	setForegroundColor( ColorA::hexA( hexValue ) );

	strValue = hasParam( "backgroundColor" ) ? getParam<string>( "backgroundColor" ) : "0xFF000000"; // should be same as DEFAULT_BACKGROUND_COLOR
	str.clear();
	str << strValue;
	str >> std::hex >> hexValue;
	setBackgroundColor( ColorA::hexA( hexValue ) );

	mHandleVisible  = hasParam( "handleVisible" ) ? getParam<bool>( "handleVisible" ) : true; 
	mVertical  = hasParam( "vertical" ) ? getParam<bool>( "vertical" ) : false; 
	// set size and render name texture
	int x = hasParam( "width" ) ? getParam<int>( "width" ) : Slider::DEFAULT_WIDTH;
	int y = Slider::DEFAULT_HEIGHT;
	setSize( Vec2i( x, y ) );
	renderNameTexture();

	// set position and bounds
	setPositionAndBounds();

	// set screen min and max
	if ( mVertical )
	{
		mScreenMin = getPosition().y + Slider::DEFAULT_HANDLE_HALFWIDTH;
		mScreenMax = getBounds().getY2() - Slider::DEFAULT_HANDLE_HALFWIDTH;
	}
	else
	{
		mScreenMin = getPosition().x + Slider::DEFAULT_HANDLE_HALFWIDTH;
		mScreenMax = getBounds().getX2() - Slider::DEFAULT_HANDLE_HALFWIDTH;
	}

	// set screen value
	update();
}

UIElementRef Slider::create( UIController *aUIController, const string &aName, float *aValueToLink, const string &aParamString )
{
	return shared_ptr<Slider>( new Slider( aUIController, aName, aValueToLink, aParamString ) );
}

void Slider::draw()
{
	// draw the solid rect
	if ( isLocked() ) {
		gl::color( UIController::DEFAULT_STROKE_COLOR );
	} else {
		gl::color( getBackgroundColor() );//Color::black() );
	}
	gl::drawSolidRect( getBounds() );

	// draw the outer rect
	if ( isActive() ) {
		gl::color( UIController::ACTIVE_STROKE_COLOR );
	} else {
		gl::color( UIController::DEFAULT_STROKE_COLOR );
	}
	gl::drawStrokedRect( getBounds() );

	// draw the handle
	if ( mHandleVisible )
	{
		if ( isLocked() ) {
			gl::color( Color::black() );
		} else if ( isActive() ) {
			gl::color( UIController::ACTIVE_STROKE_COLOR );
		} else {
			gl::color( UIController::DEFAULT_STROKE_COLOR );
		}
		Vec2f handleStart, handleEnd;
		if ( mVertical )
		{
			handleStart = Vec2f( getBounds().getX1(), mValue - Slider::DEFAULT_HANDLE_HALFWIDTH );
			handleEnd = Vec2f( getBounds().getX2(), mValue + Slider::DEFAULT_HANDLE_HALFWIDTH );
		}
		else
		{
			handleStart = Vec2f( mValue - Slider::DEFAULT_HANDLE_HALFWIDTH, getBounds().getY1() );
			handleEnd = Vec2f( mValue + Slider::DEFAULT_HANDLE_HALFWIDTH, getBounds().getY2() );
		}
		gl::drawStrokedRect( Rectf( handleStart, handleEnd ) );
		gl::drawSolidRect( Rectf( handleStart, handleEnd ) );
	}
	// draw the label
	drawLabel();
}

void Slider::update()
{
	if ( mVertical )
	{
		mValue = lmap<float>(*mLinkedValue, mMin, mMax, getBounds().getY2() - Slider::DEFAULT_HANDLE_HALFWIDTH, getPosition().y + Slider::DEFAULT_HANDLE_HALFWIDTH );
	}
	else
	{
		mValue = lmap<float>(*mLinkedValue, mMin, mMax, getPosition().x + Slider::DEFAULT_HANDLE_HALFWIDTH, getBounds().getX2() - Slider::DEFAULT_HANDLE_HALFWIDTH );
	}
}

void Slider::handleMouseDown( const Vec2i &aMousePos, const bool isRight )
{
	if ( isRight )
	{
		*mLinkedValue = mDefaultValue;		
	}
	else 
	{
		if ( mVertical )
		{
			updatePosition( aMousePos.y );
		}
		else
		{
			updatePosition( aMousePos.x );
		}	
	}
}

void Slider::handleMouseDrag( const Vec2i &aMousePos )
{
	if ( mVertical )
	{
		updatePosition( math<int>::clamp( aMousePos.y, mScreenMin, mScreenMax ) );
	}
	else
	{
		updatePosition( math<int>::clamp( aMousePos.x, mScreenMin, mScreenMax ) );
	}	
}

void Slider::updatePosition( const int &aPos )
{
	mValue = aPos;
	if ( mVertical )
	{
		*mLinkedValue = lmap<float>(mValue, mScreenMax, mScreenMin, mMin, mMax );
	}
	else
	{
		*mLinkedValue = lmap<float>(mValue, mScreenMin, mScreenMax, mMin, mMax );
	}		
}

// Slider2D
Slider2D::Slider2D( UIController *aUIController, const string &aName, Vec2f *aValueToLink, const string &aParamString )
	: UIElement( aUIController, aName, aParamString )
{
	// initialize unique variables
	mLinkedValue = aValueToLink;
	mDefaultValue = Vec2f( (*mLinkedValue).x, (*mLinkedValue).y );
	float minX = hasParam( "minX" ) ? getParam<float>( "minX" ) : 0.0f;
	float maxX = hasParam( "maxX" ) ? getParam<float>( "maxX" ) : 1.0f;
	float minY = hasParam( "minY" ) ? getParam<float>( "minY" ) : 0.0f;
	float maxY = hasParam( "maxY" ) ? getParam<float>( "maxY" ) : 1.0f;
	mMin = Vec2f( minX, minY );
	mMax = Vec2f( maxX, maxY );

	// set size and render name texture
	int x = hasParam( "width" ) ? getParam<int>( "width" ) : Slider2D::DEFAULT_WIDTH;
	int y = Slider2D::DEFAULT_HEIGHT;
	setSize( Vec2i( x, y ) );
	renderNameTexture();

	// set position and bounds
	setPositionAndBounds();

	// set screen min and max
	Vec2i offset = Vec2i( Slider2D::DEFAULT_HANDLE_HALFWIDTH, Slider2D::DEFAULT_HANDLE_HALFWIDTH );
	mScreenMin = getPosition() + offset;
	mScreenMax = getBounds().getLR() - offset;

	// set screen value
	update();
}

UIElementRef Slider2D::create( UIController *aUIController, const string &aName, Vec2f *aValueToLink, const string &aParamString )
{
	return shared_ptr<Slider2D>( new Slider2D( aUIController, aName, aValueToLink, aParamString ) );
}

void Slider2D::draw()
{
	// draw the outer rect
	gl::color( UIController::DEFAULT_STROKE_COLOR );
	gl::drawStrokedRect( getBounds() );
	gl::drawSolidRect( getBounds() );

	// draw the indicator lines
	gl::color( UIController::ACTIVE_STROKE_COLOR );
	gl::drawLine( Vec2f( getBounds().getX1(), mValue.y ), Vec2f( getBounds().getX2(), mValue.y ) );
	gl::drawLine( Vec2f( mValue.x, getBounds().getY1() ), Vec2f( mValue.x, getBounds().getY2() ) );

	// draw the handle
	Vec2f offset = Vec2i( Slider2D::DEFAULT_HANDLE_HALFWIDTH, Slider2D::DEFAULT_HANDLE_HALFWIDTH );
	Vec2f handleStart = mValue - offset;
	Vec2f handleEnd = mValue + offset;
	gl::drawStrokedRect( Rectf( handleStart, handleEnd ) );
	gl::drawSolidRect( Rectf( handleStart, handleEnd ) );
}

void Slider2D::update()
{
	Vec2i offset = Vec2i( Slider2D::DEFAULT_HANDLE_HALFWIDTH, Slider2D::DEFAULT_HANDLE_HALFWIDTH );
	mValue.x = lmap<float>((*mLinkedValue).x, mMin.x, mMax.x, getPosition().x + offset.x, getBounds().getX2() - offset.x );
	mValue.y = lmap<float>((*mLinkedValue).y, mMin.y, mMax.y, getBounds().getY2() - offset.y, getPosition().y + offset.y );
}

void Slider2D::handleMouseDown( const Vec2i &aMousePos, const bool isRight )
{
	if ( isRight )
	{
		(*mLinkedValue).x = mDefaultValue.x;
		(*mLinkedValue).y = mDefaultValue.y;		
	}
	else updatePosition( aMousePos );
}

void Slider2D::handleMouseDrag( const Vec2i &aMousePos )
{
	Vec2i newPos;
	newPos.x = math<int>::clamp( aMousePos.x, mScreenMin.x, mScreenMax.x );
	newPos.y = math<int>::clamp( aMousePos.y, mScreenMin.y, mScreenMax.y );
	updatePosition( newPos );
}

void Slider2D::updatePosition( const Vec2i &aPos )
{
	mValue = aPos;
	(*mLinkedValue).x = lmap<float>(mValue.x, mScreenMin.x, mScreenMax.x, mMin.x, mMax.x );
	(*mLinkedValue).y = lmap<float>(mValue.y, mScreenMin.y, mScreenMax.y, mMax.y, mMin.y );
}

// SliderCallback
SliderCallback::SliderCallback( UIController *aUIController, const string &aName, float *aValueToLink, const std::function<void()>& aEventHandler, const string &aParamString )
: Slider( aUIController, aName, aValueToLink, aParamString )
{
    // initialize unique variables
    addEventHandler( aEventHandler );
}

UIElementRef SliderCallback::create( UIController *aUIController, const string &aName, float *aValueToLink, const std::function<void()>& aEventHandler, const string &aParamString )
{
    return shared_ptr<SliderCallback>( new SliderCallback( aUIController, aName, aValueToLink, aEventHandler, aParamString ) );
}

void SliderCallback::addEventHandler( const std::function<void()>& aEventHandler )
{
    mEventHandlers.push_back( aEventHandler );
}

void SliderCallback::callEventHandlers()
{
    for (int i = 0; i < mEventHandlers.size(); i++ ) {
        mEventHandlers[i]();
    }
}

void SliderCallback::handleMouseDown( const Vec2i &aMousePos )
{
    callEventHandlers();
    updatePosition( aMousePos.x );
}
