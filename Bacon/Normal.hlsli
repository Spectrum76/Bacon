float3 Encode(float3 n)
{
    return n * 0.5 + 0.5;
}

float3 Decode(float3 f)
{
    return f * 2.0 - 1.0;
}