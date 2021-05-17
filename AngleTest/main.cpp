#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <direct.h> // _getcwd
#include <thread>
#pragma once
//���ݒ�
#define OFFSCREEN //����ŃI�t�X�N���[�������_�����O
#define GLFW_INCLUDE_ES2 //����̗L����AngleES������؂�ւ���

#define GL_GLEXT_PROTOTYPES


#if defined(GLFW_INCLUDE_ES2)
#define  GLdouble  double
#include "gg.h"
using namespace gg;
#include <GLFW/glfw3.h>
//#include <GL/glew.h>
extern "C" {
	extern void APIENTRY glGenVertexArrays(GLsizei n, GLuint *arrays);
	extern void APIENTRY glBindVertexArray(GLuint array);
}
#pragma comment(lib, "libEGL.dll.lib")
#pragma comment(lib, "libGLESv2.dll.lib")

#else
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma comment(lib, "opengl32.lib")
#endif

#pragma comment(lib, "glew32.lib")

typedef struct {
	// �����_�����O�p�V�F�[�_�[�v���O����
	GLuint shader_program;

	// �ʒu��񑮐�
	GLint attr_pos;

	// UV��񑮐�
	GLint attr_uv;

	// �e�N�X�`����Uniform
	GLint unif_texture;

	// �ǂݍ��񂾃e�N�X�`��
	GLuint texture_id;

} Extension_LoadTexture;

namespace
{
	constexpr int WindowWidth = 960;
	constexpr int WindowHeight = 540;
	const char* AppTitle = "OpenGLES2";

	int count = 0;
	SYSTEMTIME tm1,tm2;
	// �}�`�f�[�^
	GLfloat triangle_vertices[][2] = {
		{ 0.0f,  1.0f},
		{-0.8f, -0.8f},
		{ 0.8f, -0.8f},
	};
	GLuint program;
	GLint attribute_coord2d;
	Extension_LoadTexture *extension;
	GLFWwindow *window;
}
unsigned char buf[4 * 960 * 540];

int
outstr(const char *str)
{
	FILE *outputfile;         // �o�̓X�g���[��

	outputfile = fopen("angletest_dbg.txt", "a");  // �t�@�C�����������ݗp�ɃI�[�v��(�J��)
	if (outputfile == NULL) {          // �I�[�v���Ɏ��s�����ꍇ
		printf("cannot open\n");         // �G���[���b�Z�[�W���o����
		exit(1);                         // �ُ�I��
	}

	fprintf(outputfile, "%s", str); // �t�@�C���ɏ���
	fclose(outputfile);          // �t�@�C�����N���[�Y(����)
}

unsigned char *
load_bitmap(const char *fname) {
	FILE* stream;
	int errorCode;
	unsigned char header[14];
	char fff[1000];
	outstr(__FILE__ " in");
	printf(_getcwd(fff, 1000));
	outstr(fff);
	if (NULL == (stream = fopen(fname, "r"))) {
		errorCode = errno;
		printf("file open err\n");
	}
	fread(&header[0], 1, sizeof header, stream);
	int size = header[2] | header[3]<<8 | header[4]<<16 | header[5]<<24;
	int offset = header[10] | header[11]<<8 | header[12]<<16 | header[13]<<24;

	unsigned char *buf = new unsigned char[size];
	fseek(stream, offset, SEEK_SET);
	fread(buf, 1, size, stream);

	fclose(stream);

	/*BGR��RGB�ɕ␳����*/
	for (int i=0; i < size; i+=3) {
		unsigned char tmpR;
		tmpR = buf[i+2];
		buf[i + 2] = buf[i];
		buf[i] = tmpR;
	}
	outstr(__FILE__ " out");
	return buf;
}


/*
** ������
*/

static void init(void)
{
#ifdef GLFW_INCLUDE_ES2
	//ggInit();
#endif
//#if defined(WIN32)
#ifndef GLFW_INCLUDE_ES2
	/* GLEW �̏����� */
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		char buf[256];
		sprintf(buf, (char*)glewGetErrorString(err));
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
#endif

	/* �w�i�F */
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

// �V�F�[�_�I�u�W�F�N�g�̃R���p�C�����ʂ�\������
static GLboolean printShaderInfoLog(GLuint shader, const char *str)
{
	// �R���p�C�����ʂ��擾����
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;

	// �V�F�[�_�̃R���p�C�����̃��O�̒������擾����
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		// �V�F�[�_�̃R���p�C�����̃��O�̓��e���擾����
		GLchar *infoLog = new GLchar[bufSize];
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, infoLog);
		std::cerr << infoLog << std::endl;
		delete[] infoLog;
	}

	return (GLboolean)status;
}
// �v���O�����I�u�W�F�N�g�̃����N���ʂ�\������
static GLboolean printProgramInfoLog(GLuint program)
{
	// �����N���ʂ��擾����
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "Link Error" << std::endl;

	// �V�F�[�_�̃����N���̃��O�̒������擾����
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		// �V�F�[�_�̃����N���̃��O�̓��e���擾����
		GLchar *infoLog = new GLchar[bufSize];
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, infoLog);
		std::cerr << infoLog << std::endl;
		delete[] infoLog;
	}

	return (GLboolean)status;
}
// �v���O�����I�u�W�F�N�g�̍쐬
int idx=0;
static GLuint createProgram(const char *vsrc, const char *pv, const char *fsrc, const char *fc)
{
	// �o�[�e�b�N�X�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g
	GLuint vobj = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vobj, 1, &vsrc, NULL);
	glCompileShader(vobj);
	printShaderInfoLog(vobj, "vertex shader");

	// �t���O�����g�V�F�[�_�̃V�F�[�_�I�u�W�F�N�g�̍쐬
	GLuint fobj = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fobj, 1, &fsrc, NULL);
	glCompileShader(fobj);
	printShaderInfoLog(fobj, "fragment shader");

	// �V�F�[�_�I�u�W�F�N�g�̎��t��
	GLuint program = glCreateProgram();
	glAttachShader(program, vobj);
	glDeleteShader(vobj);
	glAttachShader(program, fobj);
	glDeleteShader(fobj);

	// �v���O�����I�u�W�F�N�g�̃����N
	glBindAttribLocation(program, idx, pv);
	//glBindFragDataLocation(program, 0, fc);
	glLinkProgram(program);
	printProgramInfoLog(program);

	idx++;
	return program;
}

extern "C"{
int AngleMain(int);
int  AngleMain_in(int a);
}
#ifndef MAKELIB
int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	AngleMain(1);
}
#endif

int
AngleMainLoop_in(int a)
{
	glfwPostEmptyEvent();

	// �X�V
	glViewport(0, 0, WindowWidth, WindowHeight);
	glClearColor(0.f + (count % 96) / 100.0f, 0.5f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

#if 1
	//�O�p�`�𓮂���
	float tmpx = 0.0013;
	float tmp;
	triangle_vertices[0][0] += tmpx;
	if (1 < triangle_vertices[0][0]) triangle_vertices[0][0] = -1;

	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord2d);

	/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
	glVertexAttribPointer(
		attribute_coord2d, // attribute
		2,                 // number of elements per vertex, here (x,y)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		triangle_vertices  // pointer to the C array
	);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(attribute_coord2d);
	glUseProgram(0);
#endif

#if 1
	/* �e�N�X�`���`�� */
	glUseProgram(extension->shader_program);
	glEnableVertexAttribArray(extension->attr_pos);
	glEnableVertexAttribArray(extension->attr_uv);
	// unif_texture�փe�N�X�`����ݒ肷��
	glUniform1i(extension->unif_texture, 0);
	// �l�p�`�`��
	{
		const GLfloat position[] = {
			// v0(left top)
					-0.75f, 0.75f,
					// v1(left bottom)
					-0.75f, -0.75f,
					// v2(right top)
					0.75f, 0.75f,
					// v3(right bottom)
					0.75f, -0.75f, };

		const GLfloat uv[] = {
			// v0(left top)
					0, 0,
					// v1(left bottom)
					0, 1,
					// v2(right top)
					1, 0,
					// v3(right bottom)
					1, 1, };

		glVertexAttribPointer(extension->attr_pos, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)position);
		glVertexAttribPointer(extension->attr_uv, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)uv);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
#endif

#if !defined(OFFSCREEN)
	glfwSwapBuffers(window);
#endif
	//����̗L���łǂ��ς�邩
	glReadPixels(0, 0, WindowWidth, WindowHeight, GL_RGBA, GL_UNSIGNED_BYTE, buf);

	glfwWaitEvents();
	count++;
	if (count % 1000 == 0) {
		GetLocalTime(&tm2);
		int millisec = (tm2.wSecond - tm1.wSecond) * 1000 + (tm2.wMilliseconds - tm1.wMilliseconds);
		char str[256];
		int idx = (960 * 50 + 200) * 4;
		sprintf(str, "1000frame / %d msec: %.2f fps\nbuf[%d]=%d %d %d %d", millisec, 1000.0* 1000.0 / millisec, idx, buf[idx], buf[idx + 1], buf[idx + 2], buf[idx + 3]);
		wchar_t wlocal[256];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, 256, wlocal, 256);
		MessageBox(NULL, wlocal, TEXT("���b�Z�[�W�{�b�N�X"), MB_OK);
		GetLocalTime(&tm1);
	}
	return 0;
}


int
AngleMain(int wait)
{
	AngleMain_in(1);
	while (true){
		AngleMainLoop_in(1);
	}
	return 0;
}

int
AngleMain_in(int a){
	if (glfwInit() == GL_FALSE)
	{
		return -1;
	}

	// �G���[�R�[���o�b�N
	glfwSetErrorCallback([](int error, const char* description) {
		std::cout << "glfw err[" << error << "] " << description << std::endl;
		fprintf(stderr, "err[%d] %s\n", error, description);
	});

#if defined(GLFW_INCLUDE_ES2)
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#if defined(OFFSCREEN)
	glfwWindowHint(GLFW_VISIBLE, 0);
	//glfwWindowHint(GLFW_REFRESH_RATE, 300);
#endif


	// �E�B���h�E�̐���
	window = glfwCreateWindow(WindowWidth, WindowHeight, AppTitle, nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// �L�[���̓R�[���o�b�N
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	});

	// OpenGL�̏����Ώۂɂ���
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	init();
	//Extension_LoadTexture* extension = new Extension_LoadTexture();
	extension = new Extension_LoadTexture();

//	int count = 0;
//	SYSTEMTIME tm,tm2;
	GetLocalTime(&tm1);

	// �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O����
	static const GLchar vsrc[] =
//#ifdef GLFW_INCLUDE_ES2
		"#version 100\n"  // OpenGL ES 2.0
//#else
//		"#version 120\n"  // OpenGL 2.1
//#endif
		"attribute vec2 coord2d;                  "
		"void main(void) {                        "
		"  gl_Position = vec4(coord2d, 0.0, 1.0); "
		"}";

	static const GLchar vsrc2[] =
		//#ifdef GLFW_INCLUDE_ES2
		"#version 100\n"  // OpenGL ES 2.0
//#else
//		"#version 120\n"  // OpenGL 2.1
//#endif
		"attribute mediump vec4 attr_pos;"
		"attribute mediump vec2 attr_uv;"
		"varying mediump vec2 vary_uv;"

		"void main(void) {                        "
		"   gl_Position = attr_pos;"
		"   vary_uv = attr_uv;"

		"}";


	// �t���O�����g�V�F�[�_�̃\�[�X�v���O����
	static const GLchar fsrc[] =
//#ifdef GLFW_INCLUDE_ES2
		"#version 100\n"  // OpenGL ES 2.0
//#else
//		"#version 120\n"  // OpenGL 2.1
//#endif
		"void main(void) {        "
		"  gl_FragColor[0] = 0.1; "
		"  gl_FragColor[1] = 0.5; "
		"  gl_FragColor[2] = 1.0; "
		"}";

	static const GLchar fsrc2[] =
		//#ifdef GLFW_INCLUDE_ES2
		"#version 100\n"  // OpenGL ES 2.0
//#else
//		"#version 120\n"  // OpenGL 2.1
//#endif
		"uniform sampler2D unif_texture;"
		"varying mediump vec2 vary_uv;"
		"void main(void) {        "
		"   gl_FragColor = texture2D(unif_texture, vary_uv);"
		"}";

	// �v���O�����I�u�W�F�N�g�̍쐬
	//GLuint program = createProgram(vsrc, "pv", fsrc, "fc");
	program = createProgram(vsrc, "pv", fsrc, "fc");
	//GLint attribute_coord2d;

	const char* attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}


	extension->shader_program = createProgram(vsrc2, "pv2", fsrc2, "fc2");
	attribute_name = "attr_pos";
	extension->attr_pos = glGetAttribLocation(extension->shader_program, attribute_name);
	if (extension->attr_pos == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}
	attribute_name = "attr_uv";
	extension->attr_uv = glGetAttribLocation(extension->shader_program, attribute_name);
	if (extension->attr_uv == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}

	// uniform�����o��
	const char* uniform_name = "unif_texture";
	extension->unif_texture = glGetUniformLocation(extension->shader_program, uniform_name);
	if (extension->unif_texture == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", uniform_name);
		return 0;
	}

	// �e�N�X�`���̐������s��
	glGenTextures(1, &extension->texture_id);
	if (extension->texture_id == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", uniform_name);
		return 0;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glBindTexture(GL_TEXTURE_2D, extension->texture_id);
	if (glGetError() != GL_NO_ERROR) {
		fprintf(stderr, "glGetError \n");
		return 0;
	}
	//unsigned char *pixel_data = new unsigned char[100 * 100 * 4];
	//unsigned char *pixel_data=load_bitmap("Bitmap.bmp");
	unsigned char *pixel_data = new unsigned char[700 * 500 * 4];

	memset(pixel_data + 2154 * 4, 200, 500 * 4);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 700, 500, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 700, 500, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel_data);
	delete pixel_data;
	if (glGetError() != GL_NO_ERROR) {
		fprintf(stderr, "glGetError \n");
		return 0;
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

#if 0
	// �}�`�f�[�^
	GLfloat triangle_vertices[][2] = {
		{ 0.0f,  1.0f},
		{-0.8f, -0.8f},
		{ 0.8f, -0.8f},
	};

	while (glfwWindowShouldClose(window) == GLFW_FALSE)
	{
		glfwPostEmptyEvent();

		// �X�V
		glViewport(0, 0, WindowWidth, WindowHeight);		
		glClearColor(0.f + (count % 96) / 100.0f, 0.5f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

#if 1
		//�O�p�`�𓮂���
		float tmpx = 0.0013;
		float tmp;
		triangle_vertices[0][0] += tmpx;
		if (1 < triangle_vertices[0][0]) triangle_vertices[0][0] = -1;

		glUseProgram(program);
		glEnableVertexAttribArray(attribute_coord2d);

		/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
		glVertexAttribPointer(
			attribute_coord2d, // attribute
			2,                 // number of elements per vertex, here (x,y)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			triangle_vertices  // pointer to the C array
		);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(attribute_coord2d);
		glUseProgram(0);
#endif

#if 1
		/* �e�N�X�`���`�� */
		glUseProgram(extension->shader_program);
		glEnableVertexAttribArray(extension->attr_pos);
		glEnableVertexAttribArray(extension->attr_uv);
		// unif_texture�փe�N�X�`����ݒ肷��
		glUniform1i(extension->unif_texture, 0);
		// �l�p�`�`��
		{
			const GLfloat position[] = {
				// v0(left top)
						-0.75f, 0.75f,
						// v1(left bottom)
						-0.75f, -0.75f,
						// v2(right top)
						0.75f, 0.75f,
						// v3(right bottom)
						0.75f, -0.75f, };

			const GLfloat uv[] = {
				// v0(left top)
						0, 0,
						// v1(left bottom)
						0, 1,
						// v2(right top)
						1, 0,
						// v3(right bottom)
						1, 1, };

			glVertexAttribPointer(extension->attr_pos, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)position);
			glVertexAttribPointer(extension->attr_uv, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)uv);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
#endif

#if !defined(OFFSCREEN)
		glfwSwapBuffers(window);
#endif
		//����̗L���łǂ��ς�邩
		glReadPixels(0, 0, WindowWidth, WindowHeight, GL_RGBA, GL_UNSIGNED_BYTE, buf);

		glfwWaitEvents();
		count++;
		if (count % 1000 == 0) {
			GetLocalTime(&tm2);
			int millisec = (tm2.wSecond - tm.wSecond) * 1000 + (tm2.wMilliseconds - tm.wMilliseconds);
			char str[256];
			int idx = (960 * 50 + 200) * 4;
			sprintf(str, "1000frame / %d msec: %.2f fps\nbuf[%d]=%d %d %d %d", millisec, 1000.0* 1000.0/millisec, idx, buf[idx], buf[idx+1], buf[idx+2], buf[idx+3]);
			wchar_t wlocal[256];
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, 256, wlocal, 256);
			MessageBox(NULL , wlocal, TEXT("���b�Z�[�W�{�b�N�X"), MB_OK);
			GetLocalTime(&tm);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return -1;
#else
	return 0;
#endif
}
