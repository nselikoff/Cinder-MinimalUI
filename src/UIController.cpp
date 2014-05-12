#include "UIController.h"
#include "UIElement.h"
#include "Slider.h"
#include "Button.h"
#include "Label.h"
#include "Image.h"
#include "Graph.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

class FontStyleExc;

int UIController::DEFAULT_PANEL_WIDTH = 216;
int UIController::DEFAULT_MARGIN_LARGE = 10;
int UIController::DEFAULT_MARGIN_SMALL = 4;
int UIController::DEFAULT_UPDATE_FREQUENCY = 2;
int UIController::DEFAULT_FBO_WIDTH = 2048;
ci::ColorA UIController::DEFAULT_STROKE_COLOR = ci::ColorA( 0.07f, 0.26f, 0.29f, 1.0f );
ci::ColorA UIController::ACTIVE_STROKE_COLOR = ci::ColorA( 0.19f, 0.66f, 0.71f, 1.0f );
ci::ColorA UIController::DEFAULT_NAME_COLOR = ci::ColorA( 0.14f, 0.49f, 0.54f, 1.0f );
ci::ColorA UIController::DEFAULT_BACKGROUND_COLOR = ci::ColorA( 0.0f, 0.0f, 0.0f, 1.0f );

UIController::UIController( app::WindowRef aWindow, const string &aParamString )
	: mWindow( aWindow ), mParamString( aParamString )
{
	JsonTree params( mParamString );
	mVisible = params.hasChild( "visible" ) ? params["visible"].getValue<bool>() : true;
	mAlpha = mVisible ? 1.0f : 0.0f;
	mWidth = params.hasChild( "width" ) ? params["width"].getValue<int>() : DEFAULT_PANEL_WIDTH;
	mX = params.hasChild( "x" ) ? params["x"].getValue<int>() : 0;
	mY = params.hasChild( "y" ) ? params["y"].getValue<int>() : 0;
	if ( params.hasChild( "height" ) ) {
		mHeightSpecified = true;
		mHeight = params["height"].getValue<int>();
	} else {
		mHeightSpecified = false;
		mHeight = getWindow()->getHeight();
	}
	mCentered = params.hasChild( "centered" ) ? params["centered"].getValue<bool>() : false;
	mDepth = params.hasChild( "depth" ) ? params["depth"].getValue<int>() : 0;
	mForceInteraction = params.hasChild( "forceInteraction" ) ? params["forceInteraction"].getValue<bool>() : false;
	mMarginLarge = params.hasChild( "marginLarge" ) ? params["marginLarge"].getValue<int>() : DEFAULT_MARGIN_LARGE;

	// JSON doesn't support hex literals...
	std::stringstream str;
	string panelColor = params.hasChild( "panelColor" ) ? params["panelColor"].getValue<string>() : "0xCC000000";
	str << panelColor;
	uint32_t hexValue;
	str >> std::hex >> hexValue;
	mPanelColor = ColorA::hexA( hexValue );

	if ( params.hasChild( "defaultStrokeColor" ) )
	{
		string strValue = params["defaultStrokeColor"].getValue<string>();
		str.clear();
		str << strValue;
		str >> std::hex >> hexValue;
		UIController::DEFAULT_STROKE_COLOR = ColorA::hexA( hexValue );
	}
	if ( params.hasChild( "activeStrokeColor" ) )
	{
		string strValue = params["activeStrokeColor"].getValue<string>();
		str.clear();
		str << strValue;
		str >> std::hex >> hexValue;
		UIController::ACTIVE_STROKE_COLOR = ColorA::hexA( hexValue );
	}
	if ( params.hasChild( "defaultNameColor" ) )
	{
		string strValue = params["defaultNameColor"].getValue<string>();
		str.clear();
		str << strValue;
		str >> std::hex >> hexValue;
		UIController::DEFAULT_NAME_COLOR = ColorA::hexA( hexValue );
	}
	if ( params.hasChild( "defaultBackgroundColor" ) )
	{
		string strValue = params["defaultBackgroundColor"].getValue<string>();
		str.clear();
		str << strValue;
		str >> std::hex >> hexValue;
		UIController::DEFAULT_BACKGROUND_COLOR = ColorA::hexA( hexValue );
	}

	resize();

	mCbMouseDown = mWindow->getSignalMouseDown().connect( mDepth + 99, std::bind( &UIController::mouseDown, this, std::placeholders::_1 ) );

	// set default fonts
	setFont( "label", Font( "Arial", 16 * 2 ) );
	setFont( "smallLabel", Font( "Arial", 12 * 2 ) );
	setFont( "icon", Font( "Arial", 22 * 2 ) );
	setFont( "header", Font( "Arial", 48 * 2 ) );
	setFont( "body", Font( "Arial", 19 * 2 ) );
	setFont( "footer", Font( "Arial Italic", 14 * 2 ) );

	mInsertPosition = Vec2i( mMarginLarge, mMarginLarge );

	mFboNumSamples = params.hasChild( "fboNumSamples" ) ? params["fboNumSamples"].getValue<int>() : 0;
	if (params.hasChild("backgroundImage")) {
		mBackgroundTexture = gl::Texture(loadImage(loadAsset(params["backgroundImage"].getValue<string>())));
	}
	setupFbo();
}

UIControllerRef UIController::create( const string &aParamString, app::WindowRef aWindow )
{
	return shared_ptr<UIController>( new UIController( aWindow, aParamString ) );
}

void UIController::resize()
{
	Vec2i size;
	if ( mCentered ) {
		size = Vec2i( mWidth, mHeight );
		mPosition = getWindow()->getCenter() - size / 2;
	} else if ( mHeightSpecified ) {
		size = Vec2i( mWidth, mHeight );
		mPosition = Vec2i( mX, mY );
	} else {
		size = Vec2i( mWidth, getWindow()->getHeight() );
		mPosition = Vec2i( mX, mY );
	}
	mBounds = Area( Vec2i::zero(), size );
}

void UIController::mouseDown( MouseEvent &event )
{
	if ( mVisible ) {
		if ( (mBounds + mPosition).contains( event.getPos() ) || mForceInteraction )
		{
			event.setHandled();
		}
	}
}

void UIController::drawBackground()
{
	gl::pushMatrices();
	gl::color(Color::white());

	// draw the background texture if it's defined
	if (mBackgroundTexture) gl::draw(mBackgroundTexture, mBounds);

	gl::popMatrices();
}

void UIController::draw()
{
	if ( !mVisible )
		return;
	
	// save state
	gl::pushMatrices();
	glPushAttrib( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_LINE_BIT | GL_CURRENT_BIT );

	// disable depth read (otherwise any 3d drawing done after this will be obscured by the FBO; not exactly sure why)
	gl::disableDepthRead();

	// start drawing to the Fbo
	mFbo.bindFramebuffer();

	gl::lineWidth( toPixels( 2.0f ) );
	gl::enable( GL_LINE_SMOOTH );
	gl::enableAlphaBlending();
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	// clear and set viewport and matrices
	gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ) );
	gl::setViewport( toPixels( mBounds + mPosition ) );
	gl::setMatricesWindow( toPixels( mBounds.getSize() ), false);

	// draw backing panel
	gl::color( mPanelColor );
	gl::drawSolidRect( toPixels( mBounds ) );

	// draw the background
	drawBackground();

	// draw elements
	for (unsigned int i = 0; i < mUIElements.size(); i++) {
		mUIElements[i]->draw();
	}

	// finish drawing to the Fbo
	mFbo.unbindFramebuffer();

	// reset the matrices and blending
	gl::setViewport( toPixels( getWindow()->getBounds() ) );
	gl::setMatricesWindow( toPixels( getWindow()->getSize() ) );
	gl::enableAlphaBlending( true );

	// if forcing interaction, draw an overlay over the whole window
	if ( mForceInteraction ) {
		gl::color(ColorA( 0.0f, 0.0f, 0.0f, 0.5f * mAlpha));
		gl::drawSolidRect( toPixels( getWindow()->getBounds() ) );
	}

	// draw the FBO to the screen
	gl::color( ColorA( mAlpha, mAlpha, mAlpha, mAlpha ) );
	gl::draw( mFbo.getTexture() );
	gl::disableAlphaBlending();
	
	// restore state
	glPopAttrib();
	gl::popMatrices();
}

void UIController::update()
{
	if ( !mVisible )
		return;

	if ( getElapsedFrames() % DEFAULT_UPDATE_FREQUENCY == 0 ) {
		for (unsigned int i = 0; i < mUIElements.size(); i++) {
			mUIElements[i]->update();
		}
	}
}

void UIController::show()
{
	mVisible = true;
	timeline().apply( &mAlpha, 1.0f, 0.25f );
}

void UIController::hide()
{
	timeline().apply( &mAlpha, 0.0f, 0.25f ).finishFn( [&]{ mVisible = false; } );
}

UIElementRef UIController::addSlider( const string &aName, float *aValueToLink, const string &aParamString )
{
	UIElementRef sliderRef = Slider::create( this, aName, aValueToLink, aParamString );
	addElement( sliderRef );
	return sliderRef;
}

UIElementRef UIController::addButton( const string &aName, const function<void( bool )> &aEventHandler, const string &aParamString )
{
	UIElementRef buttonRef = Button::create( this, aName, aEventHandler, aParamString );
	addElement( buttonRef );
	return buttonRef;
}

UIElementRef UIController::addLinkedButton( const string &aName, const function<void( bool )> &aEventHandler, bool *aLinkedState, const string &aParamString )
{
	UIElementRef linkedButtonRef = LinkedButton::create( this, aName, aEventHandler, aLinkedState, aParamString );
	addElement( linkedButtonRef );
	return linkedButtonRef;
}

UIElementRef UIController::addLabel( const string &aName, const string &aParamString )
{
	UIElementRef labelRef = Label::create( this, aName, aParamString );
	addElement( labelRef );
	return labelRef;
}

UIElementRef UIController::addImage( const string &aName, ImageSourceRef aImage, const string &aParamString )
{
	UIElementRef imageRef = Image::create( this, aName, aImage, aParamString );
	addElement( imageRef );
	return imageRef;
}

UIElementRef UIController::addSlider2D( const string &aName, Vec2f *aValueToLink, const string &aParamString )
{
	UIElementRef slider2DRef = Slider2D::create( this, aName, aValueToLink, aParamString );
	addElement( slider2DRef );
	return slider2DRef;
}

UIElementRef UIController::addSliderCallback( const std::string &aName, float *aValueToLink, const std::function<void ()> &aEventHandler, const std::string &aParamString )
{
	UIElementRef sliderCallbackRef = SliderCallback::create( this, aName, aValueToLink, aEventHandler, aParamString );
	addElement( sliderCallbackRef );
	return sliderCallbackRef;
}

UIElementRef UIController::addToggleSlider( const string &aSliderName, float *aValueToLink, const string &aButtonName, const function<void (bool)> &aEventHandler, const string &aSliderParamString, const string &aButtonParamString )
{
	// create the slider
	UIElementRef toggleSliderRef = Slider::create( this, aSliderName, aValueToLink, aSliderParamString );

	// add the slider to the controller
	addElement( toggleSliderRef );

	// create the button
	UIElementRef newButtonRef = Button::create( this, aButtonName, aEventHandler, aButtonParamString );

	// add an additional event handler to link the button to the slider
	std::shared_ptr<class Button> newButton = std::static_pointer_cast<class Button>(newButtonRef);
	newButton->addEventHandler( std::bind(&Slider::setLocked, toggleSliderRef, std::placeholders::_1 ) );

	// add the button to the controller
	addElement( newButton );
	return toggleSliderRef;
}

// without event handler
UIElementRef UIController::addMovingGraph(const string &aName, float *aValueToLink, const string &aParamString)
{
	UIElementRef movingGraphRef = MovingGraph::create(this, aName, aValueToLink, aParamString);
	addElement(movingGraphRef);
	return movingGraphRef;
}

// with event handler
// note: this would be an overloaded addMovingGraph function for consistency, were it not for a visual studio compiler defect (see http://cplusplus.github.io/LWG/lwg-active.html#2132)
UIElementRef UIController::addMovingGraphButton(const string &aName, float *aValueToLink, const std::function<void(bool)>& aEventHandler, const string &aParamString)
{
	UIElementRef movingGraphRef = MovingGraph::create(this, aName, aValueToLink, aEventHandler, aParamString);
	addElement(movingGraphRef);
	return movingGraphRef;
}

void UIController::releaseGroup( const string &aGroup )
{
	for (unsigned int i = 0; i < mUIElements.size(); i++) {
		if (mUIElements[i]->getGroup() == aGroup ) {
			mUIElements[i]->release();
		}
	}
}

void UIController::selectGroupElementByName(const std::string &aGroup, const std::string &aName)
{
	for (unsigned int i = 0; i < mUIElements.size(); i++) {
		if ( mUIElements[i]->getGroup() == aGroup ) {
			if ( mUIElements[i]->getName() == aName ) {
				mUIElements[i]->press();
			} else {
				mUIElements[i]->release();
			}
		}
	}
}

void UIController::setLockedByGroup( const std::string &aGroup, const bool &locked )
{
	for (unsigned int i = 0; i < mUIElements.size(); i++) {
		if (mUIElements[i]->getGroup() == aGroup ) {
			mUIElements[i]->setLocked( locked );
		}
	}
}

void UIController::setPressedByGroup( const std::string &aGroup, const bool &pressed )
{
	for (unsigned int i = 0; i < mUIElements.size(); i++) {
		if (mUIElements[i]->getGroup() == aGroup ) {
			pressed ? mUIElements[i]->press() : mUIElements[i]->release();
		}
	}
}

Font UIController::getFont( const string &aStyle )
{
	if ( aStyle == "label" ) {
		return mLabelFont;
	} else if ( aStyle == "icon" ) {
		return mIconFont;
	} else if ( aStyle == "header" ) {
		return mHeaderFont;
	} else if ( aStyle == "body" ) {
		return mBodyFont;
	} else if ( aStyle == "footer" ) {
		return mFooterFont;
	} else if ( aStyle == "smallLabel" ) {
		return mSmallLabelFont;
	} else {
		throw FontStyleExc( aStyle );
	}
}

void UIController::setFont( const string &aStyle, const ci::Font &aFont )
{
	if ( aStyle == "label" ) {
		mLabelFont = aFont;
	} else if ( aStyle == "icon" ) {
		mIconFont = aFont;
	} else if ( aStyle == "header" ) {
		mHeaderFont = aFont;
	} else if ( aStyle == "body" ) {
		mBodyFont = aFont;
	} else if ( aStyle == "footer" ) {
		mFooterFont = aFont;
	} else if ( aStyle == "smallLabel" ) {
		mSmallLabelFont = aFont;
	} else {
		throw FontStyleExc( aStyle );
	}
}

void UIController::setupFbo()
{
	mFormat.enableDepthBuffer( false );
	mFormat.setSamples( mFboNumSamples );
	mFbo = gl::Fbo( DEFAULT_FBO_WIDTH, DEFAULT_FBO_WIDTH, mFormat );
	mFbo.bindFramebuffer();
	gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 0.0f ) );
	mFbo.unbindFramebuffer();
}