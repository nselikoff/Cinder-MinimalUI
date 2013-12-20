#include "UserInterface.h"

using namespace ci;
using namespace ci::app;
using namespace MinimalUI;

UserInterface::UserInterface( ParameterBagRef aParameterBag, app::WindowRef aWindow, LeapMotion::SignalDeviceRef aDevice, SceneRef aScene )
{
    mWindow = aWindow;
    mDevice = aDevice;
    mScene = aScene;
    
    mCbMouseDown = mWindow->getSignalMouseDown().connect( 0, std::bind( &UserInterface::mouseDown, this, std::placeholders::_1 ) );
    mCbKeyDown = mWindow->getSignalKeyDown().connect( 0, std::bind( &UserInterface::keyDown, this, std::placeholders::_1 ) );
    
    mCbLeapConnect = mDevice->getSignalLeapConnect().connect( std::bind( &UserInterface::onLeapConnect, this ) );
    mCbLeapDisconnect = mDevice->getSignalLeapDisconnect().connect( std::bind( &UserInterface::onLeapDisconnect, this ) );
    
    mParameterBag = aParameterBag;
    mVisible = true;
    mIgnoringDisconnect = false;
	mSetupComplete = false;

    mTimer = 0.0f;
}

UserInterfaceRef UserInterface::create( ParameterBagRef aParameterBag, app::WindowRef aWindow, LeapMotion::SignalDeviceRef aDevice, SceneRef aScene )
{
    return shared_ptr<UserInterface>( new UserInterface( aParameterBag, aWindow, aDevice, aScene ) );
}

void UserInterface::setup()
{
    setupDisconnected();
    setupWelcome();
    setupImageSaved();
    setupCredits();
    setupTutorial();

    setupParams();

    setupMiniControl();

	mSetupComplete = true;
}

void UserInterface::setupParams()
{
    mParams = UIController::create( "{ \"depth\":200, \"panelColor\":\"0xEE000000\" }" );
    
    mParams->offsetInsertPosition( Vec2i( 0, 50 ) );
    
    // 2d sliders for coefficients
    mParams->addSlider2D( "ab", &mParameterBag->mAB, "{ \"minX\":-4.0, \"maxX\":4.0, \"minY\":-4.0, \"maxY\":4.0, \"clear\":false }" );
    mParams->addSlider2D( "cd", &mParameterBag->mCD, "{ \"minX\":-4.0, \"maxX\":4.0, \"minY\":-4.0, \"maxY\":4.0 }" );
    
    // spread
    string sliderParams = "{ \"width\":156, \"clear\":false, \"min\":0.1, \"max\":2.0, \"locked\":true }";
    string buttonParams = "{ \"stateless\":false, \"pressed\":true }";
    mParams->addToggleSlider( "Spread", &mParameterBag->mE, "A", std::bind(&UserInterface::lockSpread, this, std::placeholders::_1 ), sliderParams, buttonParams );
    
    // randomness
    sliderParams = "{ \"width\":156, \"clear\":false, \"min\":1.0, \"max\":1000.0, \"locked\":true }";
    mParams->addToggleSlider( "Randomness", &mParameterBag->mRandomness, "A", std::bind(&UserInterface::lockRandomness, this, std::placeholders::_1 ), sliderParams, buttonParams );
    mParams->addSlider( "Smoothing", &mParameterBag->mSmoothing, "{ \"min\":0.0, \"max\":0.95 }" );
    
    mParams->addSeparator();
    
    // camera controls
    mParams->offsetInsertPosition( Vec2i( 40, 0 ) );
    // counterclockwise
    mParams->addLinkedButton( "", std::bind(&UserInterface::rotateCamera, this, "counterclockwise", std::placeholders::_1 ), &mParameterBag->mCameraCounterclockwise, "{ \"icon\": true, \"clear\":false, \"continuous\":true }" );
    // up
    mParams->addLinkedButton( "", std::bind(&UserInterface::rotateCamera, this, "up", std::placeholders::_1 ), &mParameterBag->mCameraUp, "{ \"icon\": true, \"clear\":false, \"continuous\":true }" );
    // clockwise (clear)
    mParams->addLinkedButton( "", std::bind(&UserInterface::rotateCamera, this, "clockwise", std::placeholders::_1 ), &mParameterBag->mCameraClockwise, "{ \"icon\": true, \"continuous\":true }" );

    mParams->offsetInsertPosition( Vec2i( 40, 0 ) );
    // left
    mParams->addLinkedButton( "", std::bind(&UserInterface::rotateCamera, this, "left", std::placeholders::_1 ), &mParameterBag->mCameraLeft, "{ \"icon\": true, \"clear\":false, \"continuous\":true }" );
    // auto
    mParams->addButton( "A", std::bind(&UserInterface::lockCamera, this, std::placeholders::_1 ), "{ \"pressed\":true, \"stateless\":false, \"clear\":false }" );
    // right (clear)
    mParams->addLinkedButton( "", std::bind(&UserInterface::rotateCamera, this, "right", std::placeholders::_1 ), &mParameterBag->mCameraRight, "{ \"icon\": true, \"continuous\":true }" );

    mParams->offsetInsertPosition( Vec2i( 40, 0 ) );
    // out
    mParams->addLinkedButton( "−", std::bind(&UserInterface::rotateCamera, this, "out", std::placeholders::_1 ), &mParameterBag->mCameraOut, "{ \"icon\": true, \"clear\":false, \"continuous\":true }" );
    // down
    mParams->addLinkedButton( "", std::bind(&UserInterface::rotateCamera, this, "down", std::placeholders::_1 ), &mParameterBag->mCameraDown, "{ \"icon\": true, \"clear\":false, \"continuous\":true }" );
    // in (clear)
    mParams->addLinkedButton( "+", std::bind(&UserInterface::rotateCamera, this, "in", std::placeholders::_1 ), &mParameterBag->mCameraIn, "{ \"icon\": true, \"continuous\":true }" );

    mParams->addSlider( "Cam Sensitivity", &mParameterBag->mSensitivity, "{ \"min\":0.05, \"max\":1.0 }" );
    
    mParams->addSeparator();
    
    // opacity, fade, exposure, gamma
    mParams->addSlider( "Opacity", &mParameterBag->mFgAlpha );
    mParams->addSlider( "Fade", &mParameterBag->mBgAlpha );
    mParams->addSlider( "Exposure", &mParameterBag->mExposure, "{ \"min\":0.0001, \"max\":2.0 }" );
    mParams->addSlider( "Gamma", &mParameterBag->mGamma, "{ \"min\":0.2, \"max\":1.5 }" );
    
    mParams->addSeparator();

    // looks
    mParams->addLabel( "Looks", "{ \"clear\":false }" );
    mParams->addButton( "1", std::bind( &UserInterface::setLook, this, 0, std::placeholders::_1 ), "{ \"clear\":false }" );
    mParams->addButton( "2", std::bind( &UserInterface::setLook, this, 1, std::placeholders::_1 ), "{ \"clear\":false }" );
    mParams->addButton( "3", std::bind( &UserInterface::setLook, this, 2, std::placeholders::_1 ) );
    
    mParams->addSeparator();
    
    // colors
    mParams->addLabel( "Colors", "{ \"clear\":false }" );
    mParams->addButton( "1", std::bind( &UserInterface::setColorMode, this, 0, std::placeholders::_1 ), "{ \"stateless\":false, \"group\":\"colors\", \"exclusive\":true, \"clear\":false, \"pressed\":true }" );
    mParams->addButton( "2", std::bind( &UserInterface::setColorMode, this, 1, std::placeholders::_1 ), "{ \"stateless\":false, \"group\":\"colors\", \"exclusive\":true, \"clear\":false }" );
    mParams->addButton( "3", std::bind( &UserInterface::setColorMode, this, 2, std::placeholders::_1 ), "{ \"stateless\":false, \"group\":\"colors\", \"exclusive\":true }" );
    
    mParams->addSeparator();
    mParams->addSeparator();

    // text
    mParams->offsetInsertPosition( Vec2i( 40, 0 ) );
    mParams->addButton( "Credits", std::bind( &UserInterface::showCredits, this, std::placeholders::_1 ), "{ \"width\":116 }" );
}

void UserInterface::setupCredits()
{
    mCredits = UIController::create( "{ \"visible\":false, \"centered\":true, \"panelColor\":\"0xE5FFFFFF\", \"width\":450, \"forceInteraction\":true, \"marginLarge\":25 }" );

    mCredits->addLabel( "Credits", "{ \"style\":\"header\", \"justification\":\"left\", \"width\":400, \"nameColor\":\"0xFF444444\" }" );

    mCredits->addSeparator();
    
    mCredits->addLabel( "Beautiful Chaos v1.2.1 was lovingly crafted by fine artist and programmer Nathan Selikoff using Cinder, an open source library for professional-quality creative coding in C++. User interface icons licensed from GLYPHICONS.com.", "{ \"style\":\"body\", \"justification\":\"left\", \"width\":400, \"nameColor\":\"0xFF444444\" }" );
    
    mCredits->addSeparator();

    mCredits->addButton( "BeautifulChaosApp.com", std::bind( &UserInterface::gotoURL, this, "http://beautifulchaosapp.com", std::placeholders::_1 ), "{ \"width\":198, \"backgroundColor\":\"0xFFFFFFFF\", \"clear\":false }" );
    mCredits->addButton( "NathanSelikoff.com", std::bind( &UserInterface::gotoURL, this, "http://nathanselikoff.com", std::placeholders::_1 ), "{ \"width\":198, \"backgroundColor\":\"0xFFFFFFFF\" }" );
    mCredits->addButton( "libcinder.org", std::bind( &UserInterface::gotoURL, this, "http://libcinder.org", std::placeholders::_1 ), "{ \"width\":198, \"backgroundColor\":\"0xFFFFFFFF\", \"clear\":false }" );
    mCredits->addButton( "GLYPHICONS.com", std::bind( &UserInterface::gotoURL, this, "http://glyphicons.com", std::placeholders::_1 ), "{ \"width\":198, \"backgroundColor\":\"0xFFFFFFFF\" }" );

    mCredits->addSeparator();
    
    mCredits->addButton( "Close", std::bind(&UserInterface::hideCredits, this, std::placeholders::_1 ), "{ \"width\":400, \"backgroundColor\":\"0xFFFFFFFF\" }" );
    
    mCredits->setHeight();
}

void UserInterface::setupTutorial()
{
    mTutorial1 = UIController::create( "{ \"visible\":false, \"centered\":true, \"panelColor\":\"0xE5FFFFFF\", \"width\":600, \"forceInteraction\":true, \"marginLarge\":25 }" );
    mTutorial1->addLabel( "Using Beautiful Chaos", "{ \"style\":\"header\", \"justification\":\"left\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial1->addSeparator();
    mTutorial1->addLabel( "Welcome to Beautiful Chaos, an experimental art app you can use to explore the curves and ripples of a mathematical equation brought to life in vivid color.", "{ \"style\":\"body\", \"justification\":\"left\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial1->addLabel( "1/4", "{ \"style\":\"footer\", \"justification\":\"right\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial1->addButton( "Exit Tutorial", std::bind( &UserInterface::hideTutorial, this, std::placeholders::_1 ), "{ \"width\":272, \"clear\":false }" );
    mTutorial1->addButton( "Next", std::bind( &UserInterface::showTutorial, this, 2, std::placeholders::_1 ), "{ \"width\":272, \"backgroundColor\":\"0xFFFFFFFF\" }" );
    mTutorial1->setHeight();

    mTutorial2 = UIController::create( "{ \"visible\":false, \"centered\":true, \"panelColor\":\"0xE5FFFFFF\", \"width\":600, \"forceInteraction\":true, \"marginLarge\":25 }" );
    mTutorial2->addLabel( "Using Beautiful Chaos", "{ \"style\":\"header\", \"justification\":\"left\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial2->addSeparator();
    mTutorial2->addLabel( "Activate your control of the chaotic cloud by reaching out two hands above the Leap Motion Controller™ with your fingers spread out. Experiment with fast movement and slow movement.", "{ \"style\":\"body\", \"justification\":\"left\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial2->addLabel( "2/4", "{ \"style\":\"footer\", \"justification\":\"right\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial2->addButton( "Exit Tutorial", std::bind( &UserInterface::hideTutorial, this, std::placeholders::_1 ), "{ \"width\":272, \"clear\":false }" );
    mTutorial2->addButton( "Next", std::bind( &UserInterface::showTutorial, this, 3, std::placeholders::_1 ), "{ \"width\":272, \"backgroundColor\":\"0xFFFFFFFF\" }" );
    mTutorial2->setHeight();
    
    mTutorial3 = UIController::create( "{ \"visible\":false, \"centered\":true, \"panelColor\":\"0xE5FFFFFF\", \"width\":600, \"forceInteraction\":true, \"marginLarge\":25 }" );
    mTutorial3->addLabel( "Using Beautiful Chaos", "{ \"style\":\"header\", \"justification\":\"left\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial3->addSeparator();
    mTutorial3->addLabel( "Move the virtual camera by reaching out one hand above the controller, with your fingers spread out. When you discover an image you like, click the camera to save it.", "{ \"style\":\"body\", \"justification\":\"left\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial3->addLabel( "3/4", "{ \"style\":\"footer\", \"justification\":\"right\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial3->addButton( "Exit Tutorial", std::bind( &UserInterface::hideTutorial, this, std::placeholders::_1 ), "{ \"width\":272, \"clear\":false }" );
    mTutorial3->addButton( "Next", std::bind( &UserInterface::showTutorial, this, 4, std::placeholders::_1 ), "{ \"width\":272, \"backgroundColor\":\"0xFFFFFFFF\" }" );
    mTutorial3->setHeight();
    
    mTutorial4 = UIController::create( "{ \"visible\":false, \"centered\":true, \"panelColor\":\"0xE5FFFFFF\", \"width\":600, \"forceInteraction\":true, \"marginLarge\":25 }" );
    mTutorial4->addLabel( "Using Beautiful Chaos", "{ \"style\":\"header\", \"justification\":\"left\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial4->addSeparator();
    mTutorial4->addLabel( "Play with the other sliders and buttons to change how everything looks. Once you're an expert, you can click the gear icon to hide the parameters. Full documentation is available online.", "{ \"style\":\"body\", \"justification\":\"left\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial4->addLabel( "4/4", "{ \"style\":\"footer\", \"justification\":\"right\", \"width\":550, \"nameColor\":\"0xFF444444\" }" );
    mTutorial4->addButton( "Full Documentation", std::bind( &UserInterface::gotoURL, this, "http://beautifulchaosapp.com/support/", std::placeholders::_1 ), "{ \"width\":272, \"clear\":false }" );
    mTutorial4->addButton( "Exit Tutorial", std::bind( &UserInterface::hideTutorial, this, std::placeholders::_1 ), "{ \"width\":272, \"backgroundColor\":\"0xFFFFFFFF\" }" );
    mTutorial4->setHeight();
}

void UserInterface::setupMiniControl()
{
    mMiniControl = UIController::create( "{ \"depth\":100, \"panelColor\":\"0x00000000\", \"height\":50 }" );

    mMiniControl->addButton( "", std::bind(&UserInterface::toggleParams, this, std::placeholders::_1 ), "{ \"icon\": true, \"clear\":false, \"stateless\":false, \"pressed\":true }" );
    mMiniControl->addButton( "📷", std::bind(&UserInterface::saveImage, this, std::placeholders::_1 ), "{ \"icon\": true, \"clear\":false }" );
    mMiniControl->addButton( "", std::bind(&UserInterface::clear, this, std::placeholders::_1 ), "{ \"icon\": true, \"clear\":false }" );
    mMiniControl->addButton( "", std::bind(&UserInterface::showTutorial, this, 1, std::placeholders::_1 ), "{ \"icon\": true, \"clear\":false }" );
    mMiniControl->addButton( "", std::bind(&UserInterface::fullscreen, this, std::placeholders::_1 ), "{ \"icon\": true, \"stateless\":false, \"group\":\"fullscreen\" }" );
}

void UserInterface::setupDisconnected()
{
    mDisconnected = UIController::create( "{ \"visible\":false, \"centered\":true, \"panelColor\":\"0xE5FFFFFF\", \"width\":550, \"forceInteraction\":true, \"marginLarge\":25 }" );

    mDisconnected->addLabel( "Hi there.", "{ \"style\":\"header\", \"justification\":\"left\", \"width\":500, \"nameColor\":\"0xFF444444\" }" );
    
    mDisconnected->addSeparator();
    
    mDisconnected->addLabel( "Please make sure that your Leap Motion Controller is plugged in, and that the Leap Motion Controller software is running.", "{ \"style\":\"body\", \"justification\":\"left\", \"width\":500, \"nameColor\":\"0xFF444444\" }" );

    mDisconnected->addSeparator();
    
    mDisconnected->addButton( "Ignore", std::bind(&UserInterface::ignoreDisconnect, this, std::placeholders::_1 ), "{ \"width\":500, \"backgroundColor\":\"0xFFFFFFFF\" }" );
    
    mDisconnected->setHeight();
}

void UserInterface::setupWelcome()
{
    mWelcome = UIController::create( "{ \"visible\":true, \"centered\":true, \"panelColor\":\"0xE5FFFFFF\", \"width\":550, \"forceInteraction\":true, \"marginLarge\":25 }" );
    
    mWelcome->addLabel( "Beautiful Chaos", "{ \"style\":\"header\", \"justification\":\"left\", \"width\":500, \"nameColor\":\"0xFF444444\" }" );
    
    mWelcome->addSeparator();
    
    mWelcome->addLabel( "“When I am working on a problem, I never think about beauty. I think only of how to solve the problem. But when I have finished, if the solution is not beautiful, I know it is wrong.”", "{ \"style\":\"body\", \"justification\":\"left\", \"width\":500, \"nameColor\":\"0xFF444444\" }" );
    mWelcome->addLabel( "R. Buckminster Fuller", "{ \"style\":\"footer\", \"justification\":\"right\", \"width\":500, \"nameColor\":\"0xFF444444\" }" );
    
    mWelcome->setHeight();
}

void UserInterface::setupImageSaved()
{
    mImageSaved = UIController::create( "{ \"visible\":false, \"centered\":true, \"panelColor\":\"0xE5FFFFFF\", \"width\":550, \"forceInteraction\":true, \"marginLarge\":25 }" );
    
    mImageSaved->addLabel( "Image Saved", "{ \"style\":\"header\", \"justification\":\"left\", \"width\":500, \"nameColor\":\"0xFF444444\" }" );

    mImageSaved->addSeparator();
    
    mImageSaved->addLabel( "A screen shot has been saved in your Documents directory.", "{ \"style\":\"body\", \"justification\":\"left\", \"width\":500, \"nameColor\":\"0xFF444444\" }" );
    
    mImageSaved->setHeight();

    
    mImageNotSaved = UIController::create( "{ \"visible\":false, \"centered\":true, \"panelColor\":\"0xE5FFFFFF\", \"width\":550, \"forceInteraction\":true, \"marginLarge\":25 }" );
    
    mImageNotSaved->addLabel( "Error", "{ \"style\":\"header\", \"justification\":\"left\", \"width\":500, \"nameColor\":\"0xFF444444\" }" );
    
    mImageNotSaved->addSeparator();
    
    mImageNotSaved->addLabel( "The screen shot was not saved. Please visit beautifulchaosapp.com for support.", "{ \"style\":\"body\", \"justification\":\"left\", \"width\":500, \"nameColor\":\"0xFF444444\" }" );
    
    mImageNotSaved->setHeight();
}

void UserInterface::draw()
{
    // normal alpha blending
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    if (mVisible) {
        // TODO: all UIControllers should be in a vector and this should be a loop
        mParams->draw();
        mMiniControl->draw();
        mDisconnected->draw();
        mWelcome->draw();
        mImageSaved->draw();
        mImageNotSaved->draw();
        mCredits->draw();
        mTutorial1->draw();
        mTutorial2->draw();
        mTutorial3->draw();
        mTutorial4->draw();
    }

    // needed because of what the ping pong fbo is doing, at least
    gl::disableAlphaBlending();
}

void UserInterface::update()
{
    // TODO: all UIControllers should be in a vector and this should be a loop
    mParams->update();
    mMiniControl->update();
    mDisconnected->update();
    mWelcome->update();
    mImageSaved->update();
    mImageNotSaved->update();
    mCredits->update();
    mTutorial1->update();
    mTutorial2->update();
    mTutorial3->update();
    mTutorial4->update();
}

void UserInterface::resize()
{
    // TODO: all UIControllers should be in a vector and this should be a loop
    mParams->resize();
    mMiniControl->resize();
    mDisconnected->resize();
    mWelcome->resize();
    mImageSaved->resize();
    mImageNotSaved->resize();
    mCredits->resize();
    mTutorial1->resize();
    mTutorial2->resize();
    mTutorial3->resize();
    mTutorial4->resize();
}

void UserInterface::mouseDown( MouseEvent &event )
{
    // unpause on any input
    if ( mParameterBag->mPaused ) mParameterBag->mPaused = false;
}

void UserInterface::keyDown( KeyEvent &event )
{
    // unpause on any input except 'p'
    if ( mParameterBag->mPaused && event.getChar() != 'p' ) mParameterBag->mPaused = false;

    switch ( event.getChar() ) {

        // TODO: there's an argument to move these into the UIController/Element just because of the clamping issue
            
        // coefficients
        case '1':
            mParameterBag->mAB.x = math<float>::clamp( mParameterBag->mAB.x - 0.01, -4.0f, 4.0f );
            break;
        case '2':
            mParameterBag->mAB.x = math<float>::clamp( mParameterBag->mAB.x + 0.01, -4.0f, 4.0f );
            break;
        case '3':
            mParameterBag->mAB.y = math<float>::clamp( mParameterBag->mAB.y - 0.01, -4.0f, 4.0f );
            break;
        case '4':
            mParameterBag->mAB.y = math<float>::clamp( mParameterBag->mAB.y + 0.01, -4.0f, 4.0f );
            break;
        case '5':
            mParameterBag->mCD.x = math<float>::clamp( mParameterBag->mCD.x - 0.01, -4.0f, 4.0f );
            break;
        case '6':
            mParameterBag->mCD.x = math<float>::clamp( mParameterBag->mCD.x + 0.01, -4.0f, 4.0f );
            break;
        case '7':
            mParameterBag->mCD.y = math<float>::clamp( mParameterBag->mCD.y - 0.01, -4.0f, 4.0f );
            break;
        case '8':
            mParameterBag->mCD.y = math<float>::clamp( mParameterBag->mCD.y + 0.01, -4.0f, 4.0f );
            break;
        case '9':
            mParameterBag->mE = math<float>::clamp( mParameterBag->mE - 0.01, 0.1f, 2.0f );
            break;
        case '0':
            mParameterBag->mE = math<float>::clamp( mParameterBag->mE + 0.01, 0.1f, 2.0f );
            break;
        case '-':
            mParameterBag->mRandomness = math<float>::clamp( mParameterBag->mRandomness - 10.0, 1.0f, 1000.0f );
            break;
        case '+':
            mParameterBag->mRandomness = math<float>::clamp( mParameterBag->mRandomness + 10.0, 1.0f, 1000.0f );
            break;

        // fg alpha (opacity)
        case '[':
            mParameterBag->mFgAlpha = math<float>::clamp( mParameterBag->mFgAlpha - 0.01 );
            break;
        case ']':
            mParameterBag->mFgAlpha = math<float>::clamp( mParameterBag->mFgAlpha + 0.01 );
            break;

        // bg alpha (fade)
        case '{':
            mParameterBag->mBgAlpha = math<float>::clamp( mParameterBag->mBgAlpha - 0.01 );
            break;
        case '}':
            mParameterBag->mBgAlpha = math<float>::clamp( mParameterBag->mBgAlpha + 0.01 );
            break;
            
        // cam sensitivity
        case ',':
            mParameterBag->mSensitivity = math<float>::clamp( mParameterBag->mSensitivity - 0.01, 0.05f, 1.0f );
            break;
        case '.':
            mParameterBag->mSensitivity = math<float>::clamp( mParameterBag->mSensitivity + 0.01, 0.05f, 1.0f );
            break;
            
        // pause
        case 'p':
            mParameterBag->mPaused = !mParameterBag->mPaused;
            break;
            
        // save image
        case 's':
            saveImage( true );
            break;
            
        // toggle params & mouse
        case 'h':
            toggleVisibility();
            break;

        // fullscreen
        // TODO: this works, but doesn't update the button state
//        case 'f':
//            fullscreen( true );
//            break;
        
        // clear
        case '/':
            clear( true );
            break;
            
        // show tutorial
        case '?':
            showTutorial( 1, true );
            break;
    }

    switch ( event.getCode() ) {

        // next/prev color mode
        // TODO: this works, but doesn't update the buttons
//        case KeyEvent::KEY_LEFT:
//            mParameterBag->mColorModeManager->prev();
//            break;
//        case KeyEvent::KEY_RIGHT:
//            mParameterBag->mColorModeManager->next();
//            break;
    }
}

void UserInterface::show()
{
    mVisible = true;
    AppBasic::get()->showCursor();
}

void UserInterface::hide()
{
    mVisible = false;
    AppBasic::get()->hideCursor();
}

void UserInterface::hideWelcome()
{
    // This is called by the main app after the Scene has been set up, which can take different amounts of time on different systems.
    // If at least 7 seconds have passed, hide the welcome splash screen otherwise use the timeline to trigger the hide at a future time.
    // Either way, show the disconnected message if the Leap isn't connected after the welcome message is hidden.
    float seconds = 7.0f - getElapsedSeconds();
    if ( seconds < 0 ) {
        mWelcome->hide();
        if ( ! mDevice->isConnected() ) {
            mDisconnected->show();
        }
    } else {
        timeline().apply( &mTimer, 1.0f, seconds ).finishFn( [&]
        {
            mWelcome->hide();
            if ( ! mDevice->isConnected() ) {
                mDisconnected->show();
            }
        } );
    }
}

void UserInterface::fullscreen( const bool &pressed )
{
    mWindow->setFullScreen( pressed, mParameterBag->mFullScreenOptions );
    
    // other things besides clicking the fullscreen button might take us out of fullscreen
    // TODO: remove later when fullscreen button is properly tracking whether main window is fullscreen
    if ( ! pressed )
        mMiniControl->releaseGroup( "fullscreen" );
}

void UserInterface::showTutorial( const int &aNum, const bool &pressed )
{
    switch ( aNum ) {
        case 1:
            mTutorial1->show();
            break;
        case 2:
            mTutorial1->hide();
            mTutorial2->show();
            break;
        case 3:
            mTutorial2->hide();
            mTutorial3->show();
            break;
        case 4:
            mTutorial3->hide();
            mTutorial4->show();
            break;
    }
}

void UserInterface::hideTutorial( const bool &pressed )
{
    mTutorial1->hide();
    mTutorial2->hide();
    mTutorial3->hide();
    mTutorial4->hide();
}

void UserInterface::showCredits( const bool &pressed )
{
    mCredits->show();
}

void UserInterface::hideCredits( const bool &pressed )
{
    mCredits->hide();
}

void UserInterface::onLeapConnect()
{
    mIgnoringDisconnect = false;
	if ( mSetupComplete ) {
	    mDisconnected->hide();
	}
}

void UserInterface::onLeapDisconnect()
{
    if ( ! mIgnoringDisconnect ) {
		if ( mSetupComplete ) {
	        mDisconnected->show();
		}
    }
}

void UserInterface::saveImage( const bool &pressed )
{
    if ( mScene->save() ) {
        mImageSaved->show();
        timeline().apply( &mTimer, 1.0f, 2.0f ).finishFn( [&]{ mImageSaved->hide(); } );
    } else {
        mImageNotSaved->show();
        timeline().apply( &mTimer, 1.0f, 2.0f ).finishFn( [&]{ mImageNotSaved->hide(); } );
    }
}
