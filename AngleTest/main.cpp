#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <iostream>

#pragma once
//環境設定
//#define OFFSCREEN //これでオフスクリーンレンダリング
#define GLFW_INCLUDE_ES2 //これの有無でAngleES環境かを切り替える

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




namespace
{
	constexpr int WindowWidth = 960;
	constexpr int WindowHeight = 540;
	const char* AppTitle = "OpenGLES2";
}
unsigned char buf[4 * 960 * 540];

/*
** 初期化
*/

static void init(void)
{
#ifdef GLFW_INCLUDE_ES2
	//ggInit();
#endif
//#if defined(WIN32)
#ifndef GLFW_INCLUDE_ES2
	/* GLEW の初期化 */
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		char buf[256];
		sprintf(buf, (char*)glewGetErrorString(err));
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(1);
	}
#endif

	/* 背景色 */
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

// シェーダオブジェクトのコンパイル結果を表示する
static GLboolean printShaderInfoLog(GLuint shader, const char *str)
{
	// コンパイル結果を取得する
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "Compile Error in " << str << std::endl;

	// シェーダのコンパイル時のログの長さを取得する
	GLsizei bufSize;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		// シェーダのコンパイル時のログの内容を取得する
		GLchar *infoLog = new GLchar[bufSize];
		GLsizei length;
		glGetShaderInfoLog(shader, bufSize, &length, infoLog);
		std::cerr << infoLog << std::endl;
		delete[] infoLog;
	}

	return (GLboolean)status;
}
// プログラムオブジェクトのリンク結果を表示する
static GLboolean printProgramInfoLog(GLuint program)
{
	// リンク結果を取得する
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) std::cerr << "Link Error" << std::endl;

	// シェーダのリンク時のログの長さを取得する
	GLsizei bufSize;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufSize);

	if (bufSize > 1)
	{
		// シェーダのリンク時のログの内容を取得する
		GLchar *infoLog = new GLchar[bufSize];
		GLsizei length;
		glGetProgramInfoLog(program, bufSize, &length, infoLog);
		std::cerr << infoLog << std::endl;
		delete[] infoLog;
	}

	return (GLboolean)status;
}
// プログラムオブジェクトの作成
static GLuint createProgram(const char *vsrc, const char *pv, const char *fsrc, const char *fc)
{
	// バーテックスシェーダのシェーダオブジェクト
	GLuint vobj = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vobj, 1, &vsrc, NULL);
	glCompileShader(vobj);
	printShaderInfoLog(vobj, "vertex shader");

	// フラグメントシェーダのシェーダオブジェクトの作成
	GLuint fobj = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fobj, 1, &fsrc, NULL);
	glCompileShader(fobj);
	printShaderInfoLog(fobj, "fragment shader");

	// シェーダオブジェクトの取り付け
	GLuint program = glCreateProgram();
	glAttachShader(program, vobj);
	glDeleteShader(vobj);
	glAttachShader(program, fobj);
	glDeleteShader(fobj);

	// プログラムオブジェクトのリンク
	glBindAttribLocation(program, 0, pv);
	//glBindFragDataLocation(program, 0, fc);
	glLinkProgram(program);
	printProgramInfoLog(program);

	return program;
}
// 頂点配列オブジェクトの作成
static GLuint createObject(GLuint vertices, const GLfloat(*position)[2])
{
	// 頂点配列オブジェクト
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// 頂点バッファオブジェクト
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * vertices, position, GL_STATIC_DRAW);

	// 結合されている頂点バッファオブジェクトを attribute 変数から参照できるようにする
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 頂点バッファオブジェクトと頂点配列オブジェクトの結合を解除する
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}
int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (glfwInit() == GL_FALSE)
	{
		return -1;
	}

	// エラーコールバック
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


	// ウィンドウの生成
	auto window = glfwCreateWindow(WindowWidth, WindowHeight, AppTitle, nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// キー入力コールバック
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	});

	// OpenGLの処理対象にする
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	init();

	int count = 0;
	SYSTEMTIME tm,tm2;
	GetLocalTime(&tm);

	// バーテックスシェーダのソースプログラム
	static const GLchar vsrc[] =
#ifndef GLFW_INCLUDE_ES2
		"#version 150 core\n"
		"in vec4 pppv;\n"
#else
		"#version 100\n"
		"attribute mediump vec4 pppv;\n"
#endif
		"void main(void)\n"
		"{\n"
		"  gl_Position = pppv;\n"
		"}\n";

	// フラグメントシェーダのソースプログラム
	static const GLchar fsrc[] =
#ifndef GLFW_INCLUDE_ES2
		"#version 150 core\n"
		"out vec4 fffc;\n"
#else
		"#version 100\n"
		"mediump vec4 fffc;\n"
#endif
		"void main(void)\n"
		"{\n"
		"  fffc = vec4(1.0, 0.0, 0.0, 0.0);\n"
		"}\n";

	// プログラムオブジェクトの作成
	GLuint program = createProgram(vsrc, "pv", fsrc, "fc");

	// 図形データ
	static const GLfloat position[][2] =
	{
	  { -0.5f, -0.5f },
	  {  0.5f, -0.5f },
	  {  0.5f,  0.5f },
	  { -0.5f,  0.5f }
	};
	static const int vertices = sizeof position / sizeof position[0];

	// 頂点配列オブジェクトの作成
	GLuint vao = createObject(vertices, position);
	   
	while (glfwWindowShouldClose(window) == GLFW_FALSE)
	{
		glfwPostEmptyEvent();

		// 更新
		glViewport(0, 0, WindowWidth, WindowHeight);		
		glClearColor(0.f + (count % 100) / 100.0f, 0.5f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);

		// 図形の描画
		glBindVertexArray(vao);
		glDrawArrays(GL_LINE_LOOP, 0, vertices);
		//glBindVertexArray(0);

#if !defined(OFFSCREEN)
		glfwSwapBuffers(window);
#endif
		//これの有無でどう変わるか
		glReadPixels(0, 0, WindowWidth, WindowHeight, GL_RGBA, GL_UNSIGNED_BYTE, buf);

		glfwWaitEvents();
		count++;
		if (count % 1000 == 0) {
			GetLocalTime(&tm2);
			int millisec = (tm2.wSecond - tm.wSecond) * 1000 + (tm2.wMilliseconds - tm.wMilliseconds);
			char str[256];
			sprintf(str, "1000frame / %d msec: %.2f fps\nbuf[0]=%d %d %d %d", millisec, 1000.0* 1000.0/millisec, buf[0], buf[1], buf[2], buf[3]);
			wchar_t wlocal[256];
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, 256, wlocal, 256);
			MessageBox(NULL , wlocal, TEXT("メッセージボックス"), MB_OK);
			GetLocalTime(&tm);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return -1;
}
