#pragma once
#include <thread>
#include <memory>
#include <mutex>
#include <Iostream>
#include <string>
#include <Windows.h>

std::mutex m;
extern std::vector<HBITMAP> H_vecImageFileNames;

class CTask
{
public:

	CTask()
	{

	};
	CTask( std::wstring URL, bool _tasktype)
	{
		FileURL = URL;
		tasktype = _tasktype;
		return;
	};

	
	
	
	~CTask() {};
	void task() 
	{
		if (tasktype == true) 
		{
			draw_image();
		}
		else
		{
			playmusic();
		}
	
	
	};

	void playmusic() { 
		LPCWSTR SongToPlay = FileURL.c_str();
		PlaySound(SongToPlay, NULL, SND_FILENAME | SND_SYNC);
		return;
	};
	void draw_image() {
		m.lock();
		LoaderFile = (HBITMAP)LoadImageW(NULL, (LPCWSTR)FileURL.c_str(), IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
		//MessageBox(_hwnd, L"draw image?", L"DRAW IMAGE!!!!", MB_ICONASTERISK);
		H_vecImageFileNames.push_back(LoaderFile);
		m.unlock();

		return;
	}
	
	


private:
	
	std::wstring FileURL;
	bool tasktype = false; // false is music true is draw
	HBITMAP LoaderFile = NULL;
	
	
};


