struct ConstantData 
{
	float4x4 WorldViewProj;
	float4x4 World;
	float4 ClipPlane;
};

cbuffer ConstBuf:register(b0) 
{
	ConstantData ConstData;
}

struct VS_IN {
	float4 pos : POSITION;
	float4 normal : NORMAL;
	float4 tex : TEXTCOORD;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float2 tex : TEXCOORD;
	float clip : SV_ClipDistance0;
};

Texture2D DiffuseMap :register(t0);
SamplerState Sampler : register(s0);

PS_IN VSMain(VS_IN input)
{
	PS_IN output = (PS_IN)0;

	output.pos = mul(float4(input.pos.xyz, 1.0f), ConstData.WorldViewProj);
	output.normal = input.normal;

	output.tex = input.tex.xy;
	float4 worldPos = mul(float4(input.pos.xyz, 1.0f), ConstData.World);
	output.clip = dot(float4(worldPos.xyz, 1.0f), ConstData.ClipPlane);

	return output;
}

float4 PSMain(PS_IN input, in bool isFrontFace : SV_IsFrontFace) : SV_Target
{
	float4 col = {0.0f, 1.0f,0.0f,1.0f};

	if (!isFrontFace)
	{
		col = DiffuseMap.Sample(Sampler, float2(input.tex.x, 1.0f - input.tex.y));
	}

	return col;
}