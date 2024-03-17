#include <windows.h>

#include <iostream>
#include <sstream>

#include <thread>
#include <chrono>

using namespace std::literals::chrono_literals;


void CreateWindow_s(HWND*** msg_s_Buff);


constexpr int windowAmount = 100; // 100 * 64bits = 640bits; 640/8 = 80bytes
int glbCounter = 404;

int main()
{
	HWND** msg_s_Buff[windowAmount] = {};



	CreateWindow_s(msg_s_Buff);

	std::this_thread::sleep_for(250ms);



	float x = 0.0f;
	float y = 0.0f;
	bool upwards = true;

	while (true)
	{

		if (GetAsyncKeyState(VK_RETURN))
		{
			[&]() { for (auto o : msg_s_Buff) { ShowWindow(**o, SW_HIDE); } };
		}

		if (GetAsyncKeyState('G') & 1)
		{
			SYSTEM_INFO info;
			GetSystemInfo(&info);

			HWND hDesktop = GetDesktopWindow();
			MONITORINFOEX monitorInfo;
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			GetMonitorInfo(MonitorFromWindow(hDesktop, MONITOR_DEFAULTTOPRIMARY), &monitorInfo);
			int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;

			char logbuff[1 << 8];
			for (int i = 0; i < windowAmount / 4; i++)
			{
				for (int y = 1; y < windowAmount / 10 - 1; y++)
				{
					for (int x = 1; x < windowAmount / 10 - 1; x++)
					{
						//std::this_thread::sleep_for(50ns);

						RECT rect;
						GetWindowRect((*msg_s_Buff)[0][y], &rect);
						int OffsetX = rect.right - rect.left + 5;
						int OffsetY = rect.bottom - rect.top + 5;


						DWORD result = SetWindowPos((*msg_s_Buff)[0][i], NULL, (x), (OffsetY * y), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
					}


					//#pragma    region ErrorHandling
					//				DWORD errCode = GetLastError();
					//
					//				LPSTR errBuff = nullptr;
					//				FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					//					NULL, errCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&errBuff, 0, NULL);
					//
					//				if (errBuff != nullptr) {
					//					sprintf_s(buff, "Result: %lu\n\nError: %s -> %lu", result, errBuff, errCode);
					//					LocalFree(errBuff); // Freigabe des Puffers
					//				}
					//				else {
					//					sprintf_s(buff, "Result: %lu\n\nError: Unknown error -> %lu", result, errCode);
					//				}
					//				MessageBox(NULL, buff, "nope :/", MB_ICONERROR);
					//#pragma endregion ErrorHandling



				}
			}
		}


		/*if (x < 360.0f)
		{
			x += 0.005f; y = x;
		}
		else
			x = 0.0f; y = x;


		POINT cursor;

		GetCursorPos(&cursor);

		float radius = 50;
		int winX = (int)((cos(x/1) * (radius * 3.140f)) + ( (cursor.x) )  );
		int winY = (int)((sin(y/1) * (radius * 3.140f)) + ( (cursor.y) )  );
		static int a = 0;
		SetWindowPos(hWin, NULL, winX, winY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);


		std::this_thread::sleep_for(std::chrono::nanoseconds(500));


		std::cout << "  x = " << x;
		std::cout << "\t";
		std::cout << "  y = " << y;
		std::cout << "\n";*/




		std::this_thread::sleep_for(5ms);

	}



	return 0;
}

void CreateWindow_s(HWND*** H_msg_s_Buff)
{
	*H_msg_s_Buff = new HWND * [windowAmount];
	static int counter;
	std::thread MessageBoxWorker[windowAmount];
	char name[windowAmount][20];
	static bool failedArray[windowAmount];
	static bool allFailed = false;
	bool neededTime = false;

	for (int i = 0; i < windowAmount; i++)
	{

		// Reserve the size the pointers in the array
		(*H_msg_s_Buff)[i] = new HWND[windowAmount];

		sprintf_s(name[i], "Window-%d", i);

		MessageBoxWorker[i] = std::thread([&, i]() { MessageBox(NULL, "Hello", name[i], MB_OK); });

		MessageBoxWorker[i].detach();

		// Optional: Add a small delay to give threads time to find the windows
		std::this_thread::sleep_for(5000ns);
	}

	for (int i = 0; i < windowAmount; i++)
	{
		if (MessageBoxWorker[i].joinable())
		{
			MessageBoxWorker[i].join();
		}
	}

	// Find the windows after MessageBoxWorker threads have finished
	for (int i = 0; i < windowAmount; i++)
	{
		for (int b = 0; b < windowAmount; b++)
		{
			if (failedArray[b])
			{
				allFailed = true;
				break;
			}
		}


		while (true)
		{

			HWND windowHandle = FindWindowA(NULL, name[i]);

			if (windowHandle == NULL)
			{
				if (counter >= 1 << 14)   // 2^14 = 16k.
				{
					break;
				}
				else
				{
					++counter;
				}

				std::this_thread::sleep_for(500ns);

				windowHandle = NULL;

				// ich war schon mal hier, also sagen dass man Zeit gebraucht hat
				neededTime = true;
			}
			else
			{
				(*H_msg_s_Buff)[0][i] = windowHandle;

				SetWindowPos(windowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
				if (MessageBoxWorker[i].joinable())
				{
					MessageBoxWorker[i].join();
				}
				glbCounter = counter;


				break;
			}




		}
	}


	//std::stringstream ss;
	//ss << glbCounter;
	//std::string str = ss.str();

	//std::string sCopy = str;

	//std::thread t;
	//if (!neededTime)
	//{
	//	t = std::thread([&sCopy]() { MessageBox(NULL, "You didn't need time O_O", "Wow!", MB_OK); }); // ＼（〇_ｏ）／
	//}
	//else
	//{
	//	t = std::thread([&sCopy]() { MessageBox(NULL, sCopy.c_str(), "Tried Tests ( x * 500ns ):", MB_OK); });
	//}

	//std::this_thread::sleep_for(std::chrono::milliseconds(50));

	//t.join();



}
