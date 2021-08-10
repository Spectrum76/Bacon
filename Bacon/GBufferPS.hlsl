#include <Input.hlsli>
#include <Normal.hlsli>

Texture2D BaseColor : register(t0);
SamplerState Sampler : register(s0);

PSOutput main(VSOutput Input)
{
    PSOutput Output;
    
    Output.Albedo = BaseColor.Sample(Sampler, Input.TexCoord);
    Output.Normal = float4(Encode(normalize(Input.Normal)), 1);
    
    if (Output.Albedo.a <= 0.01)
        discard;
    
    return Output;
}