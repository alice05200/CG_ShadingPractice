/*

CG Homework2 - Phong Shading, Dissolving effects and Ramp effects

Objective - learning GLSL, glm model datatype(for .obj file) 

Overview:

	1. Render the model with texture using VBO and/or VAO

	2. Implement Phong shading

	3. Implement Dissolving effects via shader

	4. Implement Ramp effects via shader (Basically a dicrete shading)

Good luck!

!!!IMPORTANT!!! 

1. Make sure to change the window name to your student ID!
2. Make sure to remove glmDraw() at line 211.
   It's meant for a quick preview of the ball object and shouldn't exist in your final code.

*/
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h""
#include "../shader_lib/shader.h"
#include "glm/glm.h"
extern "C"
{
	#include "glm_helper.h"
}

/*you may need to do something here
you may use the following struct type to perform your single VBO method,
or you can define/declare multiple VBOs for VAO method.
Please feel free to modify it*/
struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};
typedef struct Vertex Vertex;

//no need to modify the following function declarations and global variables
void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void draw_light_bulb(void);
void camera_light_ball_move();
GLuint loadTexture(char* name, GLfloat width, GLfloat height);

namespace
{
	char *obj_file_dir = "../Resources/bunny.obj";
	char *main_tex_dir = "../Resources/Stone.ppm";
	char *noise_tex_dir = "../Resources/Noise.ppm";
	char *ramp_tex_dir = "../Resources/Ramp.ppm";
	
	GLfloat light_rad = 0.05;//radius of the light bulb
	float eyet = 0.0;//theta in degree
	float eyep = 90.0;//phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;
	int mousex = 0;
	int mousey = 0;
}

// You can modify the moving/rotating speed if it's too fast/slow for you
const float speed = 0.01; // camera / light / ball moving speed
const float rotation_speed = 0.01; // ball rotating speed

//you may need to use some of the following variables in your program 

// No need for model texture, 'cause glmModel() has already loaded it for you.
// To use the texture, check glmModel documentation.
GLuint mainTextureID; // TA has already loaded this texture for you
GLuint noiseTextureID; // TA has already loaded this texture for you
GLuint rampTextureID; // TA has already loaded this texture for you

GLMmodel *model; //TA has already loaded the model for you(!but you still need to convert it to VBO(s)!)

GLuint program;
GLuint *vboName;
GLuint vertB, fragB, vertD, fragD, vertR, fragR;//shader
GLfloat *vertexPos;//model vertices
GLfloat *normPos;//model normal
GLfloat *texCordPos;//model texCord

float eyex = 0.0;
float eyey = 0.0;
float eyez = 5.6;

int mode = 0;//shader mode
GLfloat dissolveF = 0.1f;

GLfloat light_pos[] = { 1.1, 1.0, 1.3 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// remember to replace "YourStudentID" with your own student ID
	glutCreateWindow("CG_HW2_0756626");
	glutReshapeWindow(512, 512);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	glmDelete(model);
	return 0;
}

void init(void)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_CULL_FACE);
	model = glmReadOBJ(obj_file_dir);
	mainTextureID = loadTexture(main_tex_dir, 512, 256);
	noiseTextureID = loadTexture(noise_tex_dir, 360, 360);
	rampTextureID = loadTexture(ramp_tex_dir, 256, 256);

	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(model);

	//you may need to do something here(create shaders/program(s) and create vbo(s)/vao from GLMmodel model)

	// APIs for creating shaders and creating shader programs have been done by TAs
	// following is an example for creating a shader program using given vertex shader and fragment shader
	/*
	GLuint vert = createShader("Shaders/bump.vert", "vertex");
	GLuint frag = createShader("Shaders/bump.frag", "fragment");
	GLuint program = createProgram(vert, frag);
	*/
	vertB = createShader("Shaders/bump.vert", "vertex");//phong shading
	fragB = createShader("Shaders/bump.frag", "fragment");
	vertD = createShader("Shaders/Dissolving.vert", "vertex");//dissolving
	fragD = createShader("Shaders/Dissolving.frag", "fragment");
	vertR = createShader("Shaders/Ramp.vert", "vertex");//ramp shading
	fragR = createShader("Shaders/Ramp.frag", "fragment");
	program = createProgram(vertB, fragB);
	//vbo vertices
	vertexPos = new GLfloat[model->numtriangles * 3 * 3];
	normPos = new GLfloat[model->numtriangles * 3 * 3];
	texCordPos = new GLfloat[model->numtriangles * 3 * 2];
	for (int i = 0; i < model->numtriangles; i++){
		vertexPos[i * 9 + 0] = model->vertices[model->triangles[i].vindices[0] * 3 + 0];
		vertexPos[i * 9 + 1] = model->vertices[model->triangles[i].vindices[0] * 3 + 1];
		vertexPos[i * 9 + 2] = model->vertices[model->triangles[i].vindices[0] * 3 + 2];
		vertexPos[i * 9 + 3] = model->vertices[model->triangles[i].vindices[1] * 3 + 0];
		vertexPos[i * 9 + 4] = model->vertices[model->triangles[i].vindices[1] * 3 + 1];
		vertexPos[i * 9 + 5] = model->vertices[model->triangles[i].vindices[1] * 3 + 2];
		vertexPos[i * 9 + 6] = model->vertices[model->triangles[i].vindices[2] * 3 + 0];
		vertexPos[i * 9 + 7] = model->vertices[model->triangles[i].vindices[2] * 3 + 1];
		vertexPos[i * 9 + 8] = model->vertices[model->triangles[i].vindices[2] * 3 + 2];
		if (model->numnormals > 0) {
			normPos[i * 9 + 0] = model->normals[model->triangles[i].nindices[0] * 3 + 0];
			normPos[i * 9 + 1] = model->normals[model->triangles[i].nindices[0] * 3 + 1];
			normPos[i * 9 + 2] = model->normals[model->triangles[i].nindices[0] * 3 + 2];
			normPos[i * 9 + 3] = model->normals[model->triangles[i].nindices[1] * 3 + 0];
			normPos[i * 9 + 4] = model->normals[model->triangles[i].nindices[1] * 3 + 1];
			normPos[i * 9 + 5] = model->normals[model->triangles[i].nindices[1] * 3 + 2];
			normPos[i * 9 + 6] = model->normals[model->triangles[i].nindices[2] * 3 + 0];
			normPos[i * 9 + 7] = model->normals[model->triangles[i].nindices[2] * 3 + 1];
			normPos[i * 9 + 8] = model->normals[model->triangles[i].nindices[2] * 3 + 2];
		}
		if (model->numtexcoords > 0) {
			texCordPos[i * 6 + 0] = model->texcoords[model->triangles[i].tindices[0] * 2 + 0];
			texCordPos[i * 6 + 1] = model->texcoords[model->triangles[i].tindices[0] * 2 + 1];
			texCordPos[i * 6 + 2] = model->texcoords[model->triangles[i].tindices[1] * 2 + 0];
			texCordPos[i * 6 + 3] = model->texcoords[model->triangles[i].tindices[1] * 2 + 1];
			texCordPos[i * 6 + 4] = model->texcoords[model->triangles[i].tindices[2] * 2 + 0];
			texCordPos[i * 6 + 5] = model->texcoords[model->triangles[i].tindices[2] * 2 + 1];
		}
	}
	vboName = new GLuint[3];
	glGenBuffers(3, vboName);//3buffers position+normal+texCord
	
	glBindBuffer(GL_ARRAY_BUFFER, vboName[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model->numtriangles * 3 * 3, vertexPos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(0));
	glBindBuffer(GL_ARRAY_BUFFER, vboName[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model->numtriangles * 3 * 3, normPos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vboName[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * model->numtriangles * 3 * 2, texCordPos, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//you may need to do something here(declare some local variables you need and maybe load Model matrix here...)
	GLfloat *projection = new GLfloat[16];
	GLfloat *view = new GLfloat[16];
	GLfloat *modelView = new GLfloat[16];
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(-model->position[0], -model->position[1], -model->position[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
	glPopMatrix();

	//please try not to modify the following block of code(you can but you are not supposed to)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		eyex, 
		eyey, 
		eyez,
		eyex+cos(eyet*M_PI/180)*cos(eyep*M_PI / 180), 
		eyey+sin(eyet*M_PI / 180), 
		eyez-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),
		0.0,
		1.0,
		0.0);
	draw_light_bulb();
	glPushMatrix();
		glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
		glRotatef(ball_rot[0], 1, 0, 0);
		glRotatef(ball_rot[1], 0, 1, 0);
		glRotatef(ball_rot[2], 0, 0, 1);
	// please try not to modify the previous block of code

	// you may need to do something here(pass uniform variable(s) to shader and render the model)
	//	glmDraw(model,GLM_TEXTURE);// please delete this line in your final code! It's just a preview of rendered object
	glGetFloatv(GL_MODELVIEW_MATRIX, view);
	glMatrixMode(GL_PROJECTION);
	glGetFloatv(GL_PROJECTION_MATRIX, projection);
	
	glUseProgram(program);
	
	GLint loc = glGetUniformLocation(program, "projectionMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, projection);
	loc = glGetUniformLocation(program, "viewMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, view);
	loc = glGetUniformLocation(program, "modelMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, modelView);
	loc = glGetUniformLocation(program, "dissolveFactor");
	if (loc > -1)
		glUniform1fv(loc, 1, &dissolveF);
	loc = glGetUniformLocation(program, "lightPos");
	if (loc > -1)
		glUniform3fv(loc, 1, light_pos);

	loc = glGetUniformLocation(program, "tex");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mainTextureID);
	glUniform1i(glGetUniformLocation(program, "tex"), 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noiseTextureID);
	glUniform1i(glGetUniformLocation(program, "noiseTex"), 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_1D, rampTextureID);
	glUniform1i(glGetUniformLocation(program, "rampTex"), 2);
	glDrawArrays(GL_TRIANGLES, 0, model->numtriangles * 3);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
	glPopMatrix();

	glutSwapBuffers();
	camera_light_ball_move();
}

//please implement mode toggle(switch mode between phongShading/Dissolving/Ramp) in case 'b'(lowercase)
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	//ESC
		break;
	}
	case 'b'://toggle mode
	{
		//you may need to do something here
		mode++;
		if (mode >= 3)
			mode = mode % 3;
		if (mode == 0)
			program = createProgram(vertB, fragB);
		else if(mode == 1)
			program = createProgram(vertD, fragD);
		else{
			program = createProgram(vertR, fragR);
		}
		break;
	}
	case 'd':
	{
		right = true;
		break;
	}
	case 'a':
	{
		left = true;
		break;
	}
	case 'w':
	{
		forward = true;
		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		lright = true;
		break;
	}
	case 'f':
	{
		lleft = true;
		break;
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}
	case 'n':
		if (dissolveF < 1.0f && mode == 1)
			dissolveF += 0.01f;
		break;
	case 'm':
		if (dissolveF > 0.0f && mode == 1)
			dissolveF -= 0.01f;
		break;

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos[1] = eyey + sin(eyet*M_PI / 180);
		light_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://move ball to front of camera
	{
		ball_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) * 3;
		ball_pos[1] = eyey + sin(eyet*M_PI / 180) * 5;
		ball_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180) * 3;
		break;
	}
	case 'c'://reset all pose
	{
		light_pos[0] = 1.1;
		light_pos[1] = 1.0;
		light_pos[2] = 1.3;
		ball_pos[0] = 0;
		ball_pos[1] = 0;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 0.0;
		eyey = 0.0;
		eyez = 5.6;
		eyet = 0;
		eyep = 90;
		break;
	}
	default:
	{
		break;
	}
	}
}

//no need to modify the following functions
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	if (mleft)
	{
		eyep -= (x-mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}

void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz=0;
	if(left|| right || forward || backward || up || down)
	{ 
		if (left)
			dx = -speed;
		else if (right)
			dx = speed;
		if (forward)
			dy = speed;
		else if (backward)
			dy = -speed;
		eyex += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		eyey += dy*sin(eyet*M_PI / 180);
		eyez += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (up)
			eyey += speed;
		else if (down)
			eyey -= speed;
	}
	if(lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		light_pos[1] += dy*sin(eyet*M_PI / 180);
		light_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (lup)
			light_pos[1] += speed;
		else if(ldown)
			light_pos[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		ball_pos[1] += dy*sin(eyet*M_PI / 180);
		ball_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if(bx||by||bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}

void draw_light_bulb()
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glPushMatrix();
	glColor3f(0.4, 0.5, 0);
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	gluSphere(quad, light_rad, 40, 20);
	glPopMatrix();
}

void keyboardup(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':
	{
		right =false;
		break;
	}
	case 'a':
	{
		left = false;
		break;
	}
	case 'w':
	{
		forward = false;
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}

	default:
	{
		break;
	}
	}
}

void idle(void)
{
	glutPostRedisplay();
}

GLuint loadTexture(char* name, GLfloat width, GLfloat height)
{
	return glmLoadTexture(name, false, true, true, true, &width, &height);
}
