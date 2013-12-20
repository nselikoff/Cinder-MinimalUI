MinimalUI#include "Label.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

int Label::DEFAULT_WIDTH = UIElement::DEFAULT_HEIGHT * 2 + UIController::DEFAULT_MARGIN_SMALL;

Label::Label( UIController *aUIController, const string &aName, const string &aParamString ) : UIElement( aUIController, aName, aParamString )
{
    // initialize unique variables
    mNarrow = hasParam( "narrow" ) ? getParam<bool>( "narrow" ) : false;

    // set initial size and render name texture
    int x = hasParam( "width" ) ? getParam<int>( "width" ) : Label::DEFAULT_WIDTH;
    setSize( Vec2i( x, 0 ) );
    renderNameTexture();
    
    // set actual size based on the height of the name texture
    int y;
    if ( mNarrow ) {
        y = math<float>::min( getNameTexture().getHeight(), UIElement::DEFAULT_HEIGHT );
    } else {
        y = math<float>::max( getNameTexture().getHeight(), UIElement::DEFAULT_HEIGHT );
    }
    setSize( Vec2i( getSize().x, y ) );

    // set position and bounds
    setPositionAndBounds();
}

UIElementRef Label::create( UIController *aUIController, const string &aName, const string &aParamString )
{
    return shared_ptr<Label>( new Label( aUIController, aName, aParamString ) );
}

void Label::draw()
{
    // draw the label
    drawLabel();
}


