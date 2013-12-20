//
//  PingPongFboManager.h
//  CinderLeapTest7
//
//  Created by Nathan Selikoff on 4/19/13.
//
//

#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"

#include "PingPongFbo.h"
#include "ParameterBag.h"
#include "Resources.h"

namespace MinimalUI {

    const int SIDE = 1024;

    typedef std::shared_ptr<class PingPongFboManager> PingPongFboManagerRef;

    class PingPongFboManager
    {
    public:
        PingPongFboManager(ParameterBagRef aParameterBag) : mParameterBag(aParameterBag) { }
        static PingPongFboManagerRef create( ParameterBagRef aParameterBag );
        void draw();
        void setup();
        void update();
            
    private:
        void setupPingPongFbo();
        void setupVBO();
        
        ParameterBagRef mParameterBag;
        
        PingPongFbo mPPFbo;
        ci::gl::VboMesh mVboMesh, mVboMesh2;
        ci::gl::GlslProg mParticlesShader, mDisplacementShader;
        
    };

}