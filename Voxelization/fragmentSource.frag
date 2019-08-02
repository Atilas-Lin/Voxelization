#version 330 core    
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;//uv座標

struct Material {
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

struct LightDirectional {
	vec3 pos;
	vec3 color;
	vec3 dirToLight;
};

struct LightPoint {
	vec3 pos;
	vec3 color;

	float constant;
	float linear;
	float quadratic;
};

struct LightSpot {
	vec3 pos;
	vec3 color;
	vec3 dirToLight;

	float constant;
	float linear;
	float quadratic;

	float cosPhyInner;
	float cosPhyOutter;
};

uniform Material material;
uniform LightDirectional lightD;
uniform LightPoint lightP0;
uniform LightPoint lightP1;
uniform LightPoint lightP2;
uniform LightPoint lightP3;
uniform LightSpot lightS;

uniform sampler2D texture_diffuse1;

uniform vec3 objColor;
uniform vec3 ambientColor;
uniform vec3 lightPos;
uniform vec3 lightDirUniform;
uniform vec3 lightColor;
uniform vec3 cameraPos;

out vec4 FragColor;   

vec3 CalcLightDirectional(LightDirectional light, vec3 uNormal, vec3 dirToCamera)
{
	// diffuse	max(dot(L, N), 0)
	float diffIntensity = max( dot( light.dirToLight, uNormal), 0);
	vec3 diffColor = diffIntensity * light.color * texture(material.diffuse, TexCoord).rgb;

	// specular pow( max( dot(R, Cam), 0), shininess)
	vec3 R = normalize( reflect( -light.dirToLight, uNormal));
	float specIntensity = pow( max( dot( R, dirToCamera), 0), material.shininess);
	vec3 specColor = specIntensity * light.color * texture(material.specular, TexCoord).rgb;

	vec3 result = diffColor + specColor;
	return result;
}

vec3 CalcLightPoint(LightPoint light, vec3 uNormal, vec3 dirToCamera)
{
	// attenuation
	float dist = length( light.pos - FragPos );
	float attenuation = 1 / ( light.constant + light.linear * dist + light.quadratic * (dist * dist) );

	// diffuse
	float diffIntensity = max( dot( normalize(light.pos - FragPos), uNormal), 0) * attenuation;
	vec3 diffColor = diffIntensity * light.color * texture(material.diffuse, TexCoord).rgb;

	//specular
	vec3 R = normalize( reflect( -normalize(light.pos - FragPos), uNormal)); // light.pos - FragPos 表示一片源指向光源的向量
	float specIntensity = pow( max( dot( R, dirToCamera), 0), material.shininess);
	vec3 specColor = specIntensity * light.color * texture(material.specular, TexCoord).rgb;

	vec3 result = diffColor + specColor;
	return result;
}

vec3 CalcLightSpot(LightSpot light, vec3 uNormal, vec3 dirToCamera)
{
	vec3 lightDir = normalize( light.pos - FragPos );

	// attenuation
	float dist = length( light.pos - FragPos );
	float attenuation = 1 / ( light.constant + light.linear * dist + light.quadratic * (dist * dist) );
	float spotRatio;
	float cosTheta = dot( lightDir, normalize(-light.dirToLight));

	spotRatio = clamp( (cosTheta - light.cosPhyOutter) / (light.cosPhyInner - light.cosPhyOutter), 0.0, 1.0);

	// diffuse
	float diffIntensity = max( dot(lightDir, uNormal), 0) * attenuation * spotRatio;
	vec3 diffColor = diffIntensity * light.color * texture(material.diffuse, TexCoord).rgb;

	// specular
	vec3 R = reflect(-lightDir, uNormal);
	float specIntensity = pow( max( dot( R, dirToCamera), 0), material.shininess) * spotRatio;
	vec3 specColor = specIntensity * light.color * texture(material.specular, TexCoord).rgb;

	vec3 result = diffColor + specColor;
	return result;
}

void main(){
	
	vec3 finalResult = vec3(0, 0, 0);
	vec3 uNormal = normalize(Normal);
	vec3 dirToCamera = normalize(cameraPos - FragPos); // 指向camera的向量

	finalResult += CalcLightDirectional(lightD, uNormal, dirToCamera);
	finalResult += CalcLightPoint(lightP0, uNormal, dirToCamera);
	finalResult += CalcLightPoint(lightP1, uNormal, dirToCamera);
	finalResult += CalcLightPoint(lightP2, uNormal, dirToCamera);
	finalResult += CalcLightPoint(lightP3, uNormal, dirToCamera);
	finalResult += CalcLightSpot(lightS, uNormal, dirToCamera);

	//FragColor = vec4(finalResult, 1.0f);
	FragColor = texture(texture_diffuse1, TexCoord);
}   