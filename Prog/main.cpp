#include <windows.h>

#include <iostream>
#include <sstream>

#include <thread>
#include <chrono>

using namespace std::literals::chrono_literals;


void CreateWindow_s(HWND*** msg_s_Buff);


constexpr int windowAmount = 25; // 100 * 64bits = 640bits; 640/8 = 80bytes
int glbCounter = 404;

int main()
{
	HWND** msg_s_Buff[windowAmount] = {};
	CreateWindow_s(msg_s_Buff);

	float x = 0.0f;
	float y = 0.0f;
	std::this_thread::sleep_for(250ms);

	bool upwards = true;

	while (true)
	{
		if (GetAsyncKeyState('G') & 1)
		{
			char logbuff[1 << 8];
			SYSTEM_INFO info;
			GetSystemInfo(&info);

			HWND hDesktop = GetDesktopWindow();
			MONITORINFOEX monitorInfo;
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			GetMonitorInfo(MonitorFromWindow(hDesktop, MONITOR_DEFAULTTOPRIMARY), &monitorInfo);
			int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;

			

			for (int i = 0; i < windowAmount; i++)
			{
				RECT rect;
				GetWindowRect((*msg_s_Buff)[0][(int)i], &rect);

				constexpr int commonOffset = 5;
				int windowWidth  =  rect.right - rect.left;
				int windowHeight = rect.bottom - rect.top;

				int OffsetX = (windowWidth)*x + commonOffset;
				int OffsetY =  windowHeight * y  + commonOffset;

				if (OffsetX + (windowWidth + (commonOffset)) >= monitorWidth * 0.90) // 1.00 for %
				{
					y++;
					x = 0;
				}
				else
				{
					x++;
				}


				while (true)
				{
					if ((*msg_s_Buff)[0][i] != 0)
					{
						DWORD result = SetWindowPos((*msg_s_Buff)[0][i], NULL, (OffsetX), (OffsetY), 0, 0, SWP_NOZORDER | SWP_NOSIZE);
						break;
					}
				}

				std::this_thread::sleep_for(500000000ns / windowAmount);
			
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

		}

		if (GetAsyncKeyState(VK_RETURN))
		{
			[&]() { for (auto o : msg_s_Buff) { ShowWindow(**o, SW_HIDE); } };
		}
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
}
