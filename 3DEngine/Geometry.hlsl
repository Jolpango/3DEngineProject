struct GS_IN
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

struct GS_OUT
{
	float4 Pos : SV_POSITION;
	float4 Color : COLOR;
};

cbuffer GS_CONSTANT_BUFFER : register(b0)
{
	matrix WorldMatrix;
	matrix ViewMatrix;
	matrix ProjectionMatrix;
}
[maxvertexcount(3)]
void GS_main(
	triangle GS_IN input[3] : SV_POSITION,
	inout TriangleStream< GS_OUT > output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;
		element.Pos = input[i].Pos;
		element.Color = input[i].Color;
		element.Pos = mul(element.Pos, WorldMatrix);
		element.Pos = mul(element.Pos, ViewMatrix);
		element.Pos = mul(element.Pos, ProjectionMatrix);

		output.Append(element);
	}
	output.RestartStrip();

	float3 v1 = input[1].Pos - input[0].Pos;
	float3 v2 = input[2].Pos - input[0].Pos;
	float3 normal = cross(v1, v2);

	for (uint i = 0; i < 3; i++)
	{
		GS_OUT element;
		element.Pos = input[i].Pos;
		element.Color = input[i].Color;
		element.Pos.xyz += normal;
		element.Pos = mul(element.Pos, WorldMatrix);
		element.Pos = mul(element.Pos, ViewMatrix);
		element.Pos = mul(element.Pos, ProjectionMatrix);

		output.Append(element);
	}
}