#include <Input.hlsli>
#include <Light.hlsli>

Texture2D BaseColor : register(t0);
SamplerState Sampler : register(s0);

Texture2D ShadowMap : register(t1);
SamplerState MapSampler : register(s1);

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

float CalcShadowFactor(PSInput input)
{
    float4 lightSpaceBiased = float4((input.LightSpace.xy * float2(0.5f, -0.5f)) + 0.5f.xx, input.LightSpace.zw);
    float3 projCoords = lightSpaceBiased.xyz / lightSpaceBiased.w;
	
    float current = projCoords.z;
	
    float3 normal = normalize(input.Normal);
    float3 lightDir = normalize(Sun.direction);
	
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.0005);

    float width;
    float height;
	
    ShadowMap.GetDimensions(width, height);
	
    float shadow = 0.0;
    float2 texelSize = (1.0 / width, 1.0 / height);
    
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = ShadowMap.Sample(MapSampler, projCoords.xy + float2(x, y) * texelSize).r;
            shadow += current - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;
	
    if (projCoords.z > 1.0)
    {
        shadow = 0.0;
    }
	
    return shadow;
}

float4 main(PSInput input) : SV_TARGET
{
    float4 Final = BaseColor.Sample(Sampler, input.TexCoord);
	
    if (Final.a == 0.0)
    {
        discard;
    }
	
    float shadowFactor = CalcShadowFactor(input);
    
    Final *= CalcDirectionalLight(Sun, input.Normal, input.PositionWS, Eye, shadowFactor);
    
    Final.a = BaseColor.Sample(Sampler, input.TexCoord).a;
    
    return Final;
}