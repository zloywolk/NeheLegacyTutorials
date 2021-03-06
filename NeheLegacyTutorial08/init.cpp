#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "libimage.h"
#include "tga_image.h"
#include "safe_mem.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#define KEY_COUNT		256
#define TEXTURES_COUNT	3

#define COLOR_BITS		32
#define DEPTH_BITS		16
#define STENCIL_BITS	8

#define WND_WIDTH		800
#define WND_HEIGHT		600
#define WND_CLASSNAME	L"OpenGL_WND"
#define WND_TITLE		L"OpenGL Window"

#define PROMPT_FULLSCREEN	1
#define ROTATE_ON			0
#define DRAW_BOTTOM_SIDE	0

HGLRC		hRC = nullptr;
HDC			hDC = nullptr;
HWND		hWnd = nullptr;
HINSTANCE	hInstance;

bool keys[KEY_COUNT];
bool active = true;
bool fullscreen = false;

bool blend = false;
bool light = false;
bool l_press = false;
bool f_press = false;
bool b_press = false;

GLfloat x_rot = 30.0f;
GLfloat y_rot = 60.0f;

GLfloat x_speed = 0.0f;
GLfloat y_speed = 0.0f;

GLfloat z_depth = -5.0f;

GLfloat light_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// ambient color
GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };	// Diffuse color
GLfloat light_position[] = { 0.0f, 0.0f, 2.0f, 1.0f };	// light position

/* Texture filter types */
#define TEXTURE_FILTER_NEAREST		0
#define TEXTURE_FILTER_LINEAR		1
#define TEXTURE_FILTER_MIPMAPED	2

GLuint filter = TEXTURE_FILTER_NEAREST;
GLuint textures[TEXTURES_COUNT];

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int init_gl_texture()
{
	image::tga_image texture01("textures/Glass.tga");
	err_t err_code = texture01.load();

	if (!err_code)
	{
		glGenTextures(TEXTURES_COUNT, &textures[0]);

		glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FILTER_NEAREST]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			texture01.bpp() / 8,
			texture01.width(), texture01.height(),
			0,
			GL_RGB, GL_UNSIGNED_BYTE,
			reinterpret_cast<const void *>(texture01.content()));

		glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FILTER_LINEAR]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			texture01.bpp() / 8,
			texture01.width(), texture01.height(),
			0,
			GL_RGB, GL_UNSIGNED_BYTE,
			reinterpret_cast<const void *>(texture01.content()));

		glBindTexture(GL_TEXTURE_2D, textures[TEXTURE_FILTER_MIPMAPED]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

		gluBuild2DMipmaps(GL_TEXTURE_2D,
			texture01.bpp() / 8,
			texture01.width(),
			texture01.height(),
			GL_RGB,
			GL_UNSIGNED_BYTE,
			reinterpret_cast<const void*>(texture01.content()));

		glEnable(GL_TEXTURE_2D);
	}

	return !err_code;
}

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

	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	return TRUE;
}

int draw_gl_scene(GLvoid)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, z_depth);


	glRotatef(x_rot, 1.0f, 0.0f, 0.0f);
	glRotatef(y_rot, 0.0f, 1.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, textures[filter]);

	glBegin(GL_QUADS);
		// Front
		glNormal3f(0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);		
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);	

		// Back
		glNormal3f(0.0f, 0.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);	

		// Top
		glNormal3f(0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);	

		// Bottom
		glNormal3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, -1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);	

		// Right
		glNormal3f(1.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(1.0f, 1.0f, 1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f(1.0f, -1.0f, 1.0f);	

		// Left
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 1.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 1.0f, 1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.0f, -1.0f);	
	glEnd();

	/*x_rot += x_speed;
	y_rot += y_speed;*/

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

int init_gl_lights()
{
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position);

	glEnable(GL_LIGHT1);

	return glGetError() == GL_NO_ERROR;
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

	if (!init_gl_texture())
	{
		kill_gl_window();
		MessageBox(nullptr, L"Initialization textures failed", L"Create window Error", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!init_gl_lights())
	{
		kill_gl_window();
		MessageBox(nullptr, L"Initialization lights failed", L"Create window Error", MB_OK | MB_ICONEXCLAMATION);
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

void get_window_size(int *width, int *height)
{
	*width = WND_WIDTH;
	*height = WND_HEIGHT;

	if (fullscreen)
	{
		MONITORINFO mInfo;
		HMONITOR hMonitor = MonitorFromWindow(nullptr, MONITOR_DEFAULTTONEAREST);
		mInfo.cbSize = sizeof(MONITORINFO);

		GetMonitorInfo(hMonitor, &mInfo);

		*width = mInfo.rcMonitor.right - mInfo.rcMonitor.left;
		*height = mInfo.rcMonitor.bottom - mInfo.rcMonitor.top;
	}
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

	int width = WND_WIDTH;
	int	height = WND_HEIGHT;

	get_window_size(&width, &height);

	if (!create_gl_window(WND_TITLE, width, height, COLOR_BITS, fullscreen))
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
			{
#ifdef _DEBUG
				OutputDebugString(L"[INFO] ESC typed. Program will be closed\n");
#endif
				done = true;
			}
			else
				SwapBuffers(hDC);

			if (keys['L'] && !l_press)
			{
				l_press = true; 
				light = !light;

				if (light)
					glEnable(GL_LIGHTING);
				else
					glDisable(GL_LIGHTING);
			}

			if (!keys['L']) l_press = false;

			if (keys['F'] && !f_press)
			{
				f_press = true;
				filter++;
				filter %= TEXTURES_COUNT;
			}

			if (!keys['F']) f_press = false;

			if (keys['B'] && !b_press)
			{
				b_press = true;
				blend = !blend;

				if (blend)
				{
					glEnable(GL_BLEND);
					glDisable(GL_DEPTH_TEST);
				}
				else
				{
					glDisable(GL_BLEND);
					glEnable(GL_DEPTH_TEST);
				}
			}

			if (!keys['B']) b_press = false;


			if (keys[VK_PRIOR]) z_depth -= 0.01f;	// PgUp
			if (keys[VK_NEXT]) z_depth += 0.01f;	// PgDown

			//if (keys[VK_UP]) x_speed += 0.01;	// Up
			//if (keys[VK_DOWN]) x_speed -= 0.01;	// Down
			//if (keys[VK_RIGHT]) y_speed += 0.01;	// Right
			//if (keys[VK_LEFT]) y_speed -= 0.01;	// Left

			if (keys[VK_UP]) x_rot += 0.1f;	// Up
			if (keys[VK_DOWN]) x_rot -= 0.1f;	// Down
			if (keys[VK_RIGHT]) y_rot += 0.1f;	// Right
			if (keys[VK_LEFT]) y_rot -= 0.1f;	// Left

			if (keys[VK_F1])
			{
				keys[VK_F1] = false;
				kill_gl_window();
				fullscreen = !fullscreen;
				get_window_size(&width, &height);
				if (!create_gl_window(WND_TITLE, width, height, COLOR_BITS, fullscreen))
					return 0;
			}
		}
	}

	kill_gl_window();
	return (msg.wParam);
}