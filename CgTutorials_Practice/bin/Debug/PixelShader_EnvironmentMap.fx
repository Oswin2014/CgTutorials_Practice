

//--------------------------------------------------
struct Color_Tex
{
	float4 color : COLOR0;
	float2 texCoord : TEXCOORD0;
};
//--------------------------------------------------


/*------ Functions --------------------------------------------------------*/

void C7E2f_reflection(Color_Tex input,
uniform sampler2D decal : register(s1),
out float4 color : COLOR)
{
	color = input.color + tex2D(decal, input.texCoord);
}
//--------------------------------------------------
