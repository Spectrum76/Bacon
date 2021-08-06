#include <Input.hlsli>

cbuffer Transform : register(b0)
{
    row_major matrix Model;
    row_major matrix iModel;
}

cbuffer LightSpace : register(b2)
{
    row_major matrix View;
    row_major matrix Proj;
}

float4 main(VSInput Input) : SV_POSITION
{
    return mul(float4(Input.Position, 1.0f), mul(Model, mul(View, Proj)));
}