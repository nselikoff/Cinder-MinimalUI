#include "Image.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

Image::Image( UIControllerRef parent, const string &aName, ImageSourceRef aImage, JsonTree json ) : UIElement( parent, aName, json )
{
	// initialize unique variables
	
	// create texture from data source
	setBackgroundTexture(gl::Texture::create(aImage));
	
	// set actual size based on the height of the texture, divided by 2 because we render it twice as large for retina displays
	// if width and/or height are specified, override size
	int x = hasParam("width") ? getParam<int>("width") : getBackgroundTexture()->getWidth() / 2;
	int y = hasParam( "height" ) ? getParam<int>( "height" ) : getBackgroundTexture()->getHeight() / 2;
	setSize( ivec2( x, y ) );
	
	// set position and bounds
	setPositionAndBounds();
}

UIElementRef Image::create( UIControllerRef parent, const string &aName, ImageSourceRef aImage, JsonTree json )
{
	return shared_ptr<Image>( new Image( parent, aName, aImage, json ) );
}

void Image::drawImpl()
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

