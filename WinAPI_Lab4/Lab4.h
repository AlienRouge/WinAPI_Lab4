#pragma once
#include "D2D_Win.h"
#include <d2d1.h>
#include <dwrite.h>
#include <list>
#include <memory>
#pragma comment(lib, "d2d1")
using namespace std;

class DPIScale
{
	static float scaleX;
	static float scaleY;
public:
	static void Initialize(ID2D1Factory* pFactory)
	{
		FLOAT dpiX, dpiY;
		pFactory->GetDesktopDpi(&dpiX, &dpiY);
		scaleX = dpiX / 96.0f;
		scaleY = dpiY / 96.0f;
	}

	template <typename T>
	static float PixelsToDipsX(T x)
	{
		return static_cast<float>(x) / scaleX;
	}

	template <typename T>
	static float PixelsToDipsY(T y)
	{
		return static_cast<float>(y) / scaleY;
	}

	template <typename T>
	static D2D1_POINT_2F PixelsToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / scaleX, static_cast<float>(y) / scaleY);
	}
};

struct MyEllipse
{
	D2D1_ELLIPSE ellipse;
	D2D1_COLOR_F color;

	void Draw(ID2D1RenderTarget* pRT, ID2D1SolidColorBrush* pBrush, BOOL isSelect)
	{
		pBrush->SetColor(color);
		pRT->FillEllipse(ellipse, pBrush);
		pBrush->SetColor(isSelect ? D2D1::ColorF(D2D1::ColorF::White) : D2D1::ColorF(D2D1::ColorF::Black));
		pRT->DrawEllipse(ellipse, pBrush, 1.0f);
	}

	BOOL HitTest(float x, float y)
	{
		const float a = ellipse.radiusX;
		const float b = ellipse.radiusY;
		const float x1 = x - ellipse.point.x;
		const float y1 = y - ellipse.point.y;
		const float d = ((x1 * x1) / (a * a)) + ((y1 * y1) / (b * b));
		return d <= 1.0f;
	}
};

class FuncBuilder : public D2D_Win
{
	ID2D1SolidColorBrush* pBrush;
	D2D1_ELLIPSE ellipse;
	D2D1_POINT_2F ptMouse;
	HCURSOR hCursor;
	HMENU hMenubar;

	list<shared_ptr<MyEllipse>> ellipses;
	list<shared_ptr<MyEllipse>>::iterator selection;

	enum Mode { DrawMode, SelectMode, DragMode } mode;

	static D2D1::ColorF colors[7];

	shared_ptr<MyEllipse> Selection()
	{
		if (selection == ellipses.end())
		{
			return nullptr;
		}
		else
		{
			return (*selection);
		}
	}

	void ClearSelection() { selection = ellipses.end(); }

	void SetMode(Mode m)
	{
		mode = m;
		// Update the cursor
		LPWSTR cursor = IDC_ARROW;
		switch (mode)
		{
		case DrawMode:
			cursor = IDC_CROSS;
			break;
		case SelectMode:
			cursor = IDC_HAND;
			break;
		case DragMode:
			cursor = IDC_SIZEALL;
			break;
		}
		hCursor = LoadCursor(NULL, cursor);
		SetCursor(hCursor);
	}

	void InsertEllipse(float x, float y);
	void DeleteEllipse();
	BOOL HitTest(float x, float y);

public:
	FuncBuilder(): pBrush(nullptr), ptMouse(D2D1::Point2F())
	{
		ClearSelection();
		SetMode(DrawMode);
	}

	PCWSTR ClassName() const override { return L"Ellipses Window Class"; }

	void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void OnLButtonUp();
	void OnMouseMove(int pixelX, int pixelY, DWORD flags);


	HRESULT CreateAdditionalGraphicsResources() override;
	void DiscardGraphicsResources() override;
	void OnPaintScene() override;
	bool OnCreate() override;
	bool HandleAdditionalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT*) override;
};
