
#include <Windows.h>
#include <memory>
#include <vector>
#include "CTask.h"
#include "WorkQueue.h"
#include <thread>
#include "ThreadPool.h"
#include <string>
#include <chrono>
#include "resource.h"
#include <Iostream>
#include <fstream>
#include <xaudio2.h>
#include <d2d1.h>
#include <mutex>
std::thread* myThreads;
#pragma comment(lib, "winmm.lib")
HBITMAP hBitmap = NULL;
ThreadPool tPool;
std::thread my_thread(&ThreadPool::Start, &tPool);
int TS;

#define WINDOW_CLASS_NAME L"MultiThreaded Loader Tool"
const unsigned int _kuiWINDOWWIDTH = 1200;
const unsigned int _kuiWINDOWHEIGHT = 1200;
#define MAX_FILES_TO_OPEN 50
#define MAX_CHARACTERS_IN_FILENAME 25

//Global Variables
std::vector<std::wstring> g_vecImageFileNames;
std::vector<std::wstring> g_vecSoundFileNames;
std::vector<HBITMAP> H_vecImageFileNames;


HINSTANCE g_hInstance;
std::mutex g_lock;
std::mutex m_lock;
int imageFileNumber = 0;

bool g_bIsFileLoaded = false;

bool ChooseImageFilesToLoad(HWND _hwnd)
{
	OPENFILENAME ofn;
	SecureZeroMemory(&ofn, sizeof(OPENFILENAME)); // Better to use than ZeroMemory
	wchar_t wsFileNames[MAX_FILES_TO_OPEN * MAX_CHARACTERS_IN_FILENAME + MAX_PATH]; //The string to store all the filenames selected in one buffer togther with the complete path name.
	wchar_t _wsPathName[MAX_PATH + 1];
	wchar_t _wstempFile[MAX_PATH + MAX_CHARACTERS_IN_FILENAME]; //Assuming that the filename is not more than 20 characters
	wchar_t _wsFileToOpen[MAX_PATH + MAX_CHARACTERS_IN_FILENAME];
	ZeroMemory(wsFileNames, sizeof(wsFileNames));
	ZeroMemory(_wsPathName, sizeof(_wsPathName));
	ZeroMemory(_wstempFile, sizeof(_wstempFile));

	//Fill out the fields of the structure
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = _hwnd;
	ofn.lpstrFile = wsFileNames;
	ofn.nMaxFile = MAX_FILES_TO_OPEN * 20 + MAX_PATH;  //The size, in charactesr of the buffer pointed to by lpstrFile. The buffer must be atleast 256(MAX_PATH) characters long; otherwise GetOpenFileName and 
													   //GetSaveFileName functions return False
													   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
													   // use the contents of wsFileNames to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.lpstrFilter = L"Bitmap Images(.bmp)\0*.bmp\0"; //Filter for bitmap images
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;

	//If the user makes a selection from the  open dialog box, the API call returns a non-zero value
	if (GetOpenFileName(&ofn) != 0) //user made a selection and pressed the OK button
	{
		//Extract the path name from the wide string -  two ways of doing it
		//First way: just work with wide char arrays
		wcsncpy_s(_wsPathName, wsFileNames, ofn.nFileOffset);
		int i = ofn.nFileOffset;
		int j = 0;

		while (true)
		{
			if (*(wsFileNames + i) == '\0')
			{

				_wstempFile[j] = *(wsFileNames + i);
				wcscpy_s(_wsFileToOpen, _wsPathName);
				wcscat_s(_wsFileToOpen, L"\\");
				wcscat_s(_wsFileToOpen, _wstempFile);
				g_vecImageFileNames.push_back(_wsFileToOpen);




				
				j = 0;

			}
			else
			{
				_wstempFile[j] = *(wsFileNames + i);
				j++;
			}
			if (*(wsFileNames + i) == '\0' && *(wsFileNames + i + 1) == '\0')
			{
				break;
			}
			else
			{
				i++;
			}

		}
		//*_wsPathName = '\0';
		//*_wsFileToOpen = '\0';
		g_bIsFileLoaded = true;
		return true;
	}
	else // user pressed the cancel button or closed the dialog box or an error occured
	{
		return false;
	}

}

bool ChooseSoundFilesToLoad(HWND _hwnd)
{
	OPENFILENAME ofn;
	SecureZeroMemory(&ofn, sizeof(OPENFILENAME)); // Better to use than ZeroMemory
	wchar_t wsFileNames[MAX_FILES_TO_OPEN * MAX_CHARACTERS_IN_FILENAME + MAX_PATH]; //The string to store all the filenames selected in one buffer togther with the complete path name.
	wchar_t _wsPathName[MAX_PATH + 1];
	wchar_t _wstempFile[MAX_PATH + MAX_CHARACTERS_IN_FILENAME]; //Assuming that the filename is not more than 20 characters
	//wchar_t _wsFileToOpen[MAX_PATH + MAX_CHARACTERS_IN_FILENAME];
	ZeroMemory(wsFileNames, sizeof(wsFileNames));
	ZeroMemory(_wsPathName, sizeof(_wsPathName));
	ZeroMemory(_wstempFile, sizeof(_wstempFile));

	//Fill out the fields of the structure
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = _hwnd;
	ofn.lpstrFile = wsFileNames;
	ofn.nMaxFile = MAX_FILES_TO_OPEN * 20 + MAX_PATH;  //The size, in charactesr of the buffer pointed to by lpstrFile. The buffer must be atleast 256(MAX_PATH) characters long; otherwise GetOpenFileName and 
													   //GetSaveFileName functions return False
													   // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
													   // use the contents of wsFileNames to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.lpstrFilter = L"Wave Files(*.wav)\0*.wav\0All Files (*.*)\0*.*\0"; //Filter for wav files
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT;
	

	//If the user makes a selection from the  open dialog box, the API call returns a non-zero value
	if (GetOpenFileName(&ofn) != 0) //user made a selection and pressed the OK button
	{
		//Extract the path name from the wide string -  two ways of doing it
		//Second way: work with wide strings and a char pointer 

		std::wstring _wstrPathName = ofn.lpstrFile;

		_wstrPathName.resize(ofn.nFileOffset, '\\');

		wchar_t *_pwcharNextFile = &ofn.lpstrFile[ofn.nFileOffset];

		while (*_pwcharNextFile)
		{
			std::wstring _wstrFileName = _wstrPathName + _pwcharNextFile;
			
			g_vecSoundFileNames.push_back(_wstrFileName);
			
			int temp1;
			temp1 = lstrlenW(_pwcharNextFile) + 1;
			_pwcharNextFile += temp1;
		}
		

		g_bIsFileLoaded = true;
		return true;
	}
	else // user pressed the cancel button or closed the dialog box or an error occured
	{
		return false;
	}

}
void image_draw(HWND _hwnd)
{
	
		int xPos = imageFileNumber * 100;
		int yPos = 0;
		do {
			if (xPos >= 1200) {
				yPos += 100;
				xPos -= 1200;
			}
		} while (xPos >= 1200);
		m_lock.lock();
		HBITMAP LoaderFile = H_vecImageFileNames.back();
		H_vecImageFileNames.pop_back();
		_hwnd = CreateWindow(L"STATIC", NULL, WS_VISIBLE | WS_CHILD | SS_BITMAP, xPos, yPos, 0, 0, _hwnd, NULL, NULL, NULL);
		imageFileNumber += 1;
		SendMessageW(_hwnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)LoaderFile);
		m_lock.unlock();




	return;
}


LRESULT CALLBACK WindowProc(HWND _hwnd, UINT _uiMsg, WPARAM _wparam, LPARAM _lparam)
{
	PAINTSTRUCT ps;
	HDC _hWindowDC;
	//RECT rect;
	switch (_uiMsg)
	{
	case WM_KEYDOWN:
	{
		switch (_wparam)
		{
		case VK_ESCAPE:
		{
			SendMessage(_hwnd, WM_CLOSE, 0, 0);
			return(0);
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_PAINT:
	{
		_hWindowDC = BeginPaint(_hwnd, &ps);
		


		EndPaint(_hwnd, &ps);
		return (0);
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(_wparam))
		{


		case ID_FILE_LOADIMAGE:
		{
			
			
			if (ChooseImageFilesToLoad(_hwnd))
			{
				auto ImageLoadTimeStart = std::chrono::high_resolution_clock::now();
				
				int filequantity = g_vecImageFileNames.size();
				int filesloaded = 0;
				
				if (g_vecImageFileNames.size() > MAX_FILES_TO_OPEN) { g_vecImageFileNames.resize(MAX_FILES_TO_OPEN); }
				while (g_vecImageFileNames.size() > 0) {
					
					std::wstring Url = g_vecImageFileNames.back();
					g_vecImageFileNames.pop_back();
					tPool.Initialize(Url, true);
					

				}
				
				do {
				filesloaded = H_vecImageFileNames.size();
				} while (filesloaded < filequantity);

				//Write code here to create multiple threads to load image files in parallel
				auto ImageLoadTimeStop = std::chrono::high_resolution_clock::now();
				auto ImageLoadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(ImageLoadTimeStop - ImageLoadTimeStart);
				std::wstring Out = std::to_wstring(ImageLoadDuration.count());
				LPCWSTR Outie = Out.c_str();
				//std::this_thread::sleep_for(std::chrono::milliseconds(50));
				while (H_vecImageFileNames.size() > 0) 
				{
					image_draw(_hwnd);
				}

				_hwnd = CreateWindow(L"STATIC", Outie, WS_VISIBLE | WS_CHILD | WS_BORDER, 20, 20, 300, 25, _hwnd, NULL, NULL, NULL);
			}
			else
			{
				MessageBox(_hwnd, L"No Image File selected", L"Error Message", MB_ICONWARNING);
			}
			
			//MessageBox(_hwnd, Outie ,L"Time Taken", MB_ICONASTERISK);
			
			return (0);
		}
		break;
		case ID_FILE_LOADSOUND:
		{
			if (ChooseSoundFilesToLoad(_hwnd))
			{
				if (g_vecSoundFileNames.size() > MAX_FILES_TO_OPEN) { g_vecSoundFileNames.resize(MAX_FILES_TO_OPEN); }
				
				while (g_vecSoundFileNames.size() > 0){
					int i = 0;
				std::wstring Url = g_vecSoundFileNames.back();
				g_vecSoundFileNames.pop_back();
				
				tPool.Initialize(Url, false);
				
				//std::this_thread::sleep_for(std::chrono::seconds(1));
				//PlaySound(result, NULL, SND_FILENAME | SND_ASYNC);
				
				}

			}
			else
			{
				MessageBox(_hwnd, L"No Sound File selected", L"Error Message", MB_ICONWARNING);
			}
			
			return (0);
		}
		break;
		case ID_EXIT:
		{
			SendMessage(_hwnd, WM_CLOSE, 0, 0);
			return (0);
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_CLOSE:
	{

		tPool.Stop();
		for (int i = 0; i < TS; i++) {
			if (myThreads[i].joinable()) {
				myThreads[i].join();
			}
			else { myThreads[i].detach(); }
		}

		if (my_thread.joinable()) {
			my_thread.join();
		}		
		PostQuitMessage(0);

	}
	break;
	default:
		break;
	}
	return (DefWindowProc(_hwnd, _uiMsg, _wparam, _lparam));
}


HWND CreateAndRegisterWindow(HINSTANCE _hInstance)
{
	WNDCLASSEX winclass; // This will hold the class we create.
	HWND hwnd;           // Generic window handle.

						 // First fill in the window class structure.
	winclass.cbSize = sizeof(WNDCLASSEX);
	winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc = WindowProc;
	winclass.cbClsExtra = 0;
	winclass.cbWndExtra = 0;
	winclass.hInstance = _hInstance;
	winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground =
		static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	winclass.lpszMenuName = NULL;
	winclass.lpszClassName = WINDOW_CLASS_NAME;
	winclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register the window class
	if (!RegisterClassEx(&winclass))
	{
		return (0);
	}

	HMENU _hMenu = LoadMenu(_hInstance, MAKEINTRESOURCE(IDR_MENU1));

	// create the window
	hwnd = CreateWindowEx(NULL, // Extended style.
		WINDOW_CLASS_NAME,      // Class.
		L"MultiThreaded Loader Tool",   // Title.
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		10, 10,                    // Initial x,y.
		_kuiWINDOWWIDTH, _kuiWINDOWHEIGHT,                // Initial width, height.
		NULL,                   // Handle to parent.
		_hMenu,                   // Handle to menu.
		_hInstance,             // Instance of this application.
		NULL);                  // Extra creation parameters.

	return hwnd;
}


int WINAPI WinMain(HINSTANCE _hInstance,HINSTANCE _hPrevInstance,LPSTR _lpCmdLine,int _nCmdShow)
{
	
	MSG msg;  //Generic Message
	HWND _hwnd = CreateAndRegisterWindow(_hInstance);
	WorkQueue<CTask> WQ;
	std::fstream myFile;
	//myFile.open("threadcount.txt", std::ios::out);
	//myFile << 6;
	//myFile.close();
	myFile.open("threadcount.txt", std::ios::in);
	myFile >> TS;
	myFile.close();
	
	myThreads = new std::thread[TS];
	

	if (!(_hwnd))
	{
		return (0);
	}
	//thread for threadpool
	
	
	
	

	
	for (int i = 0; i < TS; i++) {
		myThreads[i] = std::thread(&ThreadPool::Submit, &tPool);
	}
	
	
	//joining threads (always join your threads)
	
	


	// Enter main event loop
	while (true)
	{
		
		// Test if there is a message in queue, if so get it.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			
			// Test if this is a quit.
			if (msg.message == WM_QUIT)
			{
				break;
			}

			// Translate any accelerator keys.
			TranslateMessage(&msg);
			// Send the message to the window proc.
			DispatchMessage(&msg);
		}
		

	}
	
	

	// Return to Windows like this...
	return (static_cast<int>(msg.wParam));
}