Texture2D shader_texture : register(t0);
SamplerState sample_type : register(s0);

// Colour pixel/fragment shader
cbuffer ColourBuffer : register(b0)
{
	float4 colour;	
	float4 light_diffuse;
	float4 light_ambient;
	float4 light_direction;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	float4 result = light_ambient;
	float3 direction = float3(-light_direction.x, -light_direction.y, -light_direction.z);
	float light_intensity;

	//	Calculate the amount of light on this pixel.
	light_intensity = saturate(dot(input.normal, direction));

	if (light_intensity > 0.0f)
	{
		result += (light_diffuse * light_intensity);

		result = saturate(result);
	}

	//	Sample texture colour and combine with the light amount to determine the final colour of the pixel.
	float4 texture_colour = shader_texture.Sample(sample_type, input.tex);

	
	return result * texture_colour;
}