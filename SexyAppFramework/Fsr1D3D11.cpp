#include "Fsr1D3D11.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

#include <algorithm>
#include <array>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

#define A_CPU 1
#include "../third_party/FidelityFX-FSR/ffx_a.h"
#include "../third_party/FidelityFX-FSR/ffx_fsr1.h"
#undef A_CPU

#include "shaders/Fsr1Shaders.generated.h"

using Microsoft::WRL::ComPtr;

namespace Sexy
{
namespace
{
    struct alignas(16) FsrConstants
    {
        AU1 mConst0[4]{};
        AU1 mConst1[4]{};
        AU1 mConst2[4]{};
        AU1 mConst3[4]{};
        AU1 mSample[4]{};
    };

    static_assert(sizeof(FsrConstants) == 80, "FSR constant buffer layout must match HLSL.");

    struct OutputResource
    {
        ComPtr<ID3D11Texture2D> mTexture;
        ComPtr<ID3D11ShaderResourceView> mShaderView;
        ComPtr<ID3D11UnorderedAccessView> mUnorderedView;
        SDL_Texture* mSDLTexture = nullptr;

        void Reset()
        {
            // SDL owns an additional reference to the wrapped D3D texture.
            // Drop that reference before releasing our views and resource.
            if (mSDLTexture != nullptr)
            {
                SDL_DestroyTexture(mSDLTexture);
                mSDLTexture = nullptr;
            }
            mUnorderedView.Reset();
            mShaderView.Reset();
            mTexture.Reset();
        }
    };

    const char* FormatName(DXGI_FORMAT theFormat)
    {
        switch (theFormat)
        {
        case DXGI_FORMAT_R8G8B8A8_TYPELESS: return "R8G8B8A8_TYPELESS";
        case DXGI_FORMAT_R8G8B8A8_UNORM: return "R8G8B8A8_UNORM";
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return "R8G8B8A8_UNORM_SRGB";
        case DXGI_FORMAT_B8G8R8A8_TYPELESS: return "B8G8R8A8_TYPELESS";
        case DXGI_FORMAT_B8G8R8A8_UNORM: return "B8G8R8A8_UNORM";
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return "B8G8R8A8_UNORM_SRGB";
        case DXGI_FORMAT_B8G8R8X8_TYPELESS: return "B8G8R8X8_TYPELESS";
        case DXGI_FORMAT_B8G8R8X8_UNORM: return "B8G8R8X8_UNORM";
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return "B8G8R8X8_UNORM_SRGB";
        case DXGI_FORMAT_R16G16B16A16_FLOAT: return "R16G16B16A16_FLOAT";
        default: return "unsupported";
        }
    }

    // Select an explicit view instead of guessing from SDL's colorspace. This
    // makes the shader's transfer-function decision follow the view D3D11 is
    // actually sampling. Typeless 8-bit targets are read as UNORM (perceptual
    // bytes); typed *_SRGB resources necessarily decode to linear samples.
    DXGI_FORMAT ResolveSourceViewFormat(DXGI_FORMAT theResourceFormat, bool& theSamplesAreLinear)
    {
        theSamplesAreLinear = false;
        switch (theResourceFormat)
        {
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            theSamplesAreLinear = true;
            return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case DXGI_FORMAT_B8G8R8A8_TYPELESS:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            return DXGI_FORMAT_B8G8R8A8_UNORM;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            theSamplesAreLinear = true;
            return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
        case DXGI_FORMAT_B8G8R8X8_TYPELESS:
        case DXGI_FORMAT_B8G8R8X8_UNORM:
            return DXGI_FORMAT_B8G8R8X8_UNORM;
        case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
            theSamplesAreLinear = true;
            return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            theSamplesAreLinear = true;
            return DXGI_FORMAT_R16G16B16A16_FLOAT;
        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    bool IsValidDimension(int theDimension)
    {
        return theDimension > 0 && theDimension <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
    }
}

struct Fsr1D3D11Backend::Impl
{
    SDL_Renderer* mRenderer = nullptr;
    ComPtr<ID3D11Device> mDevice;
    ComPtr<ID3D11DeviceContext> mContext;
    ComPtr<ID3D11ComputeShader> mEasuShader;
    ComPtr<ID3D11ComputeShader> mRcasShader;
    ComPtr<ID3D11Buffer> mConstantBuffer;
    ComPtr<ID3D11SamplerState> mLinearClampSampler;

    ComPtr<ID3D11Texture2D> mSourceTexture;
    ComPtr<ID3D11ShaderResourceView> mSourceView;
    DXGI_FORMAT mSourceViewFormat = DXGI_FORMAT_UNKNOWN;
    bool mSourceSamplesAreLinear = false;

    OutputResource mIntermediate;
    OutputResource mOutput;
    OutputResource mPresentation;
    int mOutputWidth = 0;
    int mOutputHeight = 0;
    SDL_Colorspace mOutputColorspace = SDL_COLORSPACE_UNKNOWN;
    SDL_Colorspace mRendererOutputColorspace = SDL_COLORSPACE_UNKNOWN;
    bool mAvailable = false;
    bool mOutputValid = false;
    std::string mLastError;

    bool Fail(const char* theOperation)
    {
        mLastError = theOperation;
        mOutputValid = false;
        return false;
    }

    bool Fail(const char* theOperation, HRESULT theResult)
    {
        char aBuffer[192];
        std::snprintf(
            aBuffer,
            sizeof(aBuffer),
            "%s failed (HRESULT 0x%08lX)",
            theOperation,
            static_cast<unsigned long>(theResult));
        return Fail(aBuffer);
    }

    void InvalidateSource()
    {
        mSourceView.Reset();
        mSourceTexture.Reset();
        mSourceViewFormat = DXGI_FORMAT_UNKNOWN;
        mSourceSamplesAreLinear = false;
    }

    void InvalidateOutput()
    {
        mOutputValid = false;
        mPresentation.Reset();
        mOutput.Reset();
        mIntermediate.Reset();
        mOutputWidth = 0;
        mOutputHeight = 0;
        mOutputColorspace = SDL_COLORSPACE_UNKNOWN;
        mRendererOutputColorspace = SDL_COLORSPACE_UNKNOWN;
    }

    void Shutdown()
    {
        InvalidateOutput();
        InvalidateSource();
        mLinearClampSampler.Reset();
        mConstantBuffer.Reset();
        mRcasShader.Reset();
        mEasuShader.Reset();
        mContext.Reset();
        mDevice.Reset();
        mRenderer = nullptr;
        mAvailable = false;
    }

    bool Initialize(SDL_Renderer* theRenderer)
    {
        Shutdown();
        mLastError.clear();

        if (theRenderer == nullptr)
            return Fail("FSR initialization received a null SDL renderer");

        const char* aRendererName = SDL_GetRendererName(theRenderer);
        if (aRendererName == nullptr || SDL_strcasecmp(aRendererName, "direct3d11") != 0)
            return Fail("FSR 1 requires SDL's direct3d11 renderer");

        const SDL_PropertiesID aRendererProperties = SDL_GetRendererProperties(theRenderer);
        if (aRendererProperties == 0)
            return Fail("SDL_GetRendererProperties");

        ID3D11Device* aNativeDevice = static_cast<ID3D11Device*>(SDL_GetPointerProperty(
            aRendererProperties,
            SDL_PROP_RENDERER_D3D11_DEVICE_POINTER,
            nullptr));
        if (aNativeDevice == nullptr)
            return Fail("SDL did not expose its D3D11 device");

        mDevice = aNativeDevice;
        mDevice->GetImmediateContext(&mContext);
        if (mContext == nullptr)
            return Fail("ID3D11Device::GetImmediateContext");

        UINT aFormatSupport = 0;
        HRESULT aResult = mDevice->CheckFormatSupport(DXGI_FORMAT_R8G8B8A8_UNORM, &aFormatSupport);
        if (FAILED(aResult))
            return Fail("ID3D11Device::CheckFormatSupport", aResult);
        const UINT aRequiredFormatSupport =
            D3D11_FORMAT_SUPPORT_TEXTURE2D |
            D3D11_FORMAT_SUPPORT_SHADER_SAMPLE |
            D3D11_FORMAT_SUPPORT_TYPED_UNORDERED_ACCESS_VIEW |
            D3D11_FORMAT_SUPPORT_RENDER_TARGET;
        if ((aFormatSupport & aRequiredFormatSupport) != aRequiredFormatSupport)
            return Fail("The D3D11 device lacks R8G8B8A8 UAV/render-target support");

        aResult = mDevice->CreateComputeShader(
            Fsr1Shaders::gEasuCS,
            Fsr1Shaders::gEasuCSSize,
            nullptr,
            &mEasuShader);
        if (FAILED(aResult))
            return Fail("CreateComputeShader(EASU)", aResult);

        aResult = mDevice->CreateComputeShader(
            Fsr1Shaders::gRcasCS,
            Fsr1Shaders::gRcasCSSize,
            nullptr,
            &mRcasShader);
        if (FAILED(aResult))
            return Fail("CreateComputeShader(RCAS)", aResult);

        D3D11_BUFFER_DESC aBufferDescription{};
        aBufferDescription.ByteWidth = sizeof(FsrConstants);
        aBufferDescription.Usage = D3D11_USAGE_DEFAULT;
        aBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        aResult = mDevice->CreateBuffer(&aBufferDescription, nullptr, &mConstantBuffer);
        if (FAILED(aResult))
            return Fail("CreateBuffer(FSR constants)", aResult);

        D3D11_SAMPLER_DESC aSamplerDescription{};
        aSamplerDescription.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        aSamplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        aSamplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        aSamplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        aSamplerDescription.MaxLOD = FLT_MAX;
        aResult = mDevice->CreateSamplerState(&aSamplerDescription, &mLinearClampSampler);
        if (FAILED(aResult))
            return Fail("CreateSamplerState(FSR clamp sampler)", aResult);

        mRenderer = theRenderer;
        mAvailable = true;
        mLastError.clear();
        return true;
    }

    bool EnsureSourceView(SDL_Texture* theSource, int theSourceWidth, int theSourceHeight)
    {
        const SDL_PropertiesID aTextureProperties = SDL_GetTextureProperties(theSource);
        if (aTextureProperties == 0)
            return Fail("SDL_GetTextureProperties(FSR source)");

        ID3D11Texture2D* aNativeTexture = static_cast<ID3D11Texture2D*>(SDL_GetPointerProperty(
            aTextureProperties,
            SDL_PROP_TEXTURE_D3D11_TEXTURE_POINTER,
            nullptr));
        if (aNativeTexture == nullptr)
            return Fail("SDL did not expose the FSR source D3D11 texture");

        D3D11_TEXTURE2D_DESC aSourceDescription{};
        aNativeTexture->GetDesc(&aSourceDescription);
        if (aSourceDescription.SampleDesc.Count != 1)
            return Fail("Multisampled FSR source textures are unsupported");
        if (theSourceWidth > static_cast<int>(aSourceDescription.Width) ||
            theSourceHeight > static_cast<int>(aSourceDescription.Height))
            return Fail("FSR source viewport exceeds the native D3D11 texture");

        bool aSamplesAreLinear = false;
        const DXGI_FORMAT aViewFormat = ResolveSourceViewFormat(
            aSourceDescription.Format,
            aSamplesAreLinear);
        if (aViewFormat == DXGI_FORMAT_UNKNOWN)
        {
            char aBuffer[160];
            std::snprintf(
                aBuffer,
                sizeof(aBuffer),
                "Unsupported FSR source format: %s (%u)",
                FormatName(aSourceDescription.Format),
                static_cast<unsigned int>(aSourceDescription.Format));
            return Fail(aBuffer);
        }

        if (mSourceTexture.Get() == aNativeTexture &&
            mSourceView != nullptr &&
            mSourceViewFormat == aViewFormat)
        {
            mSourceSamplesAreLinear = aSamplesAreLinear;
            return true;
        }

        InvalidateSource();
        D3D11_SHADER_RESOURCE_VIEW_DESC aViewDescription{};
        aViewDescription.Format = aViewFormat;
        aViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        aViewDescription.Texture2D.MostDetailedMip = 0;
        aViewDescription.Texture2D.MipLevels = 1;

        HRESULT aResult = mDevice->CreateShaderResourceView(
            aNativeTexture,
            &aViewDescription,
            &mSourceView);
        if (FAILED(aResult))
            return Fail("CreateShaderResourceView(FSR source)", aResult);

        mSourceTexture = aNativeTexture;
        mSourceViewFormat = aViewFormat;
        mSourceSamplesAreLinear = aSamplesAreLinear;
        return true;
    }

    bool CreateOutputResource(
        int theWidth,
        int theHeight,
        DXGI_FORMAT theResourceFormat,
        bool theNeedsShaderView,
        bool theNeedsUnorderedView,
        bool theNeedsSDLTarget,
        SDL_Colorspace theColorspace,
        OutputResource& theResource)
    {
        D3D11_TEXTURE2D_DESC aTextureDescription{};
        aTextureDescription.Width = static_cast<UINT>(theWidth);
        aTextureDescription.Height = static_cast<UINT>(theHeight);
        aTextureDescription.MipLevels = 1;
        aTextureDescription.ArraySize = 1;
        aTextureDescription.Format = theResourceFormat;
        aTextureDescription.SampleDesc.Count = 1;
        aTextureDescription.Usage = D3D11_USAGE_DEFAULT;
        if (theNeedsShaderView || theNeedsSDLTarget)
            aTextureDescription.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        if (theNeedsUnorderedView)
            aTextureDescription.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        if (theNeedsSDLTarget)
            aTextureDescription.BindFlags |= D3D11_BIND_RENDER_TARGET;

        HRESULT aResult = mDevice->CreateTexture2D(
            &aTextureDescription,
            nullptr,
            &theResource.mTexture);
        if (FAILED(aResult))
            return Fail("CreateTexture2D(FSR output)", aResult);

        if (theNeedsUnorderedView)
        {
            D3D11_UNORDERED_ACCESS_VIEW_DESC anUnorderedViewDescription{};
            anUnorderedViewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            anUnorderedViewDescription.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
            aResult = mDevice->CreateUnorderedAccessView(
                theResource.mTexture.Get(),
                &anUnorderedViewDescription,
                &theResource.mUnorderedView);
            if (FAILED(aResult))
                return Fail("CreateUnorderedAccessView(FSR output)", aResult);
        }

        if (theNeedsShaderView)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC aShaderViewDescription{};
            aShaderViewDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            aShaderViewDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            aShaderViewDescription.Texture2D.MipLevels = 1;
            aResult = mDevice->CreateShaderResourceView(
                theResource.mTexture.Get(),
                &aShaderViewDescription,
                &theResource.mShaderView);
            if (FAILED(aResult))
                return Fail("CreateShaderResourceView(FSR intermediate)", aResult);
        }

        if (theNeedsSDLTarget)
        {
            SDL_PropertiesID aProperties = SDL_CreateProperties();
            if (aProperties == 0)
                return Fail("SDL_CreateProperties(FSR output)");

            SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_FORMAT_NUMBER, SDL_PIXELFORMAT_ABGR8888);
            SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_ACCESS_NUMBER, SDL_TEXTUREACCESS_TARGET);
            SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_WIDTH_NUMBER, theWidth);
            SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_HEIGHT_NUMBER, theHeight);
            SDL_SetNumberProperty(aProperties, SDL_PROP_TEXTURE_CREATE_COLORSPACE_NUMBER, theColorspace);
            SDL_SetPointerProperty(
                aProperties,
                SDL_PROP_TEXTURE_CREATE_D3D11_TEXTURE_POINTER,
                theResource.mTexture.Get());

            theResource.mSDLTexture = SDL_CreateTextureWithProperties(mRenderer, aProperties);
            SDL_DestroyProperties(aProperties);
            if (theResource.mSDLTexture == nullptr)
            {
                const char* anError = SDL_GetError();
                return Fail(anError != nullptr && *anError != '\0'
                    ? anError
                    : "SDL_CreateTextureWithProperties(FSR output)");
            }

            SDL_SetTextureBlendMode(theResource.mSDLTexture, SDL_BLENDMODE_NONE);
            SDL_SetTextureScaleMode(theResource.mSDLTexture, SDL_SCALEMODE_LINEAR);
        }

        return true;
    }

    SDL_Texture* GetPresentationTexture() const
    {
        return mPresentation.mSDLTexture != nullptr
            ? mPresentation.mSDLTexture
            : mOutput.mSDLTexture;
    }

    bool EnsureOutput(
        int theWidth,
        int theHeight,
        SDL_Colorspace theColorspace,
        SDL_Colorspace theRendererColorspace,
        bool theNeedsRcas)
    {
        if (GetPresentationTexture() != nullptr &&
            mOutputWidth == theWidth &&
            mOutputHeight == theHeight &&
            mOutputColorspace == theColorspace &&
            mRendererOutputColorspace == theRendererColorspace &&
            (!theNeedsRcas || mIntermediate.mTexture != nullptr))
            return true;

        InvalidateOutput();
        if (theNeedsRcas && !CreateOutputResource(
                theWidth,
                theHeight,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                true,
                true,
                false,
                theColorspace,
                mIntermediate))
        {
            InvalidateOutput();
            return false;
        }
        const bool aNeedsSrgbPresentation = theRendererColorspace == SDL_COLORSPACE_SRGB_LINEAR;
        if (!CreateOutputResource(
                theWidth,
                theHeight,
                DXGI_FORMAT_R8G8B8A8_UNORM,
                false,
                true,
                !aNeedsSrgbPresentation,
                theColorspace,
                mOutput))
        {
            InvalidateOutput();
            return false;
        }
        if (aNeedsSrgbPresentation && !CreateOutputResource(
                theWidth,
                theHeight,
                DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
                false,
                false,
                true,
                theColorspace,
                mPresentation))
        {
            InvalidateOutput();
            return false;
        }

        mOutputWidth = theWidth;
        mOutputHeight = theHeight;
        mOutputColorspace = theColorspace;
        mRendererOutputColorspace = theRendererColorspace;
        return true;
    }

    void ClearComputeBindings()
    {
        std::array<ID3D11ShaderResourceView*, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT> nullShaderViews{};
        std::array<ID3D11UnorderedAccessView*, D3D11_PS_CS_UAV_REGISTER_COUNT> nullUnorderedViews{};
        std::array<ID3D11Buffer*, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT> nullBuffers{};
        std::array<ID3D11SamplerState*, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT> nullSamplers{};

        mContext->CSSetShaderResources(0, static_cast<UINT>(nullShaderViews.size()), nullShaderViews.data());
        mContext->CSSetUnorderedAccessViews(0, static_cast<UINT>(nullUnorderedViews.size()), nullUnorderedViews.data(), nullptr);
        mContext->CSSetConstantBuffers(0, static_cast<UINT>(nullBuffers.size()), nullBuffers.data());
        mContext->CSSetSamplers(0, static_cast<UINT>(nullSamplers.size()), nullSamplers.data());
        mContext->CSSetShader(nullptr, nullptr, 0);
    }

    void Dispatch(
        ID3D11ComputeShader* theShader,
        ID3D11ShaderResourceView* theInput,
        ID3D11UnorderedAccessView* theOutput,
        const FsrConstants& theConstants)
    {
        mContext->UpdateSubresource(mConstantBuffer.Get(), 0, nullptr, &theConstants, 0, 0);
        ID3D11Buffer* aConstantBuffer = mConstantBuffer.Get();
        ID3D11SamplerState* aSampler = mLinearClampSampler.Get();
        mContext->CSSetShader(theShader, nullptr, 0);
        mContext->CSSetConstantBuffers(0, 1, &aConstantBuffer);
        mContext->CSSetSamplers(0, 1, &aSampler);
        mContext->CSSetShaderResources(0, 1, &theInput);
        mContext->CSSetUnorderedAccessViews(0, 1, &theOutput, nullptr);
        mContext->Dispatch(
            static_cast<UINT>((mOutputWidth + 15) / 16),
            static_cast<UINT>((mOutputHeight + 15) / 16),
            1);

        ID3D11ShaderResourceView* aNullShaderView = nullptr;
        ID3D11UnorderedAccessView* aNullUnorderedView = nullptr;
        mContext->CSSetShaderResources(0, 1, &aNullShaderView);
        mContext->CSSetUnorderedAccessViews(0, 1, &aNullUnorderedView, nullptr);
    }

    SDL_Texture* Upscale(
        SDL_Texture* theSource,
        int theSourceWidth,
        int theSourceHeight,
        int theOutputWidth,
        int theOutputHeight,
        float theRcasSharpness)
    {
        mOutputValid = false;
        if (!mAvailable || mRenderer == nullptr || mDevice == nullptr || mContext == nullptr)
        {
            Fail("FSR backend is not initialized");
            return nullptr;
        }
        if (theSource == nullptr)
        {
            Fail("FSR source texture is null");
            return nullptr;
        }
        if (!IsValidDimension(theSourceWidth) || !IsValidDimension(theSourceHeight) ||
            !IsValidDimension(theOutputWidth) || !IsValidDimension(theOutputHeight))
        {
            Fail("FSR received invalid texture dimensions");
            return nullptr;
        }
        if (theOutputWidth < theSourceWidth || theOutputHeight < theSourceHeight)
        {
            Fail("FSR 1 does not support downscaling");
            return nullptr;
        }
        if (SDL_GetRenderTarget(mRenderer) != nullptr)
        {
            Fail("FSR requires SDL's render target to be reset before dispatch");
            return nullptr;
        }

        if (!EnsureSourceView(theSource, theSourceWidth, theSourceHeight))
            return nullptr;

        const SDL_PropertiesID aSourceProperties = SDL_GetTextureProperties(theSource);
        SDL_Colorspace aSourceColorspace = static_cast<SDL_Colorspace>(SDL_GetNumberProperty(
            aSourceProperties,
            SDL_PROP_TEXTURE_COLORSPACE_NUMBER,
            SDL_COLORSPACE_SRGB));
        if (aSourceColorspace == SDL_COLORSPACE_UNKNOWN)
            aSourceColorspace = SDL_COLORSPACE_SRGB;

        const SDL_PropertiesID aRendererProperties = SDL_GetRendererProperties(mRenderer);
        SDL_Colorspace aRendererColorspace = static_cast<SDL_Colorspace>(SDL_GetNumberProperty(
            aRendererProperties,
            SDL_PROP_RENDERER_OUTPUT_COLORSPACE_NUMBER,
            SDL_COLORSPACE_SRGB));
        if (aRendererColorspace == SDL_COLORSPACE_UNKNOWN)
            aRendererColorspace = SDL_COLORSPACE_SRGB;

        const float aSharpness = std::clamp(theRcasSharpness, 0.0f, 1.0f);
        const bool useRcas = aSharpness > 0.0001f;
        if (!EnsureOutput(
                theOutputWidth,
                theOutputHeight,
                aSourceColorspace,
                aRendererColorspace,
                useRcas))
            return nullptr;

        // Submit all queued SDL work before touching the immediate context.
        // SDL invalidates its D3D11 state cache here, so subsequent SDL draws
        // rebuild any state changed by this compute-only interop path.
        if (!SDL_FlushRenderer(mRenderer))
        {
            Fail("SDL_FlushRenderer before FSR dispatch");
            return nullptr;
        }

        // SDL_SetRenderTarget(renderer, nullptr) updates SDL's logical target,
        // but the D3D11 backend can leave the previous texture physically bound
        // to the output merger until the next draw command. Explicitly unbind it
        // before sampling the retained canvas as a compute shader resource.
        // SDL_FlushRenderer invalidated SDL's cached D3D11 state above, so the
        // next SDL draw will restore the appropriate render target.
        mContext->OMSetRenderTargets(0, nullptr, nullptr);

        // A previous SDL composite can leave this output bound to the pixel
        // shader. Unbind every PS SRV to avoid a read/write hazard when the
        // same resource is rebound as a compute UAV.
        std::array<ID3D11ShaderResourceView*, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT> nullPixelViews{};
        mContext->PSSetShaderResources(0, static_cast<UINT>(nullPixelViews.size()), nullPixelViews.data());
        ClearComputeBindings();

        FsrConstants anEasuConstants{};
        FsrEasuCon(
            anEasuConstants.mConst0,
            anEasuConstants.mConst1,
            anEasuConstants.mConst2,
            anEasuConstants.mConst3,
            static_cast<float>(theSourceWidth),
            static_cast<float>(theSourceHeight),
            static_cast<float>(theSourceWidth),
            static_cast<float>(theSourceHeight),
            static_cast<float>(theOutputWidth),
            static_cast<float>(theOutputHeight));
        anEasuConstants.mSample[0] = mSourceSamplesAreLinear ? 1u : 0u;
        anEasuConstants.mSample[1] = static_cast<AU1>(theOutputWidth);
        anEasuConstants.mSample[2] = static_cast<AU1>(theOutputHeight);

        Dispatch(
            mEasuShader.Get(),
            mSourceView.Get(),
            useRcas ? mIntermediate.mUnorderedView.Get() : mOutput.mUnorderedView.Get(),
            anEasuConstants);

        if (useRcas)
        {
            FsrConstants anRcasConstants{};
            // Match the FidelityFX SDK sample UI: the intuitive [0, 1]
            // sharpness control maps to RCAS's [2, 0] stops range.
            const float aRcasStops = 2.0f - (2.0f * aSharpness);
            FsrRcasCon(anRcasConstants.mConst0, aRcasStops);
            anRcasConstants.mSample[1] = static_cast<AU1>(theOutputWidth);
            anRcasConstants.mSample[2] = static_cast<AU1>(theOutputHeight);
            Dispatch(
                mRcasShader.Get(),
                mIntermediate.mShaderView.Get(),
                mOutput.mUnorderedView.Get(),
                anRcasConstants);
        }

        ClearComputeBindings();
        if (mPresentation.mTexture != nullptr)
        {
            // D3D11 UAVs cannot use an sRGB format. Copy the perceptual UNORM
            // bytes to a typed sRGB presentation target so SDL can both sample
            // it as linear in scRGB mode and identify its format for F10
            // render-target readback.
            mContext->CopyResource(mPresentation.mTexture.Get(), mOutput.mTexture.Get());
        }
        mOutputValid = true;
        mLastError.clear();
        return GetPresentationTexture();
    }
};

Fsr1D3D11Backend::Fsr1D3D11Backend()
    : mImpl(std::make_unique<Impl>())
{
}

Fsr1D3D11Backend::~Fsr1D3D11Backend()
{
    Shutdown();
}

bool Fsr1D3D11Backend::Initialize(SDL_Renderer* theRenderer)
{
    return mImpl->Initialize(theRenderer);
}

SDL_Texture* Fsr1D3D11Backend::Upscale(
    SDL_Texture* theSource,
    int theSourceWidth,
    int theSourceHeight,
    int theOutputWidth,
    int theOutputHeight,
    float theRcasSharpness)
{
    return mImpl->Upscale(
        theSource,
        theSourceWidth,
        theSourceHeight,
        theOutputWidth,
        theOutputHeight,
        theRcasSharpness);
}

void Fsr1D3D11Backend::InvalidateOutput()
{
    mImpl->InvalidateOutput();
}

void Fsr1D3D11Backend::Shutdown()
{
    if (mImpl != nullptr)
        mImpl->Shutdown();
}

bool Fsr1D3D11Backend::IsAvailable() const
{
    return mImpl != nullptr && mImpl->mAvailable;
}

bool Fsr1D3D11Backend::HasValidOutput() const
{
    return mImpl != nullptr && mImpl->mOutputValid && mImpl->GetPresentationTexture() != nullptr;
}

SDL_Texture* Fsr1D3D11Backend::GetLastOutput() const
{
    return HasValidOutput() ? mImpl->GetPresentationTexture() : nullptr;
}

int Fsr1D3D11Backend::GetOutputWidth() const
{
    return mImpl != nullptr ? mImpl->mOutputWidth : 0;
}

int Fsr1D3D11Backend::GetOutputHeight() const
{
    return mImpl != nullptr ? mImpl->mOutputHeight : 0;
}

const char* Fsr1D3D11Backend::GetLastError() const
{
    return mImpl != nullptr ? mImpl->mLastError.c_str() : "FSR backend is unavailable";
}
}
