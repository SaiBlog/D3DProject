#pragma once
#include<dinput.h>
#define DIRECTINPUT_VERSION 0x0800


class D3DInputClass
{
private:
	IDirectInput8* m_pD3DInput;
	IDirectInputDevice8* m_pD3DInputMouseDevice;
	IDirectInputDevice8* m_pD3DInputKeyboardDevice;
	DIMOUSESTATE m_diMouseState;
	char m_KeyStateBuffer[256];

public:
	D3DInputClass();
	~D3DInputClass();
	HRESULT Init(HINSTANCE, HWND, DWORD, DWORD);

	void Get_Input();
	bool Is_KeyDown(int);
	bool Is_MouseDown(int);

	float Get_MouseInputX();
	float Get_MouseInputY();
	float Get_MouseInputZ();

};