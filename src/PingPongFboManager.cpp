#include "PingPongFboManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MinimalUI;

PingPongFboManagerRef PingPongFboManager::create( ParameterBagRef aParameterBag )
{
    return shared_ptr<PingPongFboManager>( new PingPongFboManager( aParameterBag ) );
}

void PingPongFboManager::draw()
{
    mPPFbo.bindTexture(0);
    mPPFbo.bindTexture(1);
    mDisplacementShader.bind();
    mDisplacementShader.uniform("displacementMap", 0 );
    mDisplacementShader.uniform("velocityMap", 1);
    //        mDisplacementShader.uniform("COI", Vec4f(mCamera.getCenterOfInterestPoint(), 1.0));
    gl::draw( mVboMesh );
    mDisplacementShader.unbind();
    mPPFbo.unbindTexture();
}

void PingPongFboManager::setup()
{
    try {
        // Multiple render targets shader updates the positions/velocities
        mParticlesShader = gl::GlslProg( loadResource( PASSTHROUGH_VERT ), loadResource( PARTICLES_FRAG ));
        // Vertex displacement shader
        mDisplacementShader = gl::GlslProg( loadResource( VERTEXDISPLACEMENT_VERT ), loadResource( VERTEXDISPLACEMENT_FRAG ));
    }
    catch( gl::GlslProgCompileExc &exc ) {
        console() << "Shader compile error: " << endl;
        console() << exc.what() << endl;
        getWindow()->getApp()->quit();
    }
    catch( const std::exception &e ) {
        console() << "Unable to load shader: " << endl;
        console() << e.what() << endl;
        getWindow()->getApp()->quit();
    }
    gl::clear();
    setupPingPongFbo();
    // THE VBO HAS TO BE DRAWN AFTER FBO!
    setupVBO();    
}

void PingPongFboManager::update()
{    
    gl::setMatricesWindow( mPPFbo.getSize(), false ); // false to prevent vertical flipping
    gl::setViewport( mPPFbo.getBounds() );
    
    mPPFbo.updateBind();
    
    mParticlesShader.bind();
    mParticlesShader.uniform( "positions", 0 );
    mParticlesShader.uniform( "velocities", 1 );
    mParticlesShader.uniform( "attractorPos", Vec3f::zero());
    mParticlesShader.uniform( "ab", mParameterBag->mAB );
    mParticlesShader.uniform( "cd", mParameterBag->mCD );
    mParticlesShader.uniform( "e", mParameterBag->mE );
    mParticlesShader.uniform( "alpha", mParameterBag->mFgAlpha );
    mParticlesShader.uniform( "randomness", mParameterBag->mRandomness );
    mParticlesShader.uniform( "suppliedColor", mParameterBag->mColorModeManager->getForegroundColor() );
    mParticlesShader.uniform( "suppliedColorFactor", mParameterBag->mColorModeManager->getSuppliedColorFactor() );
    mParticlesShader.uniform( "rainbowColorFactor", mParameterBag->mColorModeManager->getRainbowColorFactor() );
    mParticlesShader.uniform( "colorAddFactor", mParameterBag->mColorModeManager->getColorAddFactor() );
    
    //instead of gl::drawSolidRect(mPPFbo.getBounds());
    gl::draw( mVboMesh2 );
    
    mParticlesShader.unbind();
    
    mPPFbo.updateUnbind();
    mPPFbo.swap();
}

void PingPongFboManager::setupPingPongFbo()
{
    float scale = 8.0f;
    // TODO: Test with more than 2 texture attachments
    Surface32f surfaces[2];
    // Position 2D texture array
    surfaces[0] = Surface32f( SIDE, SIDE, true);
    Surface32f::Iter pixelIter = surfaces[0].getIter();
    while( pixelIter.line() ) {
        while( pixelIter.pixel() ) {
            /* Initial particle positions are passed in as R,G,B
             float values. Alpha is used as particle jitter. */
            surfaces[0].setPixel(pixelIter.getPos(),
                                 ColorAf(scale*(Rand::randFloat()-0.5f),
                                         scale*(Rand::randFloat()-0.5f),
                                         scale*(Rand::randFloat()-0.5f),
                                         Rand::randFloat(-mParameterBag->mJitter, mParameterBag->mJitter) ) );
        }
    }
    
    //Velocity 2D texture array
    surfaces[1] = Surface32f( SIDE, SIDE, true);
    pixelIter = surfaces[1].getIter();
    while( pixelIter.line() ) {
        while( pixelIter.pixel() ) {
            /* Initial particle velocities are
             passed in as R,G,B float values. */
            surfaces[1].setPixel( pixelIter.getPos(), Color::black() );
        }
    }
    mPPFbo = PingPongFbo(surfaces);
}

void PingPongFboManager::setupVBO()
{
    /*  A dummy VboMesh the same size as the
     texture to keep the vertices on the GPU */
    int totalVertices = SIDE * SIDE;
    vector<Vec2f> texCoords;
    vector<uint32_t> indices;
    vector<Vec3f> positions;
    gl::VboMesh::Layout layout;
    layout.setStaticIndices();
    layout.setStaticPositions();
    layout.setStaticTexCoords2d();
    //        layout.setStaticNormals();
    //layout.setDynamicColorsRGBA();
    glPointSize(1.0f);
    mVboMesh = gl::VboMesh( totalVertices, totalVertices, layout, GL_POINTS);
    for( int x = 0; x < SIDE; ++x ) {
        for( int y = 0; y < SIDE; ++y ) {
            indices.push_back( x * SIDE + y );
            positions.push_back( Vec3f( x, y, 0.0f ) );
            texCoords.push_back( Vec2f( x/(float)SIDE, y/(float)SIDE ) );
        }
    }
    mVboMesh.bufferIndices( indices );
    mVboMesh.bufferPositions( positions );
    mVboMesh.bufferTexCoords2d( 0, texCoords );
    mVboMesh.unbindBuffers();
    // or glFinish(); ?

    // another dummy VboMesh for the rectangle to activate the shaders
    // layout is the same (all static)
    vector<Vec2f> texCoords2;
    vector<uint32_t> indices2;
    vector<Vec3f> positions2;
    mVboMesh2 = gl::VboMesh( 4, 4, layout, GL_QUADS );
    indices2.push_back( 0 );
    indices2.push_back( 1 );
    indices2.push_back( 2 );
    indices2.push_back( 3 );
    positions2.push_back( Vec3f( 0.0f, 0.0f, 0.0f ) );
    positions2.push_back( Vec3f( mPPFbo.getBounds().getWidth(), 0.0f, 0.0f ) );
    positions2.push_back( Vec3f( mPPFbo.getBounds().getWidth(), mPPFbo.getBounds().getHeight(), 0.0f ) );
    positions2.push_back( Vec3f( 0.0f, mPPFbo.getBounds().getHeight(), 0.0f ) );
    texCoords2.push_back( Vec2f( 0.0f, 0.0f ) );
    texCoords2.push_back( Vec2f( 1.0f, 0.0f ) );
    texCoords2.push_back( Vec2f( 1.0f, 1.0f ) );
    texCoords2.push_back( Vec2f( 0.0f, 1.0f ) );

    mVboMesh2.bufferIndices( indices2 );
    mVboMesh2.bufferPositions( positions2 );
    mVboMesh2.bufferTexCoords2d( 0, texCoords2 );
    mVboMesh2.unbindBuffers();
}

