#ifndef __FSR1D3D11_H__
#define __FSR1D3D11_H__

#include <SDL3/SDL.h>

#include <memory>

namespace Sexy
{
    // Compute-only AMD FidelityFX FSR 1.0 backend for SDL's D3D11 renderer.
    // The returned SDL_Texture is owned by this object. A subsequent Upscale()
    // may replace it when size, colorspace, or the RCAS pass changes; it is
    // always invalidated by InvalidateOutput() and Shutdown().
    class Fsr1D3D11Backend final
    {
    public:
        Fsr1D3D11Backend();
        ~Fsr1D3D11Backend();

        Fsr1D3D11Backend(const Fsr1D3D11Backend&) = delete;
        Fsr1D3D11Backend& operator=(const Fsr1D3D11Backend&) = delete;

        // Initialization succeeds only for SDL's direct3d11 renderer. Failure
        // is non-fatal: callers should present the original source texture.
        bool Initialize(SDL_Renderer* theRenderer);

        // Runs EASU and, when theRcasSharpness is greater than zero, RCAS.
        // Sharpness is an intuitive linear amount in [0, 1], where 1 is the
        // strongest RCAS setting. Returns nullptr on any unsupported/failure
        // path so the caller can transparently fall back to SDL scaling.
        SDL_Texture* Upscale(
            SDL_Texture* theSource,
            int theSourceWidth,
            int theSourceHeight,
            int theOutputWidth,
            int theOutputHeight,
            float theRcasSharpness);

        // Drops size-dependent resources while keeping the device and shaders.
        void InvalidateOutput();

        // Must be called before the associated SDL_Renderer is destroyed.
        void Shutdown();

        bool IsAvailable() const;
        bool HasValidOutput() const;
        SDL_Texture* GetLastOutput() const;
        int GetOutputWidth() const;
        int GetOutputHeight() const;
        const char* GetLastError() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> mImpl;
    };
}

#endif
