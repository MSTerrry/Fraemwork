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

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 normal : NORMAL;
	float2 tex : TEXCOORD;	
};

struct Inds {
	uint pInd;
	uint nInd;
	uint tInd;
};

Texture2D DiffuseMap :register(t0);
SamplerState Sampler : register(s0);

Buffer<float3> Positions : register(t0);
Buffer<float3> Normals : register(t1);
Buffer<float3> TexCoords : register(t2);
StructuredBuffer<Inds> Indexes : register(t3);

PS_IN VSMain(uint index:SV_VertexID)
{
	PS_IN output = (PS_IN)0;

	Inds inds = Indexes[index];
	float3 pos = Positions[inds.pInd];
	float3 normal = Normals[inds.nInd];
	float2 tex = TexCoords[inds.tInd];

	output.pos = mul(float4(pos, 1.0f), ConstData.WorldViewProj);
	output.normal = float4(normal,0.0f);
	output.tex = tex;	

	return output;
}

float4 PSMain(PS_IN input) : SV_Target
{
	float4 col = DiffuseMap.Sample(Sampler, float2(input.tex.x,1.0f-input.tex.y));

	clip(col.a - 0.1f);
	return col;
}