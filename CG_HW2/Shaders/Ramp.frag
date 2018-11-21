#version 400

uniform sampler2D tex;
uniform sampler1D rampTex;
uniform vec3 lightPos;

vec3 lightPosition;	//Light Position in eye-coords
vec3 La;		//Ambient light intensity
vec3 Ld;		//Diffuse light intensity
vec3 Ls;		//Specular light intensity

vec3 Ka;			//Ambient reflectivity
vec3 Kd;			//Diffuse reflectivity
vec3 Ks;			//Specular reflectivity
float shininess;	//Specular shininess factor
vec3 color;

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;

out vec4 finalColor;

void light(vec3 position, vec3 norm, out vec3 diffuse)
{
	vec3 n = normalize(norm);
	vec3 s = normalize(lightPosition - position);

	float sDotN = dot(s, n) * 0.5 + 0.5;
	if (sDotN > 0.9)
		color = vec3(1.0);
	else if (sDotN > 0.8)
		color = vec3(0.8);
	else if (sDotN > 0.6)
		color = vec3(0.6);
	else if (sDotN > 0.3)
		color = vec3(0.3);
	else if (sDotN > 0.15)
		color = vec3(0.15);
	else
		color = vec3(0.1);
	diffuse = Ld * Kd * color;
}

void main() {
    //note: the texture function was called texture2D in older versions of GLSL
	Ka = vec3(1.0f, 1.0f, 1.0f);
	Kd = vec3(1.0f, 1.0f, 1.0f);
	Ks = vec3(1.0f, 1.0f, 1.0f);
	shininess = 100;
	La = vec3(0.2f, 0.2f, 0.2f);
	Ld = vec3(0.8f, 0.8f, 0.8f);
	Ls = vec3(0.5f, 0.5f, 0.5f);
	lightPosition = lightPos;

	vec3 diffuseSum = vec3(0);
	vec3 diffuse;
 
	if ( gl_FrontFacing ){
		light(fragPos, fragNormal, diffuse);
		diffuseSum += diffuse;
	}
	else{
		light(fragPos, -fragNormal, diffuse);
		diffuseSum += diffuse;
	}
	vec4 texColor = texture(tex, fragTexCoord);
 
	finalColor = vec4(diffuseSum, 1) * texColor;
}