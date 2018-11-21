#version 400

uniform sampler2D tex;
uniform sampler2D noiseTex;
uniform float dissolveFactor;

float w = 0.05;
vec4 diffuseColor;

in vec2 fragTexCoord;
in vec3 fragNormal;

out vec4 finalColor;

void main() {
    //note: the texture function was called texture2D in older versions of GLSL
	vec4 sampleT = texture(noiseTex, fragTexCoord);
	diffuseColor = vec4(1.0f, 97.0f / 255.0f, 0.0f, 1.0f);//¾ï¦â
	if(sampleT.r < dissolveFactor)
		discard;
	else if(sampleT.r < dissolveFactor + w){
		float diff =  max(0.0, dot(vec3(1.0f, 1.0f, 1.0f), normalize(fragNormal)));
		finalColor = diff * diffuseColor;
	}
	else
		finalColor = texture(tex, fragTexCoord);
}