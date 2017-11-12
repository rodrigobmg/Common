
#include "common.fx"


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VSOUT_POSTEX VS( float4 Pos : POSITION
	, float2 Tex : TEXCOORD0
	 )
{
	VSOUT_POSTEX output = (VSOUT_POSTEX)0;
    output.Pos = mul( Pos, gWorld );
    output.Pos = mul( output.Pos, gView );
    output.Pos = mul( output.Pos, gProjection );
    output.Tex = Tex;
	output.clip = dot(mul(Pos, gWorld), gClipPlane);
    return output;
}


VSOUT_POSTEX VS_Skybox( float4 Pos : POSITION
	, float2 Tex : TEXCOORD0
	 )
{
	VSOUT_POSTEX output = (VSOUT_POSTEX)0;
	matrix mView = gView;
	mView._41 = 0;
	mView._42 = -0.4;
	mView._43 = 0;

    output.Pos = mul( Pos, gWorld );
    output.Pos = mul( output.Pos, mView );
    output.Pos = mul( output.Pos, gProjection );
    output.Tex = Tex;
	output.clip = dot(mul(Pos, gWorld), gClipPlane);
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VSOUT_POSTEX input ) : SV_Target
{
    return txDiffuse.Sample( samLinear, input.Tex );
}


technique11 Unlit
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}


technique11 Skybox
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS_Skybox()));
		SetGeometryShader(NULL);
        SetHullShader(NULL);
       	SetDomainShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}

