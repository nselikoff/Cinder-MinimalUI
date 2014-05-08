#include "Image.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

Image::Image( UIController *aUIController, const string &aName, ImageSourceRef aImage, const string &aParamString ) : UIElement( aUIController, aName, aParamString )
{
	// initialize unique variables
	
	// create texture from data source
	setBackgroundTexture( gl::Texture( aImage ) );
	
	// set actual size based on the height of the texture, divided by 2 because we render it twice as large for retina displays
	// if width and/or height are specified, override size
	int x = hasParam( "width" ) ? getParam<int>( "width" ) : getBackgroundTexture().getWidth() / 2;
	int y = hasParam( "height" ) ? getParam<int>( "height" ) : getBackgroundTexture().getHeight() / 2;
	setSize( Vec2i( x, y ) );
	
	// set position and bounds
	setPositionAndBounds();
}

UIElementRef Image::create( UIController *aUIController, const string &aName, ImageSourceRef aImage, const string &aParamString )
{
	return shared_ptr<Image>( new Image( aUIController, aName, aImage, aParamString ) );
}

void Image::draw()
{
	drawBackground();
}

void Image::setPositionAndBounds()
{
	mPosition = getParent()->getInsertPosition();
	mBounds = Area( mPosition, mPosition + mSize );
	
	// offset the insert position
	offsetInsertPosition();
}

