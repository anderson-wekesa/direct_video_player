#include <Windows.h>
#include "Functions.h"

#pragma comment(lib, "strmiids.lib")

/*

	char Buffer[10];
	DWORD err = GetLastError();            //Error-Checking Code
	itoa(err, Buffer, 10);
	SetWindowTextA(hDisplay, Buffer);
	return;

*/


IGraphBuilder* pIGraphBuilder = NULL;
IMediaControl* pIMediaControl = NULL;
IMediaEvent* pIMediaEvent = NULL;
IVideoWindow* pIVideoWindow = NULL;

HWND hDisplay;

bool bNowPlaying = false;

void Initialize(HWND hWnd)
{

//------------------------------------------------Initialize UI--------------------------------------------------------//
	CreateWindowExA(0, "Static", "Enter Path or URL:", WS_VISIBLE | WS_CHILD,
		10, 20, 130, 25, hWnd, NULL, NULL, NULL);

	hDisplay = CreateWindowExA(0, "Edit", NULL, WS_VISIBLE| WS_CHILD | WS_BORDER,
		150, 20, 690, 25, hWnd, NULL, NULL, NULL);

	CreateWindowExA(0, "Button", "Play", WS_VISIBLE | WS_CHILD, 850, 450,
		80, 40, hWnd, (HMENU)BTN_PLAY, NULL, NULL);


//----------------------------------------Initialize COM and Other Playback Elements-----------------------------------------//
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
	{
		MessageBoxW(NULL, L"Failed to Initialize COM!", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}

	if (CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (LPVOID*)&pIGraphBuilder) != S_OK)
	{
		MessageBoxW(NULL, L"Failed to Create Graph Builder!", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}



	HRESULT hControl = pIGraphBuilder->QueryInterface(IID_IMediaControl, (LPVOID*)&pIMediaControl);
	if (hControl == E_POINTER)
	{
		MessageBoxW(NULL, L"Failed to Obtain Media Controls!", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
		//return;
	}

	HRESULT hEvent = pIGraphBuilder->QueryInterface(IID_IMediaEvent, (LPVOID*)&pIMediaEvent);
	if (FAILED(hEvent))
	{
		MessageBoxW(NULL, L"Failed to Obtain Media Events!", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}


	HRESULT hVideoWindow = pIGraphBuilder->QueryInterface(IID_IVideoWindow, (LPVOID*)&pIVideoWindow);
	if (hVideoWindow != S_OK)
	{
		MessageBoxW(NULL, L"Failed to Instantiate Video Control!", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}

	
}

void PlayFile(HWND hWnd)
{
	if (bNowPlaying == true)
	{
		return;
	}

	HRESULT hRender = pIGraphBuilder->RenderFile(L"C:\\Users\\opiyo\\Videos\\testadio.avi", NULL);

	if (hRender != S_OK)
	{
		MessageBoxW(NULL, L"Failed to Render File!", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	pIVideoWindow->put_Owner((OAHWND)hWnd);
	pIVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS);
	pIVideoWindow->SetWindowPosition(10, 60, 790, 440);
	pIVideoWindow->put_Visible(true);

	

	HRESULT hRun = pIMediaControl -> Run();

	if (FAILED(hRun))
	{
		MessageBoxW(NULL, L"Failed to Play File!", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}

	bNowPlaying = true;

	//long evCode;
	//pIMediaEvent->WaitForCompletion(INFINITE, &evCode);

}

void CleanUp()
{
	pIMediaEvent->Release();
	pIMediaControl->Release();
	pIGraphBuilder->Release();
	CoUninitialize();
}