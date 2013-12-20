//
//  SwapTexture.h
//  gpuPS
//
//  Created by Éric Renaud-Houde on 2013-01-06.
//
//

#pragma once
#include "cinder/app/AppNative.h"
#include "cinder/Vector.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rect.h"
#include "cinder/Utilities.h"
#include <iostream>
#include <fstream>

using namespace ci;
using namespace app;

namespace MinimalUI {
    
class PingPongFbo
{
public:
    PingPongFbo() {};
    
    /**
     * Create a ping-pong fbo with n texture attachments.
     */
    template <std::size_t n>
    PingPongFbo(const Surface32f (&surface)[n])
    : mCurrentFbo(0)
    {
        if(n == 0) return;
        
        mTextureSize = surface[0].getSize();
        for(int i=0; i<n; i++) {
            mAttachments.push_back(GL_COLOR_ATTACHMENT0_EXT + i);
            addTexture(surface[i]);
        }
        
        // this is where the app is most likely to get fubar
        std::ofstream debugLog;
        std::string filename = "BeautifulChaos_" + boost::lexical_cast<std::string>(time(0)) + ".log";
        fs::path path = getDocumentsDirectory() / filename;
        
        int max = gl::Fbo::getMaxAttachments();
        int MaxVertexTextureImageUnits;
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &MaxVertexTextureImageUnits);
        int MaxCombinedTextureImageUnits;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxCombinedTextureImageUnits);
        if (n > max) {
            console() << "Maximum supported number of texture attachments: " << max << " (two required for this app). Exiting now." << std::endl;
            debugLog.open( path.string() );
            debugLog << "Maximum supported number of texture attachments: " << max << " (two required for this app). Exiting now.";
            debugLog.close();
            getWindow()->getApp()->quit();
        }
        if ( MaxVertexTextureImageUnits < 4 ) {
            console() << "Maximum supported number of vertex texture image units: " << MaxVertexTextureImageUnits << " (4 required for this app). Exiting now." << std::endl;
            debugLog.open( path.string() );
            debugLog << "Maximum supported number of vertex texture image units: " << MaxVertexTextureImageUnits << " (4 required for this app). Exiting now.";
            debugLog.close();
            getWindow()->getApp()->quit();
        }
        if ( MaxCombinedTextureImageUnits < 4 ) {
            console() << "Maximum supported number of combined texture image units: " << MaxVertexTextureImageUnits << " (4 required for this app). Exiting now." << std::endl;
            debugLog.open( path.string() );
            debugLog << "Maximum supported number of combined texture image units: " << MaxVertexTextureImageUnits << " (4 required for this app). Exiting now.";
            debugLog.close();
            getWindow()->getApp()->quit();
        }
        if (!gl::isExtensionAvailable("GL_EXT_framebuffer_object")) {
            console() << "GL_EXT_framebuffer_object is required for this app. Exiting now." << std::endl;
            debugLog.open( path.string() );
            debugLog << "GL_EXT_framebuffer_object is required for this app. Exiting now.";
            debugLog.close();
            getWindow()->getApp()->quit();
        }
        if(!gl::isExtensionAvailable("GL_ARB_texture_float")) {
            console() << "GL_ARB_texture_float is required for this app. Exiting now." << std::endl;
            debugLog.open( path.string() );
            debugLog << "GL_ARB_texture_float is required for this app. Exiting now.";
            debugLog.close();
            getWindow()->getApp()->quit();
        }
        
        gl::Fbo::Format format;
        format.enableDepthBuffer(false);
        format.enableColorBuffer(true, mAttachments.size());
        format.setMinFilter( GL_NEAREST );
        format.setMagFilter( GL_NEAREST );
        format.setColorInternalFormat( GL_RGBA32F_ARB );
        mFbos[0] = gl::Fbo( mTextureSize.x, mTextureSize.y, format );
        mFbos[1] = gl::Fbo( mTextureSize.x, mTextureSize.y, format );
        
        reloadTextures();
    }
    
    /// Render initial textures into both fbos.
    void reloadTextures();
    ///Swap the two alternating fbos.
    void swap();
    /// Bind one fbo as the source, and the other as a target texture to update the texture.
    void updateBind();
    /// Unbind both the source and the target texture.
    void updateUnbind();
    /**
     * Bind the texture of the current fbo.
     * @param i Texture unit.
     * @param i Texture attachment.
     */
    void bindTexture(int textureUnit);
    /// Unbind the texture of the current fbo.
    void unbindTexture();
    /// Get the fbo/texture size.
    Vec2i getSize() const;
    /// Get the fbo/texture size.
    Area getBounds() const;
    gl::Fbo getPingFbo() const { return mFbos[0]; }
    gl::Fbo getPongFbo() const { return mFbos[1]; }
private:
	Vec2i mTextureSize;
    int mCurrentFbo; //either 0 or 1
    
    /** Texture attachments per framebuffer. */
    std::vector<gl::Texture> mTextures;
    /** GLenum texure attachments */
    std::vector<GLenum> mAttachments;
    /** Two alternating framebuffers */
    gl::Fbo	mFbos[2];
    /**
     * Add texture attachements to the ping-pong fbo.
     * @param surface Surface32f internally copied into a texture.
     */
    void addTexture(const Surface32f &surface);
};

}