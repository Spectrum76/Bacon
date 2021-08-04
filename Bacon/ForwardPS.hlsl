#include <Input.hlsli>
#include <Light.hlsli>

Texture2D BaseColor : register(t0);
SamplerState Sampler : register(s0);

cbuffer Camera : register(b0)
{
    row_major matrix View;
    row_major matrix Proj;
    float3 Eye;
}

cbuffer DirLightUB : register(b1)
{
    DirLight Sun;
}

cbuffer PointLightUB : register(b2)
{
    PointLight PointLights;
}

float4 main(PSInput input) : SV_TARGET
{
    float4 Final = BaseColor.Sample(Sampler, input.TexCoord);
	
    if (Final.a == 0.0)
    {
        discard;
    }
	
    Final *= CalcDirectionalLight(Sun, input.Normal, input.PositionWS, Eye);
    
    return Final;
}