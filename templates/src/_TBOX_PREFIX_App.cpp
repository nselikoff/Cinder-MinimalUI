/*
The MIT License (MIT)

 Copyright (c) 2014, Nathan Selikoff - Bruce Lane - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 This file is part of Cinder-MinimalUI.

 Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "cinder/app/AppNative.h"
#include "cinder/Camera.h"

#include "UIController.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class _TBOX_PREFIX_App : public AppNative {
public:
	void setup();
	void update();
	void draw();
    void buttonCallback( const bool &pressed );
    void setCount( const int &aCount, const bool &pressed ) { mCount = aCount; }
    void lockZ( const bool &pressed ) { mLockZ = pressed; }
    
private:
    MinimalUI::UIControllerRef mParams;
    
    float mZoom;
    Vec2f mXYSize;
    int mCount;
    float mZPosition;
    bool mLockZ;
    
    CameraPersp mCamera;
};

void _TBOX_PREFIX_App::setup()
{
    mZoom = 1.0f;
    mXYSize = Vec2f::one();
    mCount = 1;
    mZPosition = 0.0f;
    mLockZ = false;
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    mCamera = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f, 1.0f, 1000.0f );
	mCamera.lookAt( Vec3f( -2, 2, 2 ), Vec3f::zero() );
    
    mParams = MinimalUI::UIController::create();
    
    // Slider
    mParams->addSlider( "Zoom", &mZoom );
    
    // 2D Sliders
    mParams->addSlider2D( "XY", &mXYSize, "{ \"minX\":-2.0, \"maxX\":2.0, \"minY\":-2.0, \"maxY\":2.0 }" );
    
    // Simple Button
    mParams->addButton( "Stateless!", std::bind( &_TBOX_PREFIX_App::buttonCallback, this, std::placeholders::_1 ), "{ \"width\":96, \"clear\":false }" );
    mParams->addButton( "Stateful!", std::bind( &_TBOX_PREFIX_App::buttonCallback, this, std::placeholders::_1 ), "{ \"width\":96, \"stateless\":false }" );
    
    // Separator
    mParams->addSeparator();
    
    // Label
    mParams->addLabel( "Count", "{ \"clear\":false }" );
    
    // Button Group
    mParams->addButton( "1", std::bind( &_TBOX_PREFIX_App::setCount, this, 1, std::placeholders::_1 ), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true, \"pressed\":true }" );
    mParams->addButton( "2", std::bind( &_TBOX_PREFIX_App::setCount, this, 2, std::placeholders::_1 ), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true }" );
    mParams->addButton( "3", std::bind( &_TBOX_PREFIX_App::setCount, this, 3, std::placeholders::_1 ), "{ \"stateless\":false, \"group\":\"count\", \"exclusive\":true }" );
    
    // Toggle Slider
    mParams->addToggleSlider( "Z Position", &mZPosition, "A", std::bind(&_TBOX_PREFIX_App::lockZ, this, std::placeholders::_1 ), "{ \"width\":156, \"clear\":false, \"min\": -1, \"max\": 1 }", "{ \"stateless\":false }" );
}

void _TBOX_PREFIX_App::update()
{
    mZPosition = mLockZ ? sin( getElapsedFrames() / 100.0f ) : mZPosition;
    
    mParams->update();
}

void _TBOX_PREFIX_App::draw()
{
    gl::clear();
    
    gl::setMatrices( mCamera );
    
    gl::pushModelView();
    gl::scale( Vec3f::one() * mZoom );
    gl::color( Color::white() );
    for ( int i = 0; i < mCount; i++ )
    {
        gl::pushModelView();
        gl::translate( i * 1.5f, 0.0f, mZPosition );
        gl::drawColorCube( Vec3f::zero(), Vec3f( mXYSize, 1.0f ) );
        gl::popModelView();
    }
    gl::popModelView();
    
    mParams->draw();
}

void _TBOX_PREFIX_App::buttonCallback( const bool &pressed )
{
    console() << "Hello! Button state: " << pressed << endl;
}

// This line tells Cinder to actually create the application
CINDER_APP_NATIVE( _TBOX_PREFIX_App, RendererGl )
