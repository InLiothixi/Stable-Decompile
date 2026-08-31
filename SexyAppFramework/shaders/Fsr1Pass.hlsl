// This adapter is based on AMD's FidelityFX FSR 1.0 sample adapter and is
// distributed under the MIT license in third_party/FidelityFX-FSR/LICENSE.txt.

cbuffer FsrConstants : register(b0)
{
    uint4 Const0;
    uint4 Const1;
    uint4 Const2;
    uint4 Const3;
    // x: EASU input samples are linear, y/z: output width/height.
    uint4 Sample;
};

#define A_GPU 1
#define A_HLSL 1

#include "ffx_a.h"

Texture2D<float4> InputTexture : register(t0);
SamplerState LinearClampSampler : register(s0);
RWTexture2D<float4> OutputTexture : register(u0);

AF1 LinearToSrgbChannel(AF1 value)
{
    value = AMaxF1(value, 0.0);
    if (value <= 0.0031308)
        return value * 12.92;
    return 1.055 * pow(value, 1.0 / 2.4) - 0.055;
}

AF4 MaybeEncodeLinearSamples(AF4 value)
{
    if (Sample.x == 0)
        return value;

    return ASatF4(AF4(
        LinearToSrgbChannel(value.x),
        LinearToSrgbChannel(value.y),
        LinearToSrgbChannel(value.z),
        LinearToSrgbChannel(value.w)));
}

#if defined(FSR_PASS_EASU)

#define FSR_EASU_F 1

AF4 FsrEasuRF(AF2 position)
{
    return MaybeEncodeLinearSamples(InputTexture.GatherRed(LinearClampSampler, position, int2(0, 0)));
}

AF4 FsrEasuGF(AF2 position)
{
    return MaybeEncodeLinearSamples(InputTexture.GatherGreen(LinearClampSampler, position, int2(0, 0)));
}

AF4 FsrEasuBF(AF2 position)
{
    return MaybeEncodeLinearSamples(InputTexture.GatherBlue(LinearClampSampler, position, int2(0, 0)));
}

#elif defined(FSR_PASS_RCAS)

#define FSR_RCAS_F 1

AF4 FsrRcasLoadF(ASU2 position)
{
    return InputTexture.Load(int3(position, 0));
}

void FsrRcasInputF(inout AF1 red, inout AF1 green, inout AF1 blue)
{
    // EASU writes perceptual (sRGB-curve) values. RCAS operates in the same
    // domain, so no input transform is required here.
}

#else
#error Define exactly one of FSR_PASS_EASU or FSR_PASS_RCAS.
#endif

#include "ffx_fsr1.h"

void FilterPixel(ASU2 position)
{
    if (uint(position.x) >= Sample.y || uint(position.y) >= Sample.z)
        return;

    AF3 color;
#if defined(FSR_PASS_EASU)
    FsrEasuF(color, position, Const0, Const1, Const2, Const3);
#else
    FsrRcasF(color.r, color.g, color.b, position, Const0);
#endif
    OutputTexture[position] = AF4(color, 1.0);
}

[numthreads(64, 1, 1)]
void mainCS(uint3 localThreadId : SV_GroupThreadID, uint3 workGroupId : SV_GroupID)
{
    AU2 localPosition = ARmp8x8(localThreadId.x) + AU2(workGroupId.x << 4, workGroupId.y << 4);
    FilterPixel(localPosition);
    FilterPixel(localPosition + AU2(8, 0));
    FilterPixel(localPosition + AU2(8, 8));
    FilterPixel(localPosition + AU2(0, 8));
}
