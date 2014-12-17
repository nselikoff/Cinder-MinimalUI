#include "Label.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

int Label::DEFAULT_WIDTH = UIElement::DEFAULT_HEIGHT * 2 + UIController::DEFAULT_MARGIN_SMALL;

Label::Label( UIControllerRef parent, const string &aName, JsonTree json ) : UIElement( parent, aName, json )
{
	// initialize unique variables
	mNarrow = hasParam( "narrow" ) ? getParam<bool>( "narrow" ) : false;

	// set initial size and render name texture
	int x = hasParam( "width" ) ? getParam<int>( "width" ) : Label::DEFAULT_WIDTH;
	setSize( ivec2( x, 0 ) );
	renderNameTexture();
	
	// set actual size based on the height of the name texture, divided by 2 because we render it twice as large for retina displays
	int y;
	if ( mNarrow ) {
		y = math<float>::min(getNameTexture()->getHeight() / 2, UIElement::DEFAULT_HEIGHT);
	} else {
		y = math<float>::max(getNameTexture()->getHeight() / 2, UIElement::DEFAULT_HEIGHT);
	}
	setSize( ivec2( mSize.x, y ) );

	// set position and bounds
	setPositionAndBounds();
}

UIElementRef Label::create( UIControllerRef parent, const string &aName, JsonTree json )
{
	return shared_ptr<Label>( new Label( parent, aName, json ) );
}

void Label::drawImpl()
{
	// draw the background
	drawBackground();

	// draw the label
	drawLabel();
}

void Label::setPositionAndBounds()
{
	mPosition = getParent()->getInsertPosition();
	mBounds = Area( mPosition, mPosition + mSize );
	
	// offset the insert position
	offsetInsertPosition();
}

