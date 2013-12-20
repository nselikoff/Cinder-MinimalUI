/*
 * UI
 */

#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"

#include "Resources.h"
#include "ParameterBag.h"
#include "UIController.h"

namespace MinimalUI {

    typedef std::shared_ptr<class UserInterface> UserInterfaceRef;

    class UserInterface {

    public:
        UserInterface( ParameterBagRef aParameterBag, ci::app::WindowRef aWindow, LeapMotion::SignalDeviceRef aDevice, SceneRef aScene );
        static UserInterfaceRef create( ParameterBagRef aParameterBag, ci::app::WindowRef aWindow, LeapMotion::SignalDeviceRef aDevice, SceneRef aScene );

        void setup();
        void draw();
        void update();
        void resize();
        void hideWelcome();
        void fullscreen( const bool &pressed );
        
    private:
        void setupParams();
        void setupMiniControl();
        void setupDisconnected();
        void setupWelcome();
        void setupImageSaved();
        void setupCredits();
        void setupTutorial();
        
        void show();
        void hide();
        void toggleVisibility() { mVisible ? hide() : show(); }
        
        void mouseDown( ci::app::MouseEvent &event );
        void keyDown( ci::app::KeyEvent &event );
        void onLeapConnect();
        void onLeapDisconnect();
        
        void showTutorial( const int &aNum, const bool &pressed );
        void hideTutorial( const bool &pressed );
        void showCredits( const bool &pressed );
        void hideCredits( const bool &pressed );
        
        void saveImage( const bool &pressed );
        
        void lockRandomness( const bool &pressed ) { mParameterBag->mLockRandomness = pressed; }
        void lockSpread( const bool &pressed ) { mParameterBag->mLockSpread = pressed; }
        void lockCamera( const bool &pressed ) { mParameterBag->mLockCamera = pressed; }
        
        void setLook( const int &aLook, const bool &pressed ) { mScene->setLook( aLook ); }
        void setColorMode( const int &aColorMode, const bool &pressed ) { if ( pressed ) mParameterBag->mColorModeManager->setColorMode( aColorMode ); }
        void toggleParams( const bool &pressed ) { pressed ? mParams->show() : mParams->hide(); }
        void clear( const bool &pressed ) { mScene->clear(); }
        void pause( const bool &pressed ) { mParameterBag->mPaused = pressed; }
        void ignoreDisconnect( const bool &pressed ) { mIgnoringDisconnect = true; mDisconnected->hide(); }
        
        void gotoURL( const string &aURL, const bool &pressed ) { launchWebBrowser( Url( aURL ) ); }
        
        void rotateCamera( const string &aDirection, const bool &pressed ) { mScene->rotateCamera( aDirection ); }
        
        UIControllerRef mParams, mMiniControl, mDisconnected, mWelcome, mImageSaved, mImageNotSaved, mCredits, mTutorial1, mTutorial2, mTutorial3, mTutorial4;
        
        ci::app::WindowRef mWindow;
        LeapMotion::SignalDeviceRef mDevice;
        SceneRef mScene;
        ci::signals::scoped_connection mCbMouseDown, mCbKeyDown, mCbLeapConnect, mCbLeapDisconnect;
        
        bool mVisible;
        bool mIgnoringDisconnect;
		bool mSetupComplete;
        
        ParameterBagRef mParameterBag;

        Anim<float> mTimer;
    };
}