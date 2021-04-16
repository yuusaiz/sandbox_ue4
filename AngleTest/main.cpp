#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <iostream>

#define GLFW_INCLUDE_ES2 //これの有無でAngleES環境かを切り替える
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#if defined(GLFW_INCLUDE_ES2)
#   pragma comment(lib, "libEGL.dll.lib")
#   pragma comment(lib, "libGLESv2.dll.lib")
#else
#   pragma comment(lib, "opengl32.lib")
#endif

namespace
{
	constexpr int WindowWidth = 960;
	constexpr int WindowHeight = 540;
	const char* AppTitle = "OpenGLES2";
}
unsigned char buf[4 * 960 * 540];

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

	int count = 0;
	SYSTEMTIME tm,tm2;
	GetLocalTime(&tm);

	while (glfwWindowShouldClose(window) == GLFW_FALSE)
	{
		glfwPostEmptyEvent();

		// 更新
		glViewport(0, 0, WindowWidth, WindowHeight);
		
		glClearColor(0.f + (count % 100) / 100.0f, 0.5f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);

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
