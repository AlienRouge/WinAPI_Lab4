#pragma once
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include "basewin.h"

template <class T>
void SafeRelease(T** ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = nullptr;
	}
}


class D2D_Win : public BaseWindow<D2D_Win>
{
protected:
	ID2D1Factory* pFactory;
	ID2D1HwndRenderTarget* pRenderTarget;
public:
	D2D_Win();
	~D2D_Win();

	void OnDestroy();
	void OnPaint();
	void Resize();
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	HRESULT CreateGraphicsResources();

	virtual bool OnCreate();

	virtual void CalculateLayout()
	{
	}

	virtual HRESULT CreateAdditionalGraphicsResources();
	virtual void DiscardGraphicsResources();

	virtual void OnPaintScene()
	{
	}

	virtual bool HandleAdditionalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT*);
};
