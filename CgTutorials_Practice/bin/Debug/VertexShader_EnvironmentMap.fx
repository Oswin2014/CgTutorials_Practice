
//--------------------------------------------------
struct XYZW_Normal_Tex
{
	float4 position : POSITION;
	float3 normal	: NORMAL;
	float2 texCoord : TEXCOORD0;
};
//--------------------------------------------------
struct XYZW_Color_Tex
{
	float4 position : POSITION;
	float4 color	: COLOR0;
	float2 texCoord : TEXCOORD0;
	float3 R		: TEXCOORD1;
};
//--------------------------------------------------
struct Light
{
	float3 position;
	float3 color;
};
//--------------------------------------------------
struct Material
{
	float3 Ke;
	float3 Ka;
	float3 Kd;
	float3 Ks;
	float  shininess;
};
//--------------------------------------------------

float4x4 u_ModelViewProjMatrix;

float4x4 u_ModelToWorld;

float3 u_GlobalAmbient;

float3 u_EyePosition;

Material u_Material;

Light u_Light : register(c0);

//--------------------------------------------------



/*------ Functions --------------------------------------------------------*/

void C5E5_computeLighting(Light light,
	float3 P,
	float3 N,
	float3 eyePosition,
	float  shininess,
	out float3 diffuseResult,
	out float3 specularResult)
{
	//Compute the diffuse lighting
	float3 L = normalize(light.position - P);
	float diffuseLight = max(dot(N, L), 0);

	//diffuse term
	diffuseResult = light.color * diffuseLight;

	//Compute the specular lighting
	float3 V = normalize(eyePosition - P);
	float3 H = normalize(L + V);

	// specular term
	float specularLight = pow(max(dot(N, H), 0), shininess);
	if (diffuseLight <= 0) specularLight = 0;
	specularResult = light.color * specularLight;

}
//--------------------------------------------------
void C7E1v_reflection(XYZW_Normal_Tex input,
out XYZW_Color_Tex output)
{
	float3 emissive = 0;
	float3 ambient = 0;
	float3 diffuse = 0;
	float3 specular = 0;

	output.position = mul(input.position, u_ModelViewProjMatrix);
	
	float3 P = mul(input.position, u_ModelToWorld).xyz;
	float3 N = mul(input.normal, (float3x3)u_ModelToWorld);
	N = normalize(N);

	//emissive term
	emissive = u_Material.Ke;

	//ambient term
	ambient = u_Material.Ka * u_GlobalAmbient;

	float3 diffuseLight = 0;
	float3 specularLight = 0;

	C5E5_computeLighting(u_Light, P, N, u_EyePosition,
		u_Material.shininess, diffuseLight, specularLight);

	diffuse = u_Material.Kd * diffuseLight;
	specular = u_Material.Ks * specularLight;

	output.color.xyz = emissive + ambient + diffuse + specular;
	//output.color.xyz = input.normal;
	output.color.w = 1;
	//output.color = 0;

	output.texCoord = input.texCoord;

	float3 I = P - u_EyePosition;
	output.R = reflect(I, N);
}
//--------------------------------------------------
