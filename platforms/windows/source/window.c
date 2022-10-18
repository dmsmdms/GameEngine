#include <stdbool.h>
#include <asserts.h>
#include <windows.h>
#include <options.h>
#include <logger.h>
#include <window.h>
#include <config.h>
#ifdef CONFIG_RENDERER_VULKAN
#include <vulkan/api/allocation_callbacks.h>
#include <vulkan/api/debug_utils.h>
#include <vulkan/api/instance.h>
#include <vulkan/api/surface.h>
#endif

static HINSTANCE hInstance = NULL;
static HWND hWindow = NULL;
static ATOM hClass = 0;

static LRESULT CALLBACK rkWindowEventHandler(const HWND hWindow, const UINT messageId,
											 const WPARAM wParam, const LPARAM lParam)
{
	switch (messageId) {
	case WM_CLOSE:
		appIsRunning = false;
		break;
	}

	return DefWindowProc(hWindow, messageId, wParam, lParam);
}

void rkWindowCreate(void) {
	hInstance = GetModuleHandle(NULL);
	RK_ASSERT(hInstance == NULL, NULL);

	const WNDCLASSEX classEx = {
		.cbSize         = sizeof(WNDCLASSEX),
		.style          = 0,
		.lpfnWndProc    = rkWindowEventHandler,
		.cbClsExtra     = 0,
		.cbWndExtra     = 0,
		.hInstance      = hInstance,
		.hIcon          = NULL,
		.hCursor        = LoadCursor(NULL, IDC_ARROW),
		.hbrBackground  = NULL,
		.lpszMenuName   = NULL,
		.lpszClassName  = CONFIG_ENGINE_NAME,
		.hIconSm        = NULL,
	};

	hClass = RegisterClassEx(&classEx);
	RK_ASSERT(hClass == 0, NULL);

	DWORD dwExStyle = WS_EX_APPWINDOW;
	DWORD dwStyle = WS_VISIBLE;

	if (cmdOptions.fullscreen != false) {
		cmdOptions.width = GetSystemMetrics(SM_CXSCREEN);
		cmdOptions.height = GetSystemMetrics(SM_CYSCREEN);
		dwStyle |= WS_POPUP;

		const DEVMODE devMode = {
			.dmSize			= sizeof(DEVMODE),
			.dmPelsWidth	= cmdOptions.width,
			.dmPelsHeight	= cmdOptions.height,
			.dmFields		= DM_PELSWIDTH | DM_PELSHEIGHT,
		};

		const LONG result = ChangeDisplaySettings((LPDEVMODEA)&devMode, CDS_FULLSCREEN);
		RK_ASSERT(result != DISP_CHANGE_SUCCESSFUL, NULL);
	} else {
		dwExStyle |= WS_EX_WINDOWEDGE;
		dwStyle |= WS_OVERLAPPEDWINDOW;
	}

	hWindow = CreateWindowEx(dwExStyle, CONFIG_ENGINE_NAME, CONFIG_PROJECT_NAME,
							 dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, cmdOptions.width,
							 cmdOptions.height, NULL, NULL, hInstance, NULL);
	RK_ASSERT(hWindow == NULL, NULL);

	rkLog(RK_LOG_LEVEL_INFO, "Win32 window created! (width: %hu, height: %hu, fullscreen: %hu)",
		  (uint16_t)cmdOptions.width, (uint16_t)cmdOptions.height, (uint16_t)cmdOptions.fullscreen);
}

void rkWindowGetEvents(void) {
	MSG windowMsg;

	for (WINBOOL hasMsg; (hasMsg = PeekMessage(&windowMsg, hWindow, 0, 0, PM_REMOVE)); (void)hasMsg) {
		const LRESULT result = DispatchMessage(&windowMsg);
		RK_ASSERT(result != RK_RESULT_SUCCESS, NULL);
	}
}

void rkWindowDestroy(void) {
	if (hInstance != NULL) {
		if (hWindow != NULL) {
			DestroyWindow(hWindow);
			hWindow = NULL;
		}

		if (hClass != 0) {
			UnregisterClass(CONFIG_ENGINE_NAME, hInstance);
			hClass = 0;
		}

		hInstance = NULL;
		rkLog(RK_LOG_LEVEL_INFO, "Win32 window destroyed!");
	}
}

#ifdef CONFIG_RENDERER_VULKAN
const char * vkGetWsiSurfaceExtension(void) {
	return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
}

void vkCreateSurfaceWrap(void) {
	const VkWin32SurfaceCreateInfoKHR surfaceInfo = {
		.sType		= VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
		.pNext		= NULL,
		.flags		= 0,
		.hinstance	= hInstance,
		.hwnd		= hWindow,
	};

	const VkResult result = vkCreateWin32SurfaceKHR(instance, &surfaceInfo,
													allocationCallbacks, &surface);
	RK_VULKAN_ASSERT(result, NULL);
	vkLog(VULKAN_TAG_INFO, "Win32 surface created!");
}
#endif
