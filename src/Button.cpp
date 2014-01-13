#include "Button.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

int Button::DEFAULT_WIDTH = UIElement::DEFAULT_HEIGHT;
int Button::DEFAULT_HEIGHT = UIElement::DEFAULT_HEIGHT;

Button::Button( UIController *aUIController, const string &aName, const std::function<void( bool )>& aEventHandler, const string &aParamString )
: UIElement( aUIController, aName, aParamString )
{
	// initialize unique variables
	addEventHandler( aEventHandler );
	mPressed = hasParam( "pressed" ) ? getParam<bool>( "pressed" ) : false;
	mStateless = hasParam( "stateless" ) ? getParam<bool>( "stateless" ) : true;
	mExclusive = hasParam( "exclusive" ) ? getParam<bool>( "exclusive" ) : false;
	mCallbackOnRelease = hasParam( "callbackOnRelease" ) ? getParam<bool>( "callbackOnRelease" ) : true;
	mContinuous = hasParam( "continuous" ) ? getParam<bool>( "continuous" ) : false;
	
	// set size and render name texture
	int x = hasParam( "width" ) ? getParam<int>( "width" ) : Button::DEFAULT_WIDTH;
	int y = hasParam( "height" ) ? getParam<int>( "height" ) : Button::DEFAULT_HEIGHT;
	setSize( Vec2i( x, y) );
	renderNameTexture();

	// set position and bounds
	setPositionAndBounds();
}

UIElementRef Button::create( UIController *aUIController, const string &aName, const std::function<void( bool )>& aEventHandler, const string &aParamString )
{
	return shared_ptr<Button>( new Button( aUIController, aName, aEventHandler, aParamString ) );
}

void Button::draw()
{
	// set the color
	if ( isActive() ) {
		gl::color( UIController::ACTIVE_STROKE_COLOR );
	} else if ( mPressed ) {
		gl::color( UIController::DEFAULT_STROKE_COLOR );
	} else {
		gl::color( getBackgroundColor() );
	}

	// draw the button background
	gl::drawSolidRect( getBounds() );

	// set the color
	if ( isActive() ) {
		gl::color( UIController::ACTIVE_STROKE_COLOR );
	} else {
		gl::color( UIController::DEFAULT_STROKE_COLOR );
	}
	
	// draw the stroke
	gl::drawStrokedRect( getBounds() );

	// draw the label
	drawLabel();
}

void Button::release()
{	
	if ( mPressed ) {
		mPressed = false;
		if ( mCallbackOnRelease ) {
			callEventHandlers();
		}
	}
}

void Button::handleMouseUp( const Vec2i &aMousePos )
{
	if ( mStateless ) {
		// mPressed should always be false if it's a stateless button; just call the handler
		callEventHandlers();
		
	} else {
		if ( mPressed ) {
			// if the button is in an exclusive group and it's already pressed, don't do anything
			if ( ! mExclusive ) {
				// release the button
				mPressed = false;
				callEventHandlers();
			}
		} else {
			// if the button is an exclusive group and isn't pressed, release all the buttons in the group first
			if ( mExclusive ) {
				getParent()->releaseGroup( getGroup() );
			}
			// press the button
			mPressed = true;
			callEventHandlers();
		}
	}
}

void Button::addEventHandler( const std::function<void( bool )>& aEventHandler )
{
	mEventHandlers.push_back( aEventHandler );
}

void Button::callEventHandlers()
{
	for (int i = 0; i < mEventHandlers.size(); i++ ) {
		mEventHandlers[i]( mPressed );
	}
}

void Button::update()
{
	if ( mContinuous && mStateless && isActive() ) {
		callEventHandlers();
	}
}


LinkedButton::LinkedButton( UIController *aUIController, const string &aName, const std::function<void( bool )>& aEventHandler, bool *aLinkedState, const string &aParamString )
: Button( aUIController, aName, aEventHandler, aParamString )
{
	mLinkedState = aLinkedState;
}

UIElementRef LinkedButton::create( UIController *aUIController, const string &aName, const std::function<void( bool )>& aEventHandler, bool *aLinkedState, const string &aParamString )
{
	return shared_ptr<LinkedButton>( new LinkedButton( aUIController, aName, aEventHandler, aLinkedState, aParamString ) );
}

void LinkedButton::update()
{
	setPressed( *mLinkedState );
	Button::update();
}