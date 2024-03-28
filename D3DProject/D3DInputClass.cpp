#include"D3DInputClass.h"

#pragma region �Զ����
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //����һ����ȫ�ͷź꣬���ں���COM�ӿ�ָ����ͷ�
#define HR(x) { hr = x; if( FAILED(hr) ){ return hr; }}// HR�꣬����ִ��������ķ���
#define SAFE_DELETE(x) {if(x){delete(x);(p)=NULL;}}// SAFE_DELETE�꣬������Դ�ͷš�
#pragma endregion

D3DInputClass::D3DInputClass()
{
	m_pD3DInput = NULL;
	m_pD3DInputKeyboardDevice = NULL;
	m_pD3DInputMouseDevice = NULL;
	ZeroMemory(&m_diMouseState, sizeof(m_diMouseState));
	ZeroMemory(&m_KeyStateBuffer, sizeof(char)*256);
}

D3DInputClass::~D3DInputClass()
{
	if (m_pD3DInputMouseDevice != NULL)m_pD3DInputMouseDevice->Unacquire();
	if (m_pD3DInputKeyboardDevice != NULL)m_pD3DInputKeyboardDevice->Unacquire();
	SAFE_RELEASE(m_pD3DInput);
	SAFE_RELEASE(m_pD3DInputKeyboardDevice);
	SAFE_RELEASE(m_pD3DInputMouseDevice);
}

HRESULT D3DInputClass::Init(HINSTANCE hInstance, HWND hWnd,DWORD keyboardCoopFlags,DWORD mouseCoopFlags)
{
	HRESULT hr;

	HR(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pD3DInput, NULL));

		//������豸�����ʽ
	HR(m_pD3DInput->CreateDevice(GUID_SysMouse, &m_pD3DInputMouseDevice, NULL));
	HR(m_pD3DInputMouseDevice->SetCooperativeLevel(hWnd, mouseCoopFlags));
	HR(m_pD3DInputMouseDevice->SetDataFormat(&c_dfDIMouse));
	HR(m_pD3DInputMouseDevice->Acquire());
	HR(m_pD3DInputMouseDevice->Poll());

	//�������豸�����ʽ
	HR(m_pD3DInput->CreateDevice(GUID_SysKeyboard, &m_pD3DInputKeyboardDevice, NULL));
	HR(m_pD3DInputKeyboardDevice->SetCooperativeLevel(hWnd, keyboardCoopFlags));
	HR(m_pD3DInputKeyboardDevice->SetDataFormat(&c_dfDIKeyboard));
	HR(m_pD3DInputKeyboardDevice->Acquire());
	HR(m_pD3DInputKeyboardDevice->Poll());

	return S_OK;
}

void D3DInputClass::Get_Input()
{
	
	HRESULT hr = m_pD3DInputMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (void**)&m_diMouseState);
	//��ȡ���������Ϣ
	if (hr)
	{
		m_pD3DInputMouseDevice->Acquire();
		m_pD3DInputMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), (void**)&m_diMouseState);
	}
	hr = m_pD3DInputKeyboardDevice->GetDeviceState(sizeof(m_KeyStateBuffer), (void**)&m_KeyStateBuffer);
	//��ȡ����������Ϣ
	if (hr)
	{
		m_pD3DInputKeyboardDevice->Acquire();
		m_pD3DInputKeyboardDevice->GetDeviceState(sizeof(m_KeyStateBuffer), (LPVOID)m_KeyStateBuffer);
	}

}

bool D3DInputClass::Is_KeyDown(int x)
{
	return (m_KeyStateBuffer[x] & 0x80);
}

bool D3DInputClass::Is_MouseDown(int x)
{
	return (m_diMouseState.rgbButtons[x] & 0x80);
}

float D3DInputClass::Get_MouseInputX()
{
	return (float)m_diMouseState.lX;
}

float D3DInputClass::Get_MouseInputY()
{
	return (float)m_diMouseState.lY;
}

float D3DInputClass::Get_MouseInputZ()
{
	return (float)m_diMouseState.lZ;
}