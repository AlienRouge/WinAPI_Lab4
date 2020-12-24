#include  "D2D_Win.h"
#pragma comment(lib, "d2d1")

D2D_Win::D2D_Win() : pFactory(nullptr), pRenderTarget(nullptr)
{
}

D2D_Win::~D2D_Win()
= default;

HRESULT D2D_Win::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == nullptr)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size),
			&pRenderTarget);
		if (SUCCEEDED(hr))
		{
			hr = CreateAdditionalGraphicsResources();
		}
	}
	return hr;
}

HRESULT D2D_Win::CreateAdditionalGraphicsResources()
{
	return S_OK;
}

void D2D_Win::DiscardGraphicsResources()
{
	SafeRelease(&pRenderTarget);
}

void D2D_Win::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		BeginPaint(m_hwnd, &ps);
		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
		OnPaintScene();
		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		EndPaint(m_hwnd, &ps);
	}
}

void D2D_Win::Resize()
{
	if (pRenderTarget != nullptr)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		pRenderTarget->Resize(size);
		CalculateLayout();
		InvalidateRect(m_hwnd, nullptr, FALSE);
	}
}

bool D2D_Win::OnCreate()
{
	if (FAILED(D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
	{
		return false; // Неудача CreateWindowEx.
	}
	return true;
}

void D2D_Win::OnDestroy()
{
	DiscardGraphicsResources();
	SafeRelease(&pFactory);
}

LRESULT D2D_Win::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		if (!OnCreate())
		{
			return -1; // Неудача CreateWindowEx.
		}
		return 0;

	case WM_DESTROY:
		OnDestroy();
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_SIZE:
		Resize();
		return 0;
	default: ;
	}
	LRESULT Result = 0;
	if (HandleAdditionalMessage(uMsg, wParam, lParam, &Result)) return Result;
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

bool D2D_Win::HandleAdditionalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT*)
{
	return false;
}
