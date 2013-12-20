//
//  SharedVariableManager.h
//  CinderLeapTest7
//
//  Created by Nathan Selikoff on 4/19/13.
//
//

#pragma once

#include "cinder/gl/gl.h"
#include "ColorMode.h"

namespace MinimalUI {
    
    typedef std::shared_ptr<class ParameterBag> ParameterBagRef;

    class ParameterBag
    {
    public:
        ParameterBag()
        {
            mAB = Vec2f( -1.15411f, -0.79603f );
            mCD = Vec2f( -0.670901f, -1.16923f );
            mE = 1.72107f;
            mJitter = 0.001f;
            mRandomness = 914.973f;
            mSensitivity = 0.5f;
            mSmoothing = 0.1f;

            mFgAlpha = 0.5f;
            mBgAlpha = 0.4f;
            mExposure = 1.0f;
            mGamma = 0.7f;
            
            mPaused = false;
            mActive = true;
            mLockRandomness = true;
            mLockSpread = true;
            mLockCamera = true;
            
            mCameraUp = mCameraDown = mCameraLeft = mCameraRight = mCameraIn = mCameraOut = mCameraClockwise = mCameraCounterclockwise = false;
            
            mState = STATE_DEFAULT;
            
            mEye = Vec3f( 27.9325f, 171.056f, 866.278f );
            mTarget = Vec3f( -44.2462f, -166.521f, -0.0435257f );
            
            mFboResolution = 2048;
            
            mFullScreenOptions.exclusive( true );
        }
        
        static ParameterBagRef create()
        {
            return shared_ptr<ParameterBag>( new ParameterBag() );
        }
        
        ci::Vec2f mAB, mCD;
        
        float mE;
        float mJitter;
        float mFgAlpha;
        float mRandomness;
        float mSensitivity;
        float mSmoothing;
        float mBgAlpha;
        float mExposure;
        float mGamma;

        ci::Vec3f mEye, mTarget;
        
        bool mPaused;
        bool mActive;
        bool mLockRandomness;
        bool mLockSpread;
        bool mLockCamera;
        
        bool mCameraUp, mCameraDown, mCameraLeft, mCameraRight, mCameraIn, mCameraOut, mCameraClockwise, mCameraCounterclockwise;
        
        std::vector<Leap::Hand> mHands;
        
        int mState;
        int mFboResolution;
        
        ci::app::FullScreenOptions mFullScreenOptions;

        enum { STATE_DEFAULT, STATE_EDIT_CAMERA, STATE_EDIT_ATTRACTOR, STATE_DISCONNECTED };

        ColorModeManager *mColorModeManager;
        
    };

}
