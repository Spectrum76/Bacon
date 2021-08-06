struct PSInput
{
    float4 Position     : SV_POSITION;
    float2 TexCoord     : TEXCOORD;
    float3 Normal       : NORMAL;
    float3 PositionWS   : POSITIONWS;
    float4 LightSpace   : POSITIONLS;
};

struct VSInput
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal   : NORMAL;
};
