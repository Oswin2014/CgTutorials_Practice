

//--------------------------------------------------
struct Color_Tex
{
	float4 color	: COLOR0;
	float2 texCoord : TEXCOORD0;
	float3 R		: TEXCOORD1;
};
//--------------------------------------------------


/*------ Functions --------------------------------------------------------*/

void C7E2f_reflection(Color_Tex input,
uniform float reflectivity,
uniform sampler2D decal : register(s1),
uniform samplerCUBE cubeMap : register(s2),
out float4 color : COLOR)
{
	float4 reflectedColor = texCUBE(cubeMap, input.R);

	float4 decalColor = tex2D(decal, input.texCoord);

	color = input.color;
	if (reflectivity == 0)
		color += decalColor;
	color = lerp(color, reflectedColor, reflectivity);
}
//--------------------------------------------------
