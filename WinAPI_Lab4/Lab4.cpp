#include "Lab4.h"
#include <windowsx.h>
#pragma comment(lib, "d2d1")
#pragma comment(lib, "DWrite")

#include "resource1.h"
#include "resource2.h"

int numColor = 0;
HINSTANCE pInstance4Dialog;

float DPIScale::scaleX = 1.0f;
float DPIScale::scaleY = 1.0f;
D2D1::ColorF FuncBuilder::colors[7] = {
	D2D1::ColorF::Red, D2D1::ColorF::Orange, D2D1::ColorF::Yellow,
	D2D1::ColorF::Green, D2D1::ColorF::Cyan, D2D1::ColorF::Blue, D2D1::ColorF::Violet
};

void FuncBuilder::InsertEllipse(float x, float y)
{
	shared_ptr<MyEllipse> pme(new MyEllipse);
	D2D1_POINT_2F pt = {x, y};
	pme->ellipse.point = ptMouse = pt;
	pme->ellipse.radiusX = pme->ellipse.radiusY = 1.0f;
	pme->color = colors[numColor];
	ellipses.push_front(pme);
	selection = ellipses.begin();
}

void FuncBuilder::DeleteEllipse()
{
	ellipses.erase(selection);
	ClearSelection();
}

BOOL FuncBuilder::HitTest(float x, float y)
{
	list<shared_ptr<MyEllipse>>::iterator iter;
	for (iter = ellipses.begin(); iter != ellipses.end(); iter++)
		if ((*iter)->HitTest(x, y))
		{
			selection = iter;
			return TRUE;
		}
	return 0;
}

HRESULT FuncBuilder::CreateAdditionalGraphicsResources()
{
	HRESULT hr = S_OK;

	hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0), &pBrush);;
	if (SUCCEEDED(hr))
	{
		CalculateLayout();
	}
	return hr;
}

void FuncBuilder::DiscardGraphicsResources()
{
	SafeRelease(&pFactory);
	SafeRelease(&pRenderTarget);
	SafeRelease(&pBrush);
}

bool FuncBuilder::OnCreate()
{
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&pFactory);
	DPIScale::Initialize(pFactory);
	hMenubar = CreateMenu();
	AppendMenu(hMenubar, MF_ENABLED | MF_STRING, 322, TEXT("Цвет"));
	SetMenu(m_hwnd, hMenubar);
	return SUCCEEDED(hr);
}

void FuncBuilder::OnPaintScene()
{
	for (auto riter = ellipses.rbegin(); riter != ellipses.rend(); ++riter)
		(*riter)->Draw(pRenderTarget, pBrush, (Selection() && (*riter == *selection)));
}

void FuncBuilder::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
	const float dipX = DPIScale::PixelsToDipsX(pixelX);
	const float dipY = DPIScale::PixelsToDipsY(pixelY);
	if (mode == DrawMode)
	{
		POINT pt = {pixelX, pixelY};
		if (DragDetect(m_hwnd, pt))
		{
			SetCapture(m_hwnd);
			// Start a new ellipse.
			InsertEllipse(dipX, dipY);
		}
	}
	else
	{
		ClearSelection();
		if (HitTest(dipX, dipY))
		{
			SetCapture(m_hwnd);
			ptMouse = Selection()->ellipse.point;
			ptMouse.x -= dipX;
			ptMouse.y -= dipY;
			SetMode(DragMode);
		}
	}
	InvalidateRect(m_hwnd, NULL, FALSE);
}

void FuncBuilder::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
	const float dipX = DPIScale::PixelsToDipsX(pixelX);
	const float dipY = DPIScale::PixelsToDipsY(pixelY);
	if ((flags & MK_LBUTTON) && Selection())
	{
		if (mode == DrawMode)
		{
			// Resize the ellipse.
			const float width = (dipX - ptMouse.x) / 2;
			const float height = (dipY - ptMouse.y) / 2;
			const float x1 = ptMouse.x + width;
			const float y1 = ptMouse.y + height;
			Selection()->ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);
		}
		else if (mode == DragMode)
		{
			// Move the ellipse.
			Selection()->ellipse.point.x = dipX + ptMouse.x;
			Selection()->ellipse.point.y = dipY + ptMouse.y;
		}
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void FuncBuilder::OnLButtonUp()
{
	if ((mode == DrawMode) && Selection())
	{
		ClearSelection();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
	else if (mode == DragMode)
	{
		SetMode(SelectMode);
	}
	ReleaseCapture();
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG: // Инициализация
		SetWindowPos(hwnd,HWND_TOP, 5, 300, 0, 0, SWP_NOSIZE);
		break;
	case WM_COMMAND: // Обработчик команд кнопок
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:

			numColor = 0;
			EndDialog(hwnd, 0);
			return FALSE;

		case IDC_BUTTON2:

			numColor = 1;
			EndDialog(hwnd, 0);
			return FALSE;

		case IDC_BUTTON3:

			numColor = 2;
			EndDialog(hwnd, 0);
			return FALSE;

		case IDC_BUTTON4:

			numColor = 3;
			EndDialog(hwnd, 0);
			return FALSE;

		case IDC_BUTTON5:

			numColor = 4;
			EndDialog(hwnd, 0);
			return FALSE;

		case IDC_BUTTON6:

			numColor = 5;
			EndDialog(hwnd, 0);
			return FALSE;

		case IDC_BUTTON7:

			numColor = 6;
			EndDialog(hwnd, 0);
			return FALSE;
		}


		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		return FALSE;
	}
	return FALSE;
}

bool FuncBuilder::HandleAdditionalMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT*)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return false;
	case WM_LBUTTONUP:
		OnLButtonUp();
		return false;
	case WM_MOUSEMOVE:
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return false;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case 322:
			DialogBoxParam(pInstance4Dialog, MAKEINTRESOURCE(IDD_DIALOG1), GetActiveWindow(), (DlgProc), 0);
			break;
		case ID_DRAW_MODE:
			SetMode(DrawMode);
			break;
		case ID_SELECT_MODE:
			SetMode(SelectMode);
			break;
		case ID_TOGGLE_MODE:
			if (mode == DrawMode)
			{
				SetMode(SelectMode);
			}
			else
			{
				SetMode(DrawMode);
			}
			break;
		case ID_DELETE:
			if (Selection())
			{
				DeleteEllipse();
				InvalidateRect(m_hwnd, nullptr, FALSE);
			}
			break;
		}
		return false;
	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(hCursor);
			return TRUE;
		}
		return false;
	}
	return false;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
	FuncBuilder win;
	if (!win.Create(L"Ellipses", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}
	ShowWindow(win.Window(), nCmdShow);
	// Цикл обработки сообщений.
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
