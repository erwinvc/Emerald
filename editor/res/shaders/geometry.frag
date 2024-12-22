#version 430 core

struct Data {
	vec4 pos;
	vec2 uv;
	vec3 normal;  // Added normal for lighting calculations
};

in Data vsData;
out vec4 FragColor;

uniform int colorIndex;
uniform vec4 color[2];
uniform sampler2D _Albedo;
uniform sampler2D _Normal;
uniform vec4 _BaseColor;

// Hardcoded directional light parameters
vec3 lightDir = normalize(vec3(0.5, -1.0, -0.1));  // Directional light direction
vec3 lightColor = vec3(1.0, 1.0, 1.0);             // Light color (white)
float ambientStrength = 0.1;                       // Ambient light strength
vec3 viewPos = vec3(0.0, 0.0, 3.0);                // Camera position

void main() {
	// Ambient lighting
	vec3 ambient = ambientStrength * lightColor;
	
	// Diffuse lighting
	vec3 norm = normalize(vsData.normal);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;
	
	// Specular lighting (Blinn-Phong)
	vec3 viewDir = normalize(viewPos - vsData.pos.xyz);
	vec3 halfDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfDir), 0.0), 32.0); // 32.0 is the shininess factor
	vec3 specular = spec * lightColor;
	
	// Combine all lighting components
	vec3 lighting = ambient + diffuse + specular;

	vec4 albedo = texture(_Albedo, vsData.uv).rgba;
	if (albedo.a < 0.2) discard;

	FragColor = _BaseColor * albedo * vec4(lighting, 1.0);
}