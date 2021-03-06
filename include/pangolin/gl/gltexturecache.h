/* This file is part of the Pangolin Project.
 * http://github.com/stevenlovegrove/Pangolin
 *
 * Copyright (c) 2014 Steven Lovegrove
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef PANGOLIN_GLTEXTURECACHE_H
#define PANGOLIN_GLTEXTURECACHE_H

#include <pangolin/gl.h>
#include <pangolin/gl/glformattraits.h>
#include <pangolin/compat/memory.h>
#include <pangolin/image.h>

namespace pangolin
{

class PANGOLIN_EXPORT TextureCache
{
public:
    static TextureCache& I() {
        static TextureCache instance;
        return instance;
    }

    template<typename T>
    GlTexture& GlTex(int w, int h)
    {
        // Lookup texture
        boostd::shared_ptr<GlTexture>& ptex = texture_map[typeid(T).name()];
        if(!ptex) {
            ptex = boostd::shared_ptr<GlTexture>(new GlTexture());
        }
        GlTexture& tex = *ptex;

        // Initialise if it didn't already exist or the size was too small
        if(!tex.tid || tex.width < w || tex.height < h) {
            tex.Reinitialise(
                std::max(tex.width,w), std::max(tex.height,h),
                GlFormatTraits<T>::glinternalformat, default_sampling_linear, 0,
                GlFormatTraits<T>::glformat,
                GlFormatTraits<T>::gltype
            );
        }

        return tex;
    }

protected:
    bool default_sampling_linear;
    std::map<const char*, boostd::shared_ptr<GlTexture> > texture_map;

    // Protected constructor
    TextureCache()
        : default_sampling_linear(true)
    {
    }
};

template<typename T>
void RenderToViewport(Image<T>& image, bool flipx=false, bool flipy=false)
{
    // Retrieve texture that is at least as large as image and of appropriate type.
    GlTexture& tex = TextureCache::I().GlTex<T>(image.w, image.h);
    tex.Upload(image.ptr,0,0, image.w, image.h, GlFormatTraits<T>::glformat, GlFormatTraits<T>::gltype);
    tex.RenderToViewport(Viewport(0,0,image.w, image.h), flipx, flipy);
}

}

#endif // PANGOLIN_GLTEXTURECACHE_H
