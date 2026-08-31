# AMD FidelityFX Super Resolution 1.0

The files in this directory are vendored verbatim from
<https://github.com/GPUOpen-Effects/FidelityFX-FSR> at commit
`a21ffb8f6c13233ba336352bdff293894c706575`.

Vendored files and SHA-256 digests:

| File | SHA-256 |
| --- | --- |
| `ffx_a.h` | `c82c2092e0739121f9c26aee5b6afec765d44466f010518cbc43fdbc6dcbc7b7` |
| `ffx_fsr1.h` | `6d16231b29f0537f7737aebdb4affabba6da114247fecb8da93b060511659ce0` |
| `LICENSE.txt` | `84bb6af6a570cc2d737b1d530534139592c89f5a9114e4c6aabf0fb8e7762375` |

The local D3D11 adapter shader is in
`SexyAppFramework/shaders/Fsr1Pass.hlsl`. Its checked-in DXBC is regenerated
with `tools/Compile-Fsr1Shaders.ps1`.
