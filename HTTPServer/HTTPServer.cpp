// Simple HTTP Server.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "HTTPServer.h"
#include <WinSock2.h>
#include <stdio.h>
#define MAX_LOADSTRING 100
#define MAX_WND	1024
#define MAX_SOCKET_NUM 2
#define WM_ACCEPT WM_USER + 1
#define WM_RECV	WM_USER + 2
#define WM_SOCKET_CLOSE WM_USER + 3
// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND g_hwnd[MAX_WND]; //Danh sach cua so da tao
int g_count[MAX_WND]; //So socket cua moi cua so
int g_nWnd = 0; //So luong cua so da tao

				// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_HTTPSERVER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HTTPSERVER));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_HTTPSERVER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_HTTPSERVER);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WSADATA DATA;
	SOCKET s = INVALID_SOCKET;
	SOCKET c = INVALID_SOCKET;
	SOCKADDR_IN saddr;
	SOCKADDR_IN caddr;
	WIN32_FIND_DATAA FDATA;
	int clen;
	int i;
	char* buffer = NULL;
	char* clenstr = NULL;
	char* html = NULL;
	char* getstr = NULL;
	char* folder = NULL;
	char* httpstr = NULL;
	char* fullpath = NULL;
	switch (message)
	{
	case WM_RECV:
		if (LOWORD(lParam) == FD_READ)
		{
			int portDownload = 10000;
			buffer = (char*)calloc(1024, 1);
			getstr = (char*)calloc(1024, 1);
			folder = (char*)calloc(1024, 1);
			httpstr = (char*)calloc(1024, 1);
			fullpath = (char*)calloc(1024, 1);
			recv((SOCKET)wParam, buffer, 1024, 0);
			sscanf(buffer, "%s%s%s", getstr, folder, httpstr);
			//Xoa ki tu / thua 
			if (folder[strlen(folder) - 1] == '/') {
				folder[strlen(folder) - 1] = '\0';
			}
			if (strstr(folder, "%20")) {
				char* tmp = folder;
				folder = replaceStr(folder, "%20", " ");
				free(tmp);
			}
			free(buffer);
			if (strlen(folder + 1) > 0)
				sprintf(fullpath, "C:\\%s\\*.*", folder + 1);
			else
				sprintf(fullpath, "%s", "C:\\*.*");			
			HANDLE hFind = FindFirstFileA(fullpath, &FDATA);
			//Xet dieu kien co la fordel khong, neu la file thi gui file
			if ((strcmp(FDATA.cFileName, ".") == 0) || (strlen(folder + 1) == 0)) {
				html = (char*)calloc(32768, 1);
				sprintf(html, "%s", "<html><body>");
				sprintf(html + strlen(html), "%s", add_info(caddr));
				do
				{
					if (strlen(folder + 1) == 0) {
						if (FDATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
							sprintf(html + strlen(html), "<a href=\"%s\">%s</a><br>", getLink(FDATA.cFileName), FDATA.cFileName);
						}
						else {
							sprintf(html + strlen(html), "<a href=\"%s\" download=\"%s\">%s</a><br>", getLink(FDATA.cFileName), FDATA.cFileName, FDATA.cFileName);
						}
					}
					else
						if (FDATA.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
							sprintf(html + strlen(html), "<a href=\"%s\/%s\">%s</a><br>", getLink(folder + 1), FDATA.cFileName, FDATA.cFileName);
						}
						else {
							sprintf(html + strlen(html), "<a href=\"%s\/%s\" download=\"%s\">%s</a><br>", getLink(folder + 1), FDATA.cFileName, FDATA.cFileName, FDATA.cFileName);
						}
				} while (FindNextFileA(hFind, &FDATA));
				sprintf(html + strlen(html), "</body></html>");

				clenstr = (char*)calloc(1024, 1);
				sprintf(clenstr, "Content - length: %d\n", strlen(html));

				send((SOCKET)wParam, "HTTP/1.1 200 OK\n", 16, 0);
				send((SOCKET)wParam, clenstr, strlen(clenstr), 0);
				send((SOCKET)wParam, "Content-Type: text/html\n\n", 25, 0);
				send((SOCKET)wParam, html, strlen(html), 0);
			}
			else {
				char* fileName = (char*)calloc(1024, 1);				
				sprintf(fileName, "C:%s", folder);
				//replaceStr(fileName, "/", "\\");
				sendFile(fileName, (SOCKET)wParam);
				free(fileName);
			}
			free(clenstr);
			free(html);
		}
		else
		{
			for (i = 1; i < g_nWnd; i++)
			{
				if (g_hwnd[i] == hWnd)
					break;
			}
			if (g_count[i] > 0)
				g_count[i] -= 1;
		}
		closesocket((SOCKET)wParam);//Dong socket vi trinh duyet se load lien tuc
		break;
	case WM_ACCEPT:
		clen = sizeof(caddr);
		c = accept((SOCKET)wParam, (sockaddr*)&caddr, &clen);
		if (g_nWnd == 1) //Moi co 1 cua so chinh
		{
			//Tao them cua so
			HWND hWnd1 = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst, nullptr);
			if (hWnd1)
			{
				g_hwnd[g_nWnd] = hWnd1;
				g_count[g_nWnd] = 0;
				g_nWnd += 1;
				ShowWindow(hWnd1, 0);
				UpdateWindow(hWnd1);
			}
		}
		for (i = 1; i < g_nWnd; i++)
		{
			if (g_count[i] < MAX_SOCKET_NUM)
			{
				WSAAsyncSelect(c, g_hwnd[i], WM_RECV, FD_READ | FD_CLOSE);
				g_count[i] += 1;
				//Cua so nay van con co the xu ly them socket
				break;
			}
		}
		if (i == g_nWnd) //Khong con cua so nao xu ly them duoc
		{
			//Tao them cua so
			HWND hWnd1 = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInst, nullptr);
			if (hWnd1)
			{
				WSAAsyncSelect(c, hWnd1, WM_RECV, FD_READ | FD_CLOSE);
				g_hwnd[g_nWnd] = hWnd1;
				g_count[g_nWnd] = 1;
				g_nWnd += 1;
			}
		}
		break;
	case WM_CREATE:
		if (g_nWnd == 0)
		{
			g_hwnd[g_nWnd] = hWnd;
			g_count[g_nWnd] = 0;
			g_nWnd += 1;

			WSAStartup(MAKEWORD(2, 2), &DATA);
			s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			saddr.sin_family = AF_INET;
			saddr.sin_port = htons(PORT);
			saddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
			bind(s, (sockaddr*)&saddr, sizeof(saddr));
			listen(s, 5);
			WSAAsyncSelect(s, hWnd, WM_ACCEPT, FD_ACCEPT);
		}
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
