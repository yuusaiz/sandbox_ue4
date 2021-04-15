#include <Windows.h>
#include <iostream>
#include <vector>
#include <array>
#include <stdio.h>

#define GLFW_INCLUDE_ES2
#define GL_GLEXT_PROTOTYPES
#define GL_ES_VERSION_2_0
#include <GL/glew.h>
#include <GL/glut.h>
//#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include "VertexAttribArrayHelper.h"
#if defined(GLFW_INCLUDE_ES2)
#   pragma comment(lib, "libEGL.dll.lib")
#   pragma comment(lib, "libGLESv2.dll.lib")
#else
#   pragma comment(lib, "opengl32.lib")
#endif
using namespace std;

//#define USE_VertexAttribArrayHelper
#define TUTORIAL1
//#define USE_GLFW

GLuint program;
GLint attribute_coord2d;
namespace
{
	constexpr int WindowWidth = 800;
	constexpr int WindowHeight = 600;
	const char* AppTitle = "OpenGLES2";
}

struct attributes {
	GLfloat coord3d[3];
	GLfloat v_color[3];
};
int init_resources(void)
{
	GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	const char *vs_source =
#ifdef GL_ES_VERSION_2_0
		"#version 100\n"  // OpenGL ES 2.0
#else
		"#version 120\n"  // OpenGL 2.1
#endif
		"attribute vec2 coord2d;                  "
		"void main(void) {                        "
		"  gl_Position = vec4(coord2d, 0.0, 1.0); "
		"}";
	glShaderSource(vs, 1, &vs_source, NULL);
	glCompileShader(vs);
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_ok);
	if (0 == compile_ok)
	{
		fprintf(stderr, "Error in vertex shader\n");
		return 0;
	}

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fs_source =
#ifdef GL_ES_VERSION_2_0
		"#version 100\n"  // OpenGL ES 2.0
#else
		"#version 120\n"  // OpenGL 2.1
#endif
		"void main(void) {        "
		"  gl_FragColor[0] = 0.0; "
		"  gl_FragColor[1] = 0.0; "
		"  gl_FragColor[2] = 1.0; "
		"}";
	glShaderSource(fs, 1, &fs_source, NULL);
	glCompileShader(fs);
	glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_ok);
	if (!compile_ok) {
		fprintf(stderr, "Error in fragment shader\n");
		return 0;
	}

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram:");
		return 0;
	}

	const char* attribute_name = "coord2d";
	attribute_coord2d = glGetAttribLocation(program, attribute_name);
	if (attribute_coord2d == -1) {
		fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
		return 0;
	}

	return 1;
}
void onDisplay()
{
	/* Clear the background as white */
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glEnableVertexAttribArray(attribute_coord2d);
	GLfloat triangle_vertices[] = {
	   0.0,  0.8,
	  -0.8, -0.8,
	   0.8, -0.8,
	};
	/* Describe our vertices array to OpenGL (it can't guess its format automatically) */
	glVertexAttribPointer(
		attribute_coord2d, // attribute
		2,                 // number of elements per vertex, here (x,y)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		triangle_vertices  // pointer to the C array
	);

	/* Push each element in buffer_vertices to the vertex shader */
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(attribute_coord2d);

	/* Display the result */
//	glutSwapBuffers(); // ??

}
void free_resources()
{
	glDeleteProgram(program);
}
int ic(int a)
{
	return a;
}
int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#if defined (USE_GLFW)
	if (glfwInit() == GL_FALSE)
	{
		return -1;
	}

	// エラーコールバック
	glfwSetErrorCallback([](int error, const char* description) {
		std::cout << "glfw err[" << error << "] " << description << std::endl;
		fprintf(stderr, "err[%d] %s\n", error, description);
	});

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
#endif
	int count = 0;

#ifdef USE_VertexAttribArrayHelper //よくわからんのでpending
	int vshader_position_index = 0;
	int vshader_color_index = 0;
	int vshader_texcoord_index = 0;
	int vshader_texcoord_index2 = 0;
	// vaah の生成と構築
	VertexAttribArrayHelper vaah(vshader_position_index, 0, vshader_color_index, vshader_texcoord_index, vshader_texcoord_index2, 0);
	auto& gl = vaah;
	array<float, 10> _tile{ 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
	vector<pair<float, float>> _section_tips{};
	for (int i = 0; i <= 1; ++i)
	{
		gl.Begin(GL_TRIANGLE_STRIP);
		const auto& ipr = _section_tips[ic(i)];
		const auto& iipr = _section_tips[ic(i + 1)];
		const float c = 1.0;
		gl.Color4f(c, c, c, 1.0);
		gl.TexCoord2f(0.0, 1.0);
		gl.Vertex3fv(glm::value_ptr(iipr.first));
		gl.TexCoord2f(1.0, 1.0);
		gl.Vertex3fv(glm::value_ptr(iipr.second));
		gl.TexCoord2f(0.0, 0.0);
		gl.Vertex3fv(glm::value_ptr(ipr.first));
		gl.TexCoord2f(1.0, 0.0);
		gl.Vertex3fv(glm::value_ptr(ipr.second));
		gl.End();
	}// ipositions
	// vaah のコンパイル（VBO,VAOの生成）
	gl.Compile();

#endif


#if defined(USE_GLFW)
	while (glfwWindowShouldClose(window) == GLFW_FALSE)
	{
		glfwPostEmptyEvent();

		// 更新
		glViewport(0, 0, WindowWidth, WindowHeight);
		
		glClearColor(0.f + (count % 100) / 100.0, 0.5f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);


		glfwWaitEvents();
		count++;
	}

	glfwDestroyWindow(window);
	glfwTerminate();
#endif

#if defined(TUTORIAL1)
	/* Glut-related initialising functions */
	glutInit( &__argc, __argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	//glutInitWindowSize(640, 480);
	glutInitWindowSize(1280, 480);
	glutCreateWindow("My First Triangle");

	/* Extension wrangler initialising */
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return EXIT_FAILURE;
	}

	/* When all init functions runs without errors,
	the program can initialise the resources */
	if (1 == init_resources())
	{
		/* We can display it if everything goes OK */
		glutDisplayFunc(onDisplay);
		glutMainLoop();
	}

	/* If the program exits in the usual way,
	free resources and exit with a success */
	free_resources();
	return EXIT_SUCCESS; 
#endif

	return -1;
}

