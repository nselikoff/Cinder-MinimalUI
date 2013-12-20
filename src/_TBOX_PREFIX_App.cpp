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

#include "Resources.h"

#include "cinder/app/AppNative.h"

#include "ParameterBag.h"
#include "UserInterface.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;


class _TBOX_PREFIX_App : public AppNative {
 public:
	void prepareSettings(Settings* settings);
	void setup();
	void update();
	void draw();
private:	
    UserInterfaceRef mUserInterface;
    ParameterBagRef mParameterBag;

};

void _TBOX_PREFIX_App::prepareSettings(Settings* settings){
	settings->setFrameRate(60.0f);
	settings->setWindowSize(640, 480);
}

void _TBOX_PREFIX_App::setup()
{
	// Setup the user interface
    mUserInterface = UserInterface::create( mParameterBag, getWindow(), mLeapWrapper->getDevice(), mScene );
    mUserInterface->setup();
}

void _TBOX_PREFIX_App::update()
{
	mUserInterface->update();
}

void _TBOX_PREFIX_App::draw()
{
	mUserInterface->draw();
}

// This line tells Cinder to actually create the application
CINDER_APP_NATIVE( _TBOX_PREFIX_App, RendererGl )
