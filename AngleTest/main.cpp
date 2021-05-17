#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <direct.h> // _getcwd
#include <thread>
#pragma once
//環境設定
#define OFFSCREEN //これでオフスクリーンレンダリング
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

typedef struct {
	// レンダリング用シェーダープログラム
	GLuint shader_program;

	// 位置情報属性
	GLint attr_pos;

	// UV情報属性
	GLint attr_uv;

	// テクスチャのUniform
	GLint unif_texture;

	// 読み込んだテクスチャ
	GLuint texture_id;

} Extension_LoadTexture;

namespace
{
	constexpr int WindowWidth = 960;
	constexpr int WindowHeight = 540;
	const char* AppTitle = "OpenGLES2";

	int count = 0;
	SYSTEMTIME tm1,tm2;
	// 図形データ
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
	FILE *outputfile;         // 出力ストリーム

	outputfile = fopen("angletest_dbg.txt", "a");  // ファイルを書き込み用にオープン(開く)
	if (outputfile == NULL) {          // オープンに失敗した場合
		printf("cannot open\n");         // エラーメッセージを出して
		exit(1);                         // 異常終了
	}

	fprintf(outputfile, "%s", str); // ファイルに書く
	fclose(outputfile);          // ファイルをクローズ(閉じる)
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

	/*BGRをRGBに補正する*/
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
int idx=0;
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

	// 更新
	glViewport(0, 0, WindowWidth, WindowHeight);
	glClearColor(0.f + (count % 96) / 100.0f, 0.5f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

#if 1
	//三角形を動かす
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
	/* テクスチャ描画 */
	glUseProgram(extension->shader_program);
	glEnableVertexAttribArray(extension->attr_pos);
	glEnableVertexAttribArray(extension->attr_uv);
	// unif_textureへテクスチャを設定する
	glUniform1i(extension->unif_texture, 0);
	// 四角形描画
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
	//これの有無でどう変わるか
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
		MessageBox(NULL, wlocal, TEXT("メッセージボックス"), MB_OK);
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
	window = glfwCreateWindow(WindowWidth, WindowHeight, AppTitle, nullptr, nullptr);
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
	//Extension_LoadTexture* extension = new Extension_LoadTexture();
	extension = new Extension_LoadTexture();

//	int count = 0;
//	SYSTEMTIME tm,tm2;
	GetLocalTime(&tm1);

	// バーテックスシェーダのソースプログラム
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


	// フラグメントシェーダのソースプログラム
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

	// プログラムオブジェクトの作成
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

	// uniformを取り出す
	const char* uniform_name = "unif_texture";
	extension->unif_texture = glGetUniformLocation(extension->shader_program, uniform_name);
	if (extension->unif_texture == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", uniform_name);
		return 0;
	}

	// テクスチャの生成を行う
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
	// 図形データ
	GLfloat triangle_vertices[][2] = {
		{ 0.0f,  1.0f},
		{-0.8f, -0.8f},
		{ 0.8f, -0.8f},
	};

	while (glfwWindowShouldClose(window) == GLFW_FALSE)
	{
		glfwPostEmptyEvent();

		// 更新
		glViewport(0, 0, WindowWidth, WindowHeight);		
		glClearColor(0.f + (count % 96) / 100.0f, 0.5f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

#if 1
		//三角形を動かす
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
		/* テクスチャ描画 */
		glUseProgram(extension->shader_program);
		glEnableVertexAttribArray(extension->attr_pos);
		glEnableVertexAttribArray(extension->attr_uv);
		// unif_textureへテクスチャを設定する
		glUniform1i(extension->unif_texture, 0);
		// 四角形描画
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
		//これの有無でどう変わるか
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
			MessageBox(NULL , wlocal, TEXT("メッセージボックス"), MB_OK);
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
