#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#define KEY_COUNT		256

#define COLOR_BITS		24
#define DEPTH_BITS		16
#define STENCIL_BITS	8

#define WND_WIDTH		800
#define WND_HEIGHT		600
#define WND_CLASSNAME	L"OpenGL_WND"
#define WND_TITLE		L"OpenGL Window"

#define PROMPT_FULLSCREEN 0

HGLRC		hRC = nullptr;
HDC			hDC = nullptr;
HWND		hWnd = nullptr;
HINSTANCE	hInstance;

bool keys[KEY_COUNT];
bool active = true;
bool fullscreen = false;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

GLvoid resize_gl_scene(GLsizei width, GLsizei height)
{
	height = (height == 0) ? 1 : height;
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int init_gl(GLvoid)
{
	glShadeModel(GL_SMOOTH);

	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	return TRUE;
}

int draw_gl_scene(GLvoid)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(-1.5f, 0.0f, -6.0f);
	glBegin(GL_TRIANGLES);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.0f, 0.0f);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();

	glTranslatef(3.0f, 0.0f, 0.0f);
	glColor3f(0.5f, 0.5f, 1.0f);
	glBegin(GL_QUADS);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
	glEnd();

	return TRUE;
}

GLvoid kill_gl_window(GLvoid)
{
	if (fullscreen)
	{
		ChangeDisplaySettings(nullptr, 0);
		ShowCursor(true);
	}

	if (hRC)
	{
		if (!wglMakeCurrent(nullptr, nullptr))
			MessageBox(nullptr, L"Release of DC and RC failed", L"Shutdown Error", MB_OK | MB_ICONINFORMATION);
		if (!wglDeleteContext(hRC))
			MessageBox(nullptr, L"Release of rendering context Failed", L"Shutdown Error", MB_OK | MB_ICONINFORMATION);

		hRC = nullptr;
	}

	if (hDC && !ReleaseDC(hWnd, hDC))
	{
		MessageBox(nullptr, L"Release of DC Failed", L"Shutdown Error", MB_OK | MB_ICONINFORMATION);
		hDC = nullptr;
	}

	if (hWnd && !DestroyWindow(hWnd))
	{
		MessageBox(nullptr, L"Could not destroy window", L"Shutdown Error", MB_OK | MB_ICONINFORMATION);
		hWnd = nullptr;
	}

	if (!UnregisterClass(WND_CLASSNAME, hInstance))
	{
		MessageBox(nullptr, L"Could not unregister window class", L"Shutdown Error", MB_OK | MB_ICONINFORMATION);
		hInstance = nullptr;
	}
}

BOOL create_gl_window(LPCWSTR title, int width, int height, int bits, bool is_fullscreen)
{
	GLuint pixel_format;
	WNDCLASS wc;

	DWORD dwExStyle;
	DWORD dwStyle;

	RECT windowRect;
	windowRect.left = 0L;
	windowRect.right = (long)width;
	windowRect.top = 0L;
	windowRect.bottom = (long)height;

	fullscreen = is_fullscreen;

	hInstance = GetModuleHandle(nullptr);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = WND_CLASSNAME;

	if (!RegisterClass(&wc))
	{
		MessageBox(nullptr, L"Could not register window class", L"Create window Error", MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	if (fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(DEVMODE));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = width;
		dmScreenSettings.dmPelsHeight = height;
		dmScreenSettings.dmBitsPerPel = bits;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(nullptr, L"The requested fullscreen mode not supported by\nyour video card.\nUse windowed mode instead?",
				L"Create window Error", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				fullscreen = false;
			else
			{
				MessageBox(nullptr, L"Program will now close", L"Create window Error", MB_OK | MB_ICONSTOP);
				return FALSE;
			}
		}
	}

	if (fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(FALSE);
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	/* Creating window */
	if (!(hWnd = CreateWindowEx(dwExStyle,
		WND_CLASSNAME,
		title,
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0UL, 0UL,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr)))
	{
		kill_gl_window();
		MessageBox(nullptr, L"Window creation failed", L"Create window Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	/* Pixel Format Descriptor */

	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		bits,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		DEPTH_BITS,		// 32-bit Z-buffer (depth buffer)
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	if (!(hDC = GetDC(hWnd)))
	{
		kill_gl_window();
		MessageBox(nullptr, L"Can't create a GL Device Context.", L"Create window Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(pixel_format = ChoosePixelFormat(hDC, &pfd)))
	{
		kill_gl_window();
		MessageBox(nullptr, L"Can't find suitable pixel format.", L"Create window Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!SetPixelFormat(hDC, pixel_format, &pfd))
	{
		kill_gl_window();
		MessageBox(nullptr, L"Can't set pixel format.", L"Create window Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(hRC = wglCreateContext(hDC)))
	{
		kill_gl_window();
		MessageBox(nullptr, L"Can't create GL Rendering Context.", L"Create window Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!wglMakeCurrent(hDC, hRC))
	{
		kill_gl_window();
		MessageBox(nullptr, L"Can't activate GL Rendering Context.", L"Create window Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	resize_gl_scene((GLsizei)width, (GLsizei)height);

	if (!init_gl())
	{
		kill_gl_window();
		MessageBox(nullptr, L"Initialization failed", L"Create window Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_ACTIVATE:
		active = !HIWORD(wParam) ? true : false;
		return 0L;
	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_SCREENSAVE:
		case SC_MONITORPOWER:
			return 0L;
		}
		break;
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0L;
	case WM_KEYDOWN:
		keys[wParam] = true;
		return 0L;
	case WM_KEYUP:
		keys[wParam] = false;
		return 0L;
	case WM_SIZE:
		resize_gl_scene((GLsizei)LOWORD(lParam), (GLsizei)HIWORD(lParam));
		return 0L;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	MSG msg;
	bool done = false;


#if PROMPT_FULLSCREEN
	fullscreen = MessageBox(nullptr,
		L"Do you want run application in fullscreen mode?",
		L"Is fullscreen mode", MB_YESNO | MB_ICONQUESTION) == IDNO ? false : true;
#endif

	if (!create_gl_window(WND_TITLE, WND_WIDTH, WND_HEIGHT, COLOR_BITS, fullscreen))
		return 0;

	while (!done)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				done = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if ((active && !draw_gl_scene()) || keys[VK_ESCAPE])
				done = true;
			else
				SwapBuffers(hDC);

			if (keys[VK_F1])
			{
				keys[VK_F1] = false;
				kill_gl_window();
				fullscreen = !fullscreen;
				if (!create_gl_window(WND_TITLE, WND_WIDTH, WND_HEIGHT, COLOR_BITS, fullscreen))
					return 0;
			}
		}
	}

	kill_gl_window();
	return (msg.wParam);
}