#version 400

uniform sampler2D tex;
uniform vec3 lightPos;

vec3 lightPosition;	//Light Position in eye-coords
vec3 La;		//Ambient light intensity
vec3 Ld;		//Diffuse light intensity
vec3 Ls;		//Specular light intensity

vec3 Ka;			//Ambient reflectivity
vec3 Kd;			//Diffuse reflectivity
vec3 Ks;			//Specular reflectivity
float shininess;	//Specular shininess factor

in vec2 fragTexCoord;
in vec3 fragNormal;
in vec3 fragPos;

out vec4 finalColor;

void light(vec3 position, vec3 norm, out vec3 ambient, out vec3 diffuse, out vec3 spec)
{
	vec3 n = normalize(norm);
	vec3 s = normalize(lightPosition - position);
	vec3 v = normalize(-position);
	vec3 r = reflect(-s, n);

	ambient = La * Ka;
	float sDotN = max(dot(s, n), 0.0);
	diffuse = Ld * Kd * sDotN;
	spec = Ls * Ks * pow(max(dot(r, v) , 0.0), shininess); 
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

	vec3 ambientSum = vec3(0);
	vec3 diffuseSum = vec3(0);
	vec3 specSum = vec3(0);
	vec3 ambient, diffuse, spec;
 
	if ( gl_FrontFacing ){
		light(fragPos, fragNormal, ambient, diffuse, spec);
		ambientSum += ambient;
		diffuseSum += diffuse;
		specSum += spec;
	}
	else{
		light(fragPos, -fragNormal, ambient, diffuse, spec);
		ambientSum += ambient;
		diffuseSum += diffuse;
		specSum += spec;
	}
	vec4 texColor = texture(tex, fragTexCoord);
 
	finalColor = vec4(ambientSum + diffuseSum + specSum, 1) * texColor;
}