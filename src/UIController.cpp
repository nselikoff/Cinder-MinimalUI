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
ci::ColorA UIController::DEFAULT_STROKE_COLOR = ci::ColorA(0.07f, 0.26f, 0.29f, 1.0f);
ci::ColorA UIController::ACTIVE_STROKE_COLOR = ci::ColorA(0.19f, 0.66f, 0.71f, 1.0f);
ci::ColorA UIController::DEFAULT_NAME_COLOR = ci::ColorA(0.14f, 0.49f, 0.54f, 1.0f);
ci::ColorA UIController::DEFAULT_BACKGROUND_COLOR = ci::ColorA(0.0f, 0.0f, 0.0f, 1.0f);

UIController::UIController(app::WindowRef aWindow, JsonTree json)
	: mWindow(aWindow)
{
	mVisible = json.hasChild("visible") ? json["visible"].getValue<bool>() : true;
	mAlpha = mVisible ? 1.0f : 0.0f;
	mWidth = json.hasChild("width") ? json["width"].getValue<int>() : DEFAULT_PANEL_WIDTH;
	mX = json.hasChild("x") ? json["x"].getValue<int>() : 0;
	mY = json.hasChild("y") ? json["y"].getValue<int>() : 0;
	if (json.hasChild("height")) {
		mHeightSpecified = true;
		mHeight = json["height"].getValue<int>();
	}
	else {
		mHeightSpecified = false;
		mHeight = getWindow()->getHeight();
	}
	mCentered = json.hasChild("centered") ? json["centered"].getValue<bool>() : false;
	mDepth = json.hasChild("depth") ? json["depth"].getValue<int>() : 0;
	mForceInteraction = json.hasChild("forceInteraction") ? json["forceInteraction"].getValue<bool>() : false;
	mMarginLarge = json.hasChild("marginLarge") ? json["marginLarge"].getValue<int>() : DEFAULT_MARGIN_LARGE;

	// JSON doesn't support hex literals...
	std::stringstream str;
	string panelColor = json.hasChild("panelColor") ? json["panelColor"].getValue<string>() : "0xCC000000";
	str << panelColor;
	uint32_t hexValue;
	str >> std::hex >> hexValue;
	mPanelColor = ColorA::hexA(hexValue);

	if (json.hasChild("defaultStrokeColor"))
	{
		string strValue = json["defaultStrokeColor"].getValue<string>();
		str.clear();
		str << strValue;
		str >> std::hex >> hexValue;
		UIController::DEFAULT_STROKE_COLOR = ColorA::hexA(hexValue);
	}
	if (json.hasChild("activeStrokeColor"))
	{
		string strValue = json["activeStrokeColor"].getValue<string>();
		str.clear();
		str << strValue;
		str >> std::hex >> hexValue;
		UIController::ACTIVE_STROKE_COLOR = ColorA::hexA(hexValue);
	}
	if (json.hasChild("defaultNameColor"))
	{
		string strValue = json["defaultNameColor"].getValue<string>();
		str.clear();
		str << strValue;
		str >> std::hex >> hexValue;
		UIController::DEFAULT_NAME_COLOR = ColorA::hexA(hexValue);
	}
	if (json.hasChild("defaultBackgroundColor"))
	{
		string strValue = json["defaultBackgroundColor"].getValue<string>();
		str.clear();
		str << strValue;
		str >> std::hex >> hexValue;
		UIController::DEFAULT_BACKGROUND_COLOR = ColorA::hexA(hexValue);
	}
	// draw everything once
	mIsDirty = true;

	resize();

	mCbMouseDown = mWindow->getSignalMouseDown().connect(mDepth + 99, std::bind(&UIController::mouseDown, this, std::placeholders::_1));

	// set default fonts
	setFont("label", Font("Arial", 16 * 2));
	setFont("smallLabel", Font("Arial", 12 * 2));
	setFont("icon", Font("Arial", 22 * 2));
	setFont("header", Font("Arial", 48 * 2));
	setFont("body", Font("Arial", 19 * 2));
	setFont("footer", Font("Arial Italic", 14 * 2));

	mInsertPosition = ivec2(mMarginLarge, mMarginLarge);

	mFboNumSamples = json.hasChild("fboNumSamples") ? json["fboNumSamples"].getValue<int>() : 0;
	if (json.hasChild("backgroundImage")) {
		mBackgroundTexture = gl::Texture::create(loadImage(loadAsset(json["backgroundImage"].getValue<string>())));
	}
	setupFbo();
}

UIControllerRef UIController::create(const string &aParamString, app::WindowRef aWindow)
{
	JsonTree json( aParamString );
	return shared_ptr<UIController>(new UIController(aWindow, json));
}

UIControllerRef UIController::create( DataSourceRef dataSource, app::WindowRef aWindow)
{
	JsonTree json( dataSource );
	return shared_ptr<UIController>(new UIController(aWindow, json));
}

void UIController::resize()
{
	ivec2 size;
	if (mCentered) {
		size = ivec2(mWidth, mHeight);
		mPosition = ivec2(getWindow()->getCenter().x - size.x / 2, getWindow()->getCenter().y - size.y / 2);
	}
	else if (mHeightSpecified) {
		size = ivec2(mWidth, mHeight);
		mPosition = ivec2(mX, mY);
	}
	else {
		size = ivec2(mWidth, getWindow()->getHeight());
		mPosition = ivec2(mX, mY);
	}
	mBounds = Area(ivec2(0), size);
	
	for ( auto controller : mChildControllers ) {
		controller->resize();
	}
}

void UIController::mouseDown(MouseEvent &event)
{
	for ( auto controller : mChildControllers ) {
		controller->mouseDown( event );
	}

	if ( mVisible && ! event.isHandled() ) {
		if ((mBounds + mPosition).contains(event.getPos()) || mForceInteraction)
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

void UIController::renderToFbo()
{
	if (getElapsedFrames() % DEFAULT_UPDATE_FREQUENCY == 0 && mIsDirty)
	{

		// this will restore the old framebuffer binding when we leave this function
		// on non-OpenGL ES platforms, you can just call mFbo->unbindFramebuffer() at the end of the function
		// but this will restore the "screen" FBO on OpenGL ES, and does the right thing on both platforms
		gl::ScopedFramebuffer fbScp(mFbo);

		gl::lineWidth(toPixels(2.0f));
		//	gl::enable( GL_LINE_SMOOTH );
		gl::enableAlphaBlending();
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		// clear and set viewport and matrices to match the position and dimensions of the UIController
		gl::clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
		// note that the first parameter is the lower left position, hence the offset. otherwise the controller will be drawn in the bottom left of the big FBO.
		// TODO: test this on retina
		gl::ScopedViewport scpVp(ivec2(mPosition.x, -mPosition.y) + ivec2(0, mFbo->getHeight() - mBounds.getHeight()), mBounds.getSize());
		gl::setMatricesWindow(mBounds.getSize());

		// draw backing panel
		gl::color(mPanelColor);
		gl::drawSolidRect(toPixels(mBounds));

		// draw the background
		drawBackground();

		// draw elements
		for (unsigned int i = 0; i < mUIElements.size(); i++) {
			mUIElements[i]->draw();
		}
		mIsDirty = false;
	}
}
void UIController::draw()
{
	if (!mVisible)
		return;

	// save state
	gl::pushMatrices();
	//BL glPushAttrib( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_LINE_BIT | GL_CURRENT_BIT );

	// disable depth read (otherwise any 3d drawing done after this will be obscured by the FBO; not exactly sure why)
	gl::disableDepthRead();

	// render to Fbo
	renderToFbo();

	// reset the matrices and blending
	gl::setMatricesWindow(toPixels(getWindow()->getSize()));
	gl::enableAlphaBlending(true);

	// if forcing interaction, draw an overlay over the whole window
	if (mForceInteraction) {
		gl::color(ColorA(0.0f, 0.0f, 0.0f, 0.5f * mAlpha));
		gl::drawSolidRect(toPixels(getWindow()->getBounds()));
	}

	// draw the FBO to the screen
	gl::color(ColorA(mAlpha, mAlpha, mAlpha, mAlpha));
	gl::draw(mFbo->getColorTexture());

	gl::disableAlphaBlending();

	// restore state
	//BL glPopAttrib();
	gl::popMatrices();

	for ( auto controller : mChildControllers ) {
		controller->draw();
	}
}

void UIController::update()
{
	if (!mVisible)
		return;

	if (getElapsedFrames() % DEFAULT_UPDATE_FREQUENCY == 0 && mIsDirty) {
		for (unsigned int i = 0; i < mUIElements.size(); i++) {
			mUIElements[i]->update();
		}
	}

	for ( auto controller : mChildControllers ) {
		controller->update();
	}
}

void UIController::show()
{
	mVisible = true;
	timeline().apply(&mAlpha, 1.0f, 0.25f);
}

void UIController::hide()
{
	timeline().apply(&mAlpha, 0.0f, 0.25f).finishFn([&]{ mVisible = false; });
}

void UIController::setDirty()
{
	mIsDirty = true;
}

UIElementRef UIController::addSlider(const string &aName, float *aValueToLink, const string &aParamString)
{
	UIElementRef sliderRef = Slider::create( shared_from_this(), aName, aValueToLink, JsonTree(aParamString));
	addElement(sliderRef);
	return sliderRef;
}

UIElementRef UIController::addButton(const string &aName, const function<void(bool)> &aEventHandler, const string &aParamString)
{
	UIElementRef buttonRef = Button::create(shared_from_this(), aName, aEventHandler, JsonTree(aParamString));
	addElement(buttonRef);
	return buttonRef;
}

UIElementRef UIController::addLinkedButton(const string &aName, const function<void(bool)> &aEventHandler, bool *aLinkedState, const string &aParamString)
{
	UIElementRef linkedButtonRef = LinkedButton::create(shared_from_this(), aName, aEventHandler, aLinkedState, JsonTree(aParamString));
	addElement(linkedButtonRef);
	return linkedButtonRef;
}

UIElementRef UIController::addLabel(const string &aName, const string &aParamString)
{
	UIElementRef labelRef = Label::create(shared_from_this(), aName, JsonTree(aParamString));
	addElement(labelRef);
	return labelRef;
}

UIElementRef UIController::addImage(const string &aName, ImageSourceRef aImage, const string &aParamString)
{
	UIElementRef imageRef = Image::create(shared_from_this(), aName, aImage, JsonTree(aParamString));
	addElement(imageRef);
	return imageRef;
}

UIElementRef UIController::addSlider2D(const string &aName, vec2 *aValueToLink, const string &aParamString)
{
	UIElementRef slider2DRef = Slider2D::create(shared_from_this(), aName, aValueToLink, JsonTree(aParamString));
	addElement(slider2DRef);
	return slider2DRef;
}

UIElementRef UIController::addSliderCallback(const std::string &aName, float *aValueToLink, const std::function<void()> &aEventHandler, const std::string &aParamString)
{
	UIElementRef sliderCallbackRef = SliderCallback::create(shared_from_this(), aName, aValueToLink, aEventHandler, JsonTree(aParamString));
	addElement(sliderCallbackRef);
	return sliderCallbackRef;
}

UIElementRef UIController::addToggleSlider(const string &aSliderName, float *aValueToLink, const string &aButtonName, const function<void(bool)> &aEventHandler, const string &aSliderParamString, const string &aButtonParamString)
{
	// create the slider
	UIElementRef toggleSliderRef = Slider::create(shared_from_this(), aSliderName, aValueToLink, JsonTree(aSliderParamString));

	// add the slider to the controller
	addElement(toggleSliderRef);

	// create the button
	UIElementRef newButtonRef = Button::create(shared_from_this(), aButtonName, aEventHandler, JsonTree(aButtonParamString));

	// add an additional event handler to link the button to the slider
	std::shared_ptr<class Button> newButton = std::static_pointer_cast<class Button>(newButtonRef);
	newButton->addEventHandler(std::bind(&Slider::setLocked, toggleSliderRef, std::placeholders::_1));

	// add the button to the controller
	addElement(newButton);
	return toggleSliderRef;
}

// without event handler
UIElementRef UIController::addMovingGraph(const string &aName, float *aValueToLink, const string &aParamString)
{
	UIElementRef movingGraphRef = MovingGraph::create(shared_from_this(), aName, aValueToLink, JsonTree(aParamString));
	addElement(movingGraphRef);
	return movingGraphRef;
}

// with event handler
// note: this would be an overloaded addMovingGraph function for consistency, were it not for a visual studio compiler defect (see http://cplusplus.github.io/LWG/lwg-active.html#2132)
UIElementRef UIController::addMovingGraphButton(const string &aName, float *aValueToLink, const std::function<void(bool)>& aEventHandler, const string &aParamString)
{
	UIElementRef movingGraphRef = MovingGraph::create(shared_from_this(), aName, aValueToLink, aEventHandler, JsonTree(aParamString));
	addElement(movingGraphRef);
	return movingGraphRef;
}

void UIController::releaseGroup(const string &aGroup)
{
	for (unsigned int i = 0; i < mUIElements.size(); i++) {
		if (mUIElements[i]->getGroup() == aGroup) {
			mUIElements[i]->release();
		}
	}
}

void UIController::selectGroupElementByName(const std::string &aGroup, const std::string &aName)
{
	for (unsigned int i = 0; i < mUIElements.size(); i++) {
		if (mUIElements[i]->getGroup() == aGroup) {
			if (mUIElements[i]->getName() == aName) {
				mUIElements[i]->press();
			}
			else {
				mUIElements[i]->release();
			}
		}
	}
}

void UIController::setLockedByGroup(const std::string &aGroup, const bool &locked)
{
	for (unsigned int i = 0; i < mUIElements.size(); i++) {
		if (mUIElements[i]->getGroup() == aGroup) {
			mUIElements[i]->setLocked(locked);
		}
	}
}

void UIController::setPressedByGroup(const std::string &aGroup, const bool &pressed)
{
	for (unsigned int i = 0; i < mUIElements.size(); i++) {
		if (mUIElements[i]->getGroup() == aGroup) {
			pressed ? mUIElements[i]->press() : mUIElements[i]->release();
		}
	}
}

Font UIController::getFont(const string &aStyle)
{
	if (aStyle == "label") {
		return mLabelFont;
	}
	else if (aStyle == "icon") {
		return mIconFont;
	}
	else if (aStyle == "header") {
		return mHeaderFont;
	}
	else if (aStyle == "body") {
		return mBodyFont;
	}
	else if (aStyle == "footer") {
		return mFooterFont;
	}
	else if (aStyle == "smallLabel") {
		return mSmallLabelFont;
	}
	else {
		throw FontStyleExc(aStyle);
	}
}

void UIController::setFont(const string &aStyle, const ci::Font &aFont)
{
	if (aStyle == "label") {
		mLabelFont = aFont;
	}
	else if (aStyle == "icon") {
		mIconFont = aFont;
	}
	else if (aStyle == "header") {
		mHeaderFont = aFont;
	}
	else if (aStyle == "body") {
		mBodyFont = aFont;
	}
	else if (aStyle == "footer") {
		mFooterFont = aFont;
	}
	else if (aStyle == "smallLabel") {
		mSmallLabelFont = aFont;
	}
	else {
		throw FontStyleExc(aStyle);
	}
}

void UIController::setupFbo()
{
	mFormat.enableDepthBuffer(false);
	mFormat.setSamples(mFboNumSamples);
	mFbo = gl::Fbo::create(DEFAULT_FBO_WIDTH, DEFAULT_FBO_WIDTH, mFormat);
	mFbo->bindFramebuffer();
	gl::clear(ColorA(0.0f, 0.0f, 0.0f, 0.0f));
	mFbo->unbindFramebuffer();
}