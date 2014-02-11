#pragma once
#include "DoubleRect.h"

class ViewSettingObserver;
enum class ViewSettingObserverState;

class ViewSetting : public DoubleRect
{
public:
	ViewSetting();
	~ViewSetting();

	void SetFromClientRect(HWND const & hWnd);
	void Shift(long x, long y);
	void Crop(long x, long y);
	void CopyFrom(DoubleRect const & other);

	void SetScale(double const & newScale);
	void SetScaleToWindow(RECT const & clientRect);
	void SetScaleToMonitorSize(RECT const & monitorRect);

	double GetWidth();
	double GetHeight();
	double GetAspect();
	void GetScaledDimensions(long & width, long & height);
	void GetScaledRect(double const & width, double const & height, RECT & rect);

	void RegisterObserver(ViewSettingObserver * obs);
	void UnregisterObserver(ViewSettingObserver * obs);

private:
	double scale;

	std::vector<ViewSettingObserver*> observers;
	void NotifyObservers(ViewSettingObserverState const & state);

	static const double MaxResetRatio;
};

