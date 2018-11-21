#version 400

layout(location = 0) in vec3 Position;

out vec3 position_v;
void main() {
	vec3 p = vec3(Position.x, Position.y, Position.z);
	gl_Position = vec4(p, 1.0);//gl_position is the preserved vec4 variable defining the vertex position on screen after vertex shader
	position_v = p;
}