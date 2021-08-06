#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 4

struct DirLight
{
	float3 colour;
	float3 direction;

	float ambientIntensity;
	float diffuseIntensity;
};

struct PointLight
{
	float3 colour;
	float3 position;

	float ambientIntensity;
	float diffuseIntensity;
	
	float constant;
	float lin;
	float quadratic;
};

struct SpotLight
{
	float3 colour;
	float3 position;
	float3 direction;

	float procEdge;
	float constant;
	float lin;
	float quadratic;
};

float4 CalcDirectionalLight(DirLight light, float3 Normal, float3 PositionWS, float3 eye, float shadowFactor)
{
	float4 ambientColour = float4(light.colour, 1.0f) * light.ambientIntensity;

    float diffuseFactor = max(dot(normalize(Normal), normalize(light.direction)), 0.0f);
    float4 diffuseColour = float4(light.colour, 1.0f) * light.diffuseIntensity * diffuseFactor;
	
    float4 specularColour = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
    if (diffuseFactor > 0.0f)
    {
        float3 fragToEye = normalize(eye - PositionWS);
        float3 reflectedVertex = normalize(reflect(light.direction, normalize(Normal)));
		
        float specularFactor = dot(fragToEye, reflectedVertex);
        if (specularFactor > 0.0f)
        {
            specularFactor = pow(specularFactor, 4);
            specularColour = float4(light.colour * 0.3f * specularFactor, 1.0f);
        }
    }
	
    return (ambientColour + (1.0 - shadowFactor) * (diffuseColour + specularColour));
}