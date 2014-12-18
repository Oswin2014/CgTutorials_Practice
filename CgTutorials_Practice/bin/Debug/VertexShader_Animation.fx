
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

float3 u_GlobalAmbient;

float3 u_EyePosition;

Material u_Material;

Light u_Lights[2] : register(c0);

//--------------------------------------------------



/*------ Functions --------------------------------------------------------*/

void C5E5_computeLighting(Light light,
float3 P,
float3 N,
float  shininess,
out float3 diffuseResult,
out float3 specularResult)
{
	//Compute the diffuse lighting
	float3 L = normalize(light.position - P);
	float diffuseLight = max(dot(N,L), 0);

	//diffuse term
	diffuseResult = light.color * diffuseLight;

	//Compute the specular lighting
	float3 V = normalize(u_EyePosition - P);
	float3 H = normalize(L + V);

	// specular term
	float specularLight = pow( max(dot(N,H), 0), shininess);
	if(diffuseLight <= 0) specularLight = 0;
	specularResult = light.color * specularLight;

}
//--------------------------------------------------
void C6E1_pulsate(XYZW_Normal_Tex input,
uniform float time,
uniform float frequency,
uniform float scaleFactor,
out XYZW_Color_Tex output)
{
	frequency = 1.5;
	scaleFactor = 2;

// 	float displacement = scaleFactor * 0.5 * sin(input.position.y * frequency * time) + 1;
// 	float4 displacementDirection = float4(input.normal.x, input.normal.y, input.normal.z, 0);

	float displacement = scaleFactor * 0.5 * sin(frequency * time) + 1;
	float4 displacementDirection = float4(0, 0, input.normal.z, 0);
	float4 newPosition = input.position + displacement * displacementDirection;

	output.position = mul(newPosition, u_ModelViewProjMatrix);
	
	float3 emissive = float3(0,0,0);
	float3 ambient = float3(0,0,0);
	float3 diffuse = float3(0,0,0);
	float3 specular = float3(0,0,0);

	//emissive term
	emissive = u_Material.Ke;
	
	//ambient term
	ambient = u_Material.Ka * u_GlobalAmbient;
	
	//Loop over diffuse and specular contributions for each light
	float3 diffuseLight;
	float3 specularLight;
	float3 diffuseSum = 0;
	float3 specularSum = 0;

	for(int i = 0; i < 2; i++)
	{
		C5E5_computeLighting(u_Lights[i], input.position.xyz, input.normal, 
			u_Material.shininess, diffuseLight, specularLight);

		diffuseSum += diffuseLight;
		specularSum += specularLight;
	}

	diffuse = u_Material.Kd * diffuseSum;
	specular = u_Material.Ks * specularSum;
	
	output.color.xyz = emissive + ambient + diffuse + specular;
	output.color.w = 1;

	output.texCoord = input.texCoord;
}
//--------------------------------------------------
