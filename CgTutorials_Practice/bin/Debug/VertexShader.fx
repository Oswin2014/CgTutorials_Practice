//--------------------------------------------------
struct C2E1v_Output
{
	float4 position : POSITION;
	float4 color : COLOR0;
};
//--------------------------------------------------
struct C3E2v_Output
{
	float4 position : POSITION;
	float4 color	: COLOR0;
	float2 texCoord : TEXCOORD0;
};
//--------------------------------------------------
struct C3E4_Output
{
	float4 position : POSITION;
	float4 color 	: COLOR0;
};
//--------------------------------------------------
struct XYZW_Normal
{
	float4 position : POSITION;
	float3 normal	: NORMAL;
};
//--------------------------------------------------
struct XYZW_Color
{
	float4 position : POSITION;
	float4 color 	: COLOR0;
};
//--------------------------------------------------
struct FragmentLight
{
	float4 position		: POSITION;
	float3 objectPos	: TEXCOORD0;
	float3 normal		: TEXCOORD1;
};
//--------------------------------------------------
float4x4 u_ModelViewProjMatrix;

float3 u_GlobalAmbient;

float3 u_EyePosition;
//--------------------------------------------------


/*------ Functions --------------------------------------------------------*/
C2E1v_Output C2E1v_green(float2 position : POSITION)
{
	C2E1v_Output OUT;
	OUT.position = float4(position, 0, 1);
	OUT.color = float4(0, 1, 0, 1);
	return OUT;
}
//--------------------------------------------------
C3E2v_Output C3E2v_input( C3E2v_Output Input )
{
	C3E2v_Output OUT;
	OUT = Input;
	return OUT;
}
//--------------------------------------------------
uniform float4 testColor;

C3E2v_Output C3E2v_varying(float4 position : POSITION,
float4 color : COLOR0, float2 texCoord : TEXCOORD0
//, uniform float4 constantColor
)
{
	C3E2v_Output OUT;
	OUT.position = position;
	OUT.color = color;
	OUT.texCoord = texCoord;
	return OUT;
}
//--------------------------------------------------
C3E2v_Output C3E4v_twist( C3E2v_Output input, uniform float twisting )
{
	C3E2v_Output OUT;
	OUT = input;
	float angle = twisting * length(input.position);
	float cosLength, sinLength;
	sincos(angle, sinLength, cosLength);
	OUT.position[0] = cosLength * input.position[0] + 
					 -sinLength * input.position[1];
	OUT.position[1] = sinLength * input.position[0] + 
					  cosLength * input.position[1];
					  
	OUT.position[2] = 0;
	OUT.position[3] = 1;
	return OUT;
}
//--------------------------------------------------
void C3E5v_twoTextures(float2 position : POSITION,
float2 texCoord : TEXCOORD0, 
out float4 oPosition : POSITION,
out float2 leftTexCoord : TEXCOORD0,
out float2 rightTexCoord : TEXCOORD1,
uniform float2 leftSeparation,
uniform float2 rightSeparation)
{
	oPosition = float4(position, 0, 1);
	leftTexCoord = texCoord + leftSeparation;
	rightTexCoord = texCoord + rightSeparation;
}
//--------------------------------------------------
float4x4 modelTrans;
float4x4 viewTrans;
float4x4 projectionTrans;
float4x4 modelViewProj;

C3E4_Output C4E1_Transform(C3E4_Output Input)
{
	C3E4_Output Output;
	float4 aposition = mul(Input.position, modelViewProj);
	Output.position = aposition;
	//Output.position = float4(aposition.xyz / aposition.w, 1);
	//Output.position.z = 0;
	Output.color = Input.color;
	return Output;
}

//--------------------------------------------------

float3 u_LightColor;
float3 u_LightPosition;

float3 u_Ke;
float3 u_Ka;
float3 u_Kd;
float3 u_Ks;
float u_Shininess;

void C5E1v_basicLight(XYZW_Normal input,
out XYZW_Color output)
{
	output.position = mul(input.position, u_ModelViewProjMatrix);
	//output.color = float4(0, 1, 0, 1);
	
	float3 emissive = float3(0,0,0);
	float3 ambient = float3(0,0,0);
	float3 diffuse = float3(0,0,0);
	float3 specular = float3(0,0,0);
	
	//vertex
	float3 P = input.position.xyz;
	float3 N = input.normal;

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

	
	output.color.xyz = emissive + ambient + diffuse + specular;
	output.color.w = 1;
}

//--------------------------------------------------
void C5E2v_fargmentLighting(XYZW_Normal input,
out FragmentLight output)
{
	output.position = mul(input.position, u_ModelViewProjMatrix);
	output.objectPos = input.position.xyz;
	output.normal = input.normal;
}
//--------------------------------------------------
struct Material
{
	float3 Ke;
	float3 Ka;
	float3 Kd;
	float3 Ks;
	float  shininess;
};

struct Light
{
	float3 position;
	float3 color;
};

//uniform Material u_Material : register(c0);
//uniform Light u_Lights[2] : register(c5);

Material u_Material;
Light u_Lights[2] : register(c0);

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
void C5E4v_twoLights(XYZW_Normal input,
out XYZW_Color output)
{
	output.position = mul(input.position, u_ModelViewProjMatrix);
	
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
}
//--------------------------------------------------
