#include "Global.h"

using namespace std;
bool gameover = false;
#define APPLICATION_INSTANCE_MUTEX_NAME "{7C5CCAF4-229A-47D1-B545-8DFB8ACDCF3E}"
//Window event callback function
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_CLOSE:
		gameover = true;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//Entry Point for a Windows Program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	//create mutex to ensure only one instance

	HANDLE MutexInstance = CreateMutex(NULL, TRUE, APPLICATION_INSTANCE_MUTEX_NAME);
	bool AlreadyExist = false;
	if (GetLastError() == ERROR_ALREADY_EXISTS) AlreadyExist = true; //checking if handle to mutex already exist
	if (MutexInstance == NULL || AlreadyExist)//if mutex cannot be created
	{
		if (MutexInstance)
		{
			ReleaseMutex(MutexInstance);
			CloseHandle(MutexInstance);
		}
		return 0;
	}
	

	//set window properties
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MainWindowCLass";
	wc.hIconSm = NULL;
	RegisterClassEx(&wc);

	//create new window

	HWND window = CreateWindow(
		"MainWindowClass", //window class
		APPTITLE.c_str(), //title bar
		WS_OVERLAPPEDWINDOW, //WINDOW style
		CW_USEDEFAULT, CW_USEDEFAULT, //position of window
		SCREENW, SCREENH, //dimensions of the window
		NULL, // parent window(not used)
		NULL, //menu(not used)
		hInstance, //application instance
		NULL); //window parameters(not used)

	//was there an error creating the window?
	if (!window) return 1;
	//display the window
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);


	//initialize the game
	if (!Game_Init(window)) return 2;
	//main message loop
	MSG msg = { 0 };
	while (!gameover)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//process game loop
		Game_Run(window);
	}
	//shutdown
	Game_End();

	return (int)msg.wParam;
}