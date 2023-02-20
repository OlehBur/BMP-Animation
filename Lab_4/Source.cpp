#include <Windows.h>
#include <string>
#include <tchar.h>

//���� ��������
#pragma comment(lib,"Winmm.lib")

using namespace std;

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE This, HINSTANCE Prev, LPTSTR cmd, int mode) {
	static TCHAR WndName[] = _T("GeneralWnd");
	HWND hWnd;
	MSG msg;

	WNDCLASS wc;
	wc.hInstance = This;
	wc.lpszClassName = WndName;
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClass(&wc)) return 0;
	hWnd = CreateWindow(WndName,
		_T("BMP-animation"),
		WS_SYSMENU,
		0,
		0,
		0,
		0,
		HWND_DESKTOP,
		NULL,
		This,
		NULL);
	ShowWindow(hWnd, mode);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//������� ��'� ��������� �����
	static TCHAR lofiSound[] = _T("meditation.wav");

	PAINTSTRUCT ps;
	HDC hdc;
	//���������� �������� ��� ��� ������ �� �������� ��� ���
	static int textureIndex = 1, buffTextureIndex=1; //������� ������� ��� �������
	static int caption, menu, border;//������ ���� ����
	static bool isStart = false;
	static HDC memBit;
	static HBITMAP hbmpCosmonaut;
	static BITMAP bmpCosmonaut;
	string bmpTextureStr;
	static LPCWSTR backgrTexture = L"textures/cosmonaut_(1).bmp";


	switch (message)
	{
	case WM_CREATE:

		//�������� ������ ��� ���� ���� ��� ����������� �� ��� �������� ������
		caption = GetSystemMetrics(SM_CYCAPTION);
		menu = GetSystemMetrics(SM_CYMENU);
		border = GetSystemMetrics(SM_CXFIXEDFRAME);

		//��������� ������� �������� ��� ������ �� ��������� �� �� �������
		hbmpCosmonaut = (HBITMAP)LoadImage(NULL, backgrTexture, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_LOADTRANSPARENT);
		if (hbmpCosmonaut == NULL)
		{
			MessageBox(hWnd, _T("���� �� ��������"), _T("������������ ������"),
				MB_OK | MB_ICONHAND);
			DestroyWindow(hWnd);
			return 1;
		}
		GetObject(hbmpCosmonaut, sizeof(bmpCosmonaut), &bmpCosmonaut);//�������� ��'��� ������

		hdc = GetDC(hWnd);
		memBit = CreateCompatibleDC(hdc);
		ReleaseDC(hWnd, hdc);

		break;
	case WM_SIZE:
		//������� ����� ���� �� ����� ������
		MoveWindow(hWnd, GetSystemMetrics(SM_CXSCREEN) / 2 - bmpCosmonaut.bmWidth / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - caption - bmpCosmonaut.bmHeight / 2, bmpCosmonaut.bmWidth + 2 * border, bmpCosmonaut.bmHeight + caption + menu - border * 1.5, TRUE);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		//������� �� ������� �����
		SelectObject(memBit, hbmpCosmonaut);
		BitBlt(hdc, 0, 0, bmpCosmonaut.bmWidth, bmpCosmonaut.bmHeight, memBit, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		//������ ����� �� ��� ���� ��� ���������/���������� �������
		isStart = !isStart;
		if (isStart) {
			//��������� ����
			PlaySound(lofiSound, NULL, SND_FILENAME | SND_LOOP | SND_ASYNC);
			//��������� ������ ��� ���� ������� (�������) ������ 
			SetTimer(hWnd, 1, 80, NULL);
		}
		else {
			//��������� ����������� ������
			PlaySound(0, 0, 0);
			//������ ������
			KillTimer(hWnd, 1);
		}

		InvalidateRect(hWnd, NULL, true);
		break;
	case WM_TIMER:
		if (isStart) {
			//��� ������� ������ ������� ������� ������ �������� ������ � ����������� ���� �������� ��������
			bmpTextureStr = "textures/cosmonaut_(" + to_string(textureIndex) + ").bmp";
			wstring wStr(bmpTextureStr.begin(), bmpTextureStr.end());
			backgrTexture = wStr.c_str();

			//������� ������� 157 �� ���������� �� ���� ������� ����� ������� ����� �� 313 
			//(�������� ������ �� ��������� ������ �� ����� ��� ������ �������)
			if (buffTextureIndex < 313/*157*/) {
				buffTextureIndex++;
				//���� �������� ������ ����� 157,
				(buffTextureIndex < 157) ?
					// �� ���������� �� ��������� ��������� �������
					textureIndex = buffTextureIndex :
					//������ ������� �� ������� ��� ������� (��������� ��������� ������ �� ���������) �������� ��������� ����������� 
					//��������� �������
					textureIndex = 314 - buffTextureIndex;
			}
			else {//�� ����������� ������ �� ����� ������� ��������� �� ���������� ��������� ��������� �������
				buffTextureIndex = 1;
				textureIndex = 1;
			}

			hbmpCosmonaut = (HBITMAP)LoadImage(NULL, backgrTexture, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_LOADTRANSPARENT);
			if (hbmpCosmonaut == NULL)
			{
				MessageBox(hWnd, _T("���� �� ��������"), _T("������������ ������"),
					MB_OK | MB_ICONHAND);
				DestroyWindow(hWnd);
				return 1;
			}
			GetObject(hbmpCosmonaut, sizeof(bmpCosmonaut), &bmpCosmonaut);

			InvalidateRect(hWnd, NULL, true);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}