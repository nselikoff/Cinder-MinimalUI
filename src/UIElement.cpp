#include "UIElement.h"
#include "UIController.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

int UIElement::DEFAULT_HEIGHT = 36;

UIElement::UIElement( UIController *aUIController, const std::string &aName, const std::string &aParamString )
	: mParent( aUIController ), mName( aName ), mParams( ci::JsonTree( aParamString ) )
{
	// attach mouse callbacks
	mCbMouseDown = mParent->getWindow()->getSignalMouseDown().connect( mParent->getDepth(), std::bind( &UIElement::mouseDown, this, std::placeholders::_1 ) );
	mCbMouseUp = mParent->getWindow()->getSignalMouseUp().connect( mParent->getDepth(), std::bind( &UIElement::mouseUp, this, std::placeholders::_1 ) );
	mCbMouseDrag = mParent->getWindow()->getSignalMouseDrag().connect( mParent->getDepth(), std::bind( &UIElement::mouseDrag, this, std::placeholders::_1 ) );

	// initialize some variables
	mActive = false;

	// parse params that are common to all UIElements
	mGroup = hasParam( "group" ) ? getParam<string>( "group" ) : "";
	mIcon = hasParam( "icon" ) ? getParam<bool>( "icon" ) : false;
	mLocked = hasParam( "locked" ) ? getParam<bool>( "locked" ) : false;
	mClear = hasParam( "clear" ) ? getParam<bool>( "clear" ) : true;

	// JSON doesn't support hex literals...
	// TODO: there's got to be a better way to do this, esp considering I need hex versions of default values set above...
	std::stringstream str;
	string strValue;
	uint32_t hexValue;
	if ( hasParam( "nameColor" ) )
	{
		strValue = getParam<string>( "nameColor" ); 
		str << strValue;
		str >> std::hex >> hexValue;
		mNameColor = ColorA::hexA( hexValue );
	}
	else
	{
		mNameColor = UIController::DEFAULT_NAME_COLOR;
	}
	if ( hasParam( "backgroundColor" ) )
	{
		strValue = getParam<string>( "backgroundColor" ); 
		str.clear();
		str << strValue;
		str >> std::hex >> hexValue;
		mBackgroundColor = ColorA::hexA( hexValue );
	}
	else
	{
		mBackgroundColor = UIController::DEFAULT_BACKGROUND_COLOR;
	}

	if ( hasParam( "justification" ) ) {
		string justification = getParam<string>( "justification" );
		if ( justification == "left" ) {
			mAlignment = TextBox::LEFT;
		} else if ( justification == "right" ) {
			mAlignment = TextBox::RIGHT;
		} else {
			mAlignment = TextBox::CENTER;
		}
	} else {
		mAlignment = TextBox::CENTER;
	}

	if ( hasParam( "style" ) ) {
		mFont = mParent->getFont( getParam<string>( "style" ) );
	} else if ( mIcon ) {
		mFont = mParent->getFont( "icon" );
	} else {
		mFont = mParent->getFont( "label" );
	}

	if ( hasParam( "backgroundImage" ) ) {
		mBackgroundTexture = gl::Texture( loadImage( loadAsset( getParam<string>( "backgroundImage" ) ) ) );
	}
}

void UIElement::offsetInsertPosition()
{
	if ( mClear ) {
		mParent->resetInsertPosition( mBounds.getHeight() + UIController::DEFAULT_MARGIN_SMALL );
	} else {
		mParent->offsetInsertPosition( Vec2i( mBounds.getWidth() + UIController::DEFAULT_MARGIN_SMALL, 0 ) );
	}
}

void UIElement::setPositionAndBounds()
{
	mPosition = mParent->getInsertPosition();
	mBounds = Area( mPosition, mPosition + mSize );

	// offset the insert position
	offsetInsertPosition();
}

void UIElement::mouseDown( MouseEvent &event )
{
	if ( mParent->isVisible() && !mLocked && mBounds.contains( event.getPos() - mParent->getPosition() ) ) {
		mActive = true;
		handleMouseDown( event.getPos() - mParent->getPosition(), event.isRight() );
		event.setHandled();
	}
}

void UIElement::mouseUp( MouseEvent &event )
{
	if ( mParent->isVisible() && !mLocked && mActive ) {
		mActive = false;
		handleMouseUp( event.getPos() - mParent->getPosition() );
		//		event.setHandled(); // maybe?
	}
}

void UIElement::mouseDrag( MouseEvent &event )
{
	if ( mParent->isVisible() && !mLocked && mActive ) {
		handleMouseDrag( event.getPos() - mParent->getPosition() );
	}
}

void UIElement::renderNameTexture()
{
	TextBox textBox = TextBox().size( Vec2i( mSize.x * 2 , TextBox::GROW ) ).font( mFont ).color( mNameColor ).alignment( mAlignment ).text( mName );
	mNameTexture = textBox.render();
}

void UIElement::drawLabel()
{
	gl::pushMatrices();
	gl::color( Color::white() );
	
	// draw the background texture if it's defined
	if ( mBackgroundTexture ) gl::draw( mBackgroundTexture, getBounds() );
	
	// lower right of the name texture
	Vec2i texLR = toPixels( mNameTexture.getBounds().getLR() / 2 );
	
	// offset by the upper left of the bounds of the UIElement
	Vec2i offset = getBounds().getUL();
	
	// vertically center the label
	offset += Vec2i( 0, ( getBounds().getHeight() - toPixels( mNameTexture.getHeight() / 2 ) ) / 2 );
	
	// draw the label
	gl::draw( mNameTexture, Area( offset, offset + texLR ) );

	gl::popMatrices();
}