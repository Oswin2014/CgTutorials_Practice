
#define ATTENUATION_LIGHT 0
#define SPOT_ATTENUATION_LIGHT 1

//--------------------------------------------------
struct C2E2f_Output
{
	float4 color	: COLOR0;
};
//--------------------------------------------------
struct C3E3f_Output
{
	float4 color	: COLOR0;
};
//--------------------------------------------------
struct FragmentLight
{
	float3 objectPos	: TEXCOORD0;
	float3 normal		: TEXCOORD1;
};
//--------------------------------------------------

struct Light
{
	float3	position;
	float3	color;
	float	kC;
	float	kL;
	float	kQ;
	float3	direction;
	float	cosInnerCone;
	float	cosOuterCone;
};

struct Material
{
	float3 Ke;
	float3 Ka;
	float3 Kd;
	float3 Ks;
	float  shininess;
};

float3 u_EyePosition;

Light u_Light;
Material u_Material;

float4x4 u_ModelMatrix;

//--------------------------------------------------

C2E2f_Output C2E2f_passthrough(float4 color : COLOR0)
{
	C2E2f_Output OUT;
	OUT.color = color;
	return OUT;
}
//--------------------------------------------------

C3E3f_Output C3E3f_texture(float2 texCoord : TEXCOORD0,
uniform sampler2D decal : register(s1))
{
	C3E3f_Output OUT;
	OUT.color = tex2D(decal, texCoord);
	return OUT;
}
//--------------------------------------------------
void C3E6f_twoTextures(float2 leftTexCoord : TEXCOORD0,
float2 rightTexCoord : TEXCOORD1,
out float4 color : COLOR0,
uniform sampler2D decal : register(s0))
{
	C3E3f_Output OUT;
	float4 leftColor 	= tex2D(decal, leftTexCoord);
	float4 rightColor  	= tex2D(decal, rightTexCoord);
	color = lerp(leftColor, rightColor, 0.5);
	//return OUT;
}
//--------------------------------------------------

float3 u_GlobalAmbient;
float3 u_LightColor;
float3 u_LightPosition;
float3 u_Ke;
float3 u_Ka;
float3 u_Kd;
float3 u_Ks;
float u_Shininess;

void C5E3f_basicLight(FragmentLight input,
out float4 color : COLOR)
{
	float3 emissive = float3(0,0,0);
	float3 ambient = float3(0,0,0);
	float3 diffuse = float3(0,0,0);
	float3 specular = float3(0,0,0);
	
	//vertex
	float3 P = input.objectPos.xyz;
	float3 N = normalize(input.normal);

	//emissive term
	emissive = u_Ke;
	
	//ambient term
	ambient = u_Ka * u_GlobalAmbient;
	
	//light direction
	float3 L = normalize(u_LightPosition - P);
	float diffuseLight = max(dot(N,L), 0);

	//diffuse term
	diffuse = u_Kd * u_LightColor * diffuseLight;

	float3 V = normalize(u_EyePosition - P);
	float3 H = normalize(L + V);

	// specular term
	float specularLight = pow( max(dot(N,H), 0), u_Shininess);
	if(diffuseLight <= 0) specularLight = 0;
	specular = u_Ks * u_LightColor * specularLight;
	
	color.xyz = emissive + ambient + diffuse + specular;
	color.w = 1;
}
//--------------------------------------------------

float C5E6_attenuation(float3 P, Light light)
{
	float d = distance(P, light.position);
	return 1 / (light.kC + light.kL * d + light.kQ * d * d);
}

//--------------------------------------------------

void C5E7_attenuateLighting(Light light,
float3 P,
float3 N,
float3 eyePosition,
float  shininess,
out float3 diffuseResult,
out float3 specularResult)
{
	//Compute attenuation
	float attenuation = C5E6_attenuation(P, light);

	//Compute the diffuse lighting
	float3 L = normalize(light.position - P);
	float diffuseLight = max(dot(N,L), 0);

	//diffuse term
	diffuseResult = light.color * diffuseLight * attenuation;

	//Compute the specular lighting
	float3 V = normalize(eyePosition - P);
	float3 H = normalize(L + V);

	// specular term
	float specularLight = pow( max(dot(N,H), 0), shininess);
	if(diffuseLight <= 0) specularLight = 0;
	specularResult = light.color * specularLight * attenuation;

}

//--------------------------------------------------

float C5E8_spotlight(float3 P, Light light)
{
	float3 V = normalize(P - light.position);
	float cosCone = light.cosInnerCone;

	float3 D = normalize(light.direction);

	float cosDirection = dot(V, D);

	if(cosCone <= cosDirection)
		return 1;
	else
		return 0;
}

//--------------------------------------------------

float C5E9_dualConeSpotlight(float3 P, Light light)
{
	float3 V = normalize(P - light.position);

	float3 D = normalize(light.direction);

	float cosDirection = dot(V, D);

	return smoothstep(light.cosOuterCone, light.cosInnerCone, cosDirection);
}

//--------------------------------------------------

void C5E10_spotAttenLighting(Light light,
float3 P,
float3 N,
float3 eyePosition,
float  shininess,
out float3 diffuseResult,
out float3 specularResult)
{
	//Compute attenuation
	float attenuation = 1;
	//attenuation = C5E6_attenuation(P, light);

	//float spotEffect = C5E8_spotlight(P, light);
	float spotEffect = C5E9_dualConeSpotlight(P, light);

	float factor = attenuation * spotEffect;

	//Compute the diffuse lighting
	float3 L = normalize(light.position - P);
	float diffuseLight = max(dot(N,L), 0);

	//diffuse term
	diffuseResult = light.color * diffuseLight * factor;

	//Compute the specular lighting
	float3 V = normalize(eyePosition - P);
	float3 H = normalize(L + V);

	// specular term
	float specularLight = pow( max(dot(N,H), 0), shininess);
	if(diffuseLight <= 0) specularLight = 0;
	specularResult = light.color * specularLight * factor;

}

//--------------------------------------------------
void C5_Lighting(FragmentLight input,
out float4 color : COLOR)
{
	float3 emissive = float3(0,0,0);
	float3 ambient = float3(0,0,0);
	float3 diffuse = float3(0,0,0);
	float3 specular = float3(0,0,0);
	
	//vertex
	float3 P = input.objectPos.xyz;
	float3 N = normalize(input.normal);
	
	float4 eyePosition = float4(u_EyePosition.xyz, 1);
	eyePosition = mul(eyePosition, u_ModelMatrix);

	//emissive term
	emissive = u_Material.Ke;
	
	//ambient term
	ambient = u_Material.Ka * u_GlobalAmbient;
	
	float3 diffuseLight;
	float3 specularLight;

	Light light0 = u_Light;
	float4 pos = float4(light0.position.xyz, 1);
	pos = mul(pos, u_ModelMatrix);
	light0.position = pos.xyz;

#if ATTENUATION_LIGHT
	C5E7_attenuateLighting(light0, P, N, eyePosition.xyz, u_Material.shininess, diffuseLight, specularLight);
#elif SPOT_ATTENUATION_LIGHT
	C5E10_spotAttenLighting(light0, P, N, eyePosition.xyz, u_Material.shininess, diffuseLight, specularLight);
#endif

	diffuse = u_Material.Kd * diffuseLight;
	specular = u_Material.Ks * specularLight;
	
	color.xyz = emissive + ambient + diffuse + specular;
	color.w = 1;
}
//--------------------------------------------------
