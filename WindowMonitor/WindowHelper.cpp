#pragma once
#include "stdafx.h"
#include "WindowHelper.h"
#include <Strsafe.h>

namespace WindowHelper
{
	void GetResourceString(HINSTANCE const & hInstance, UINT const & nID, std::wstring & str)
	{
		const wchar_t * buffer;
		int len = LoadStringW(hInstance, nID, reinterpret_cast<LPWSTR>(&buffer), 0);
		str.assign(&buffer[0], len);
	}
	
	void GetResourceString(HINSTANCE const & hInstance, UINT const & nID, std::string & str)
	{
		std::vector<char> buffer(4096, '\0');
		LoadStringA(hInstance, nID, &buffer[0], 4096);
		str.assign(&buffer[0]);
	}

	HICON GetIcon(HINSTANCE const & hInstance, UINT const & nID, bool big)
	{
		return static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(nID), IMAGE_ICON, 
			GetSystemMetrics(big ? SM_CXICON : SM_CXSMICON), 
			GetSystemMetrics(big ? SM_CYICON : SM_CYSMICON), 
			0));
	}

	void SetTitle(HWND const & hWnd, HINSTANCE const & hInstance, UINT const & nID)
	{
		std::wstring text;
		WindowHelper::GetResourceString(hInstance, nID, text);
		SetWindowTextW(hWnd, text.c_str());
	}

	HFONT CreateFont(HWND const & hWnd, LPCWSTR const & faceName, double const & points, int const & weight,
		bool const & italic, bool const & underline, bool const & strikeout)
	{
		HDC hdc = GetDC(hWnd);

		int fontHeight = -static_cast<int>((points * static_cast<double>(GetDeviceCaps(hdc, LOGPIXELSY))) / 72.0);

		return ::CreateFont(
			fontHeight,
			0, 0, 0,
			weight,
			italic ? TRUE : FALSE,
			underline ? TRUE : FALSE,
			strikeout ? TRUE : FALSE,
			ANSI_CHARSET, OUT_DEVICE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
			faceName);
	}

	void GetEditText(HWND const & hWnd, std::wstring & text)
	{
		// Get number of characters + null terminator
		int textLength = static_cast<int>(SendMessage(hWnd, WM_GETTEXTLENGTH, 0, 0)) + 1;

		// Create buffer
		std::vector<wchar_t> buffer(textLength, '\0');

		// Get winapi-friendly pointer to buffer
		wchar_t* bufferPointer = reinterpret_cast<wchar_t*>(&buffer[0]);

		// Get text from control
		SendMessage(hWnd, WM_GETTEXT, static_cast<WPARAM>(textLength), reinterpret_cast<LPARAM>(bufferPointer));

		// Set output string
		text.assign(&buffer[0]);
	}

	void GetClassNameText(HWND const & hWnd, std::wstring & text)
	{
		// Get number of characters + null terminator
		int textLength = 257;

		// Create buffer
		std::vector<wchar_t> buffer(textLength, '\0');

		// Get classname
		GetClassNameW(hWnd, &buffer[0], textLength);

		// Set output string
		text.assign(&buffer[0]);
	}

	void GetListboxItemText(HWND const & hWnd, int const & index, std::wstring & text)
	{
		// Check
		if (index < 0) return;

		// Get number of characters for selected listbox item
		int textLength = static_cast<int>(SendMessage(hWnd, LB_GETTEXTLEN, static_cast<WPARAM>(index), 0));

		// Create buffer + null terminator
		std::vector<wchar_t> buffer(textLength + 1, '\0');

		// Get winapi-friendly pointer to buffer
		wchar_t* bufferPointer = reinterpret_cast<wchar_t*>(&buffer[0]);

		// Get text from control
		SendMessage(hWnd, LB_GETTEXT, static_cast<WPARAM>(index), reinterpret_cast<LPARAM>(bufferPointer));

		// Set output string
		text.assign(&buffer[0]);
	}

	void GetMonitorRect(HWND const & hWnd, RECT & rect)
	{
		HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo;
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(monitor, &monitorInfo);
		rect.bottom = monitorInfo.rcMonitor.bottom;
		rect.left = monitorInfo.rcMonitor.left;
		rect.right = monitorInfo.rcMonitor.right;
		rect.top = monitorInfo.rcMonitor.top;
	}

	void DisplayExceptionMessage(unsigned int const & titleId, unsigned int const & textId, std::exception const & e)
	{
		// Get title
		std::wstring title;
		WindowHelper::GetResourceString(NULL, titleId, title);

		// Get message template
		std::wstring messageTemplate;
		WindowHelper::GetResourceString(NULL, textId, messageTemplate);

		// Fill placeholder with exception text
		auto pos = messageTemplate.find(L"%1");
		if (pos != std::string::npos) 
		{
			std::string err(e.what());
			std::wstring wideErr(err.begin(), err.end());
			messageTemplate.replace(pos, 2, wideErr);
		}

		// Display message box
		MessageBoxW(NULL, messageTemplate.c_str(), title.c_str(), MB_OK | MB_ICONERROR | MB_TOPMOST);
	}

	HINSTANCE GetCurrentModuleHandle()
	{
		HMODULE module = NULL;
		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, reinterpret_cast<LPCTSTR>(&GetCurrentModuleHandle), &module))
			return module;
		return NULL;
	}

	void GetPathToExecutable(std::wstring & filePath)
	{
		// Get full path to exe
		WCHAR path[MAX_PATH];
		HMODULE hModule = GetCurrentModuleHandle();
		if (GetModuleFileNameW(hModule, path, MAX_PATH) == 0) return;

		// Get length
		size_t len = 0;
		StringCchLength(path, MAX_PATH, &len);
		if (len < 1) return;

		// Move to end
		LPWSTR tmp = path + len;

		// Find last slash in path
		do
		{
			tmp--;
			if (*tmp == L'\\')
			{
				// Set string terminator
				*tmp = L'\0';
				break;
			}
		} while (tmp > path);

		filePath.assign(path);
		filePath.append(L"\\");
	}

	void FillRegion(HDC const & hdc, HBRUSH const & brush, int const & x1, int const & y1, int const & x2, int const & y2)
	{
		HRGN region = CreateRectRgn(x1, y1, x2, y2);
		FillRgn(hdc, region, brush);
		DeleteObject(region);
	}
}
