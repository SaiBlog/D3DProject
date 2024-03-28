#pragma once
#include<d3dx9.h>
#include<d3d9.h>
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //����һ����ȫ�ͷź꣬���ں���COM�ӿ�ָ����ͷ�
#define HR(x) { hr = x; if( FALIED(hr) ){ return hr; }}// HR�꣬����ִ��������ķ���
#define SAFE_DELETE(x) {if(x){delete(x);(p)=NULL;}}// SAFE_DELETE�꣬������Դ�ͷš�

struct SKYBOXVERTEX
{
	float x, y, z;
	float u, v;
};

#define D3DFVF_SKYBOX D3DFVF_XYZ|D3DFVF_TEX1

class D3DSkyBoxClass
{
	float m_Length;

	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPDIRECT3DVERTEXBUFFER9 m_pSkyBoxVertexBuffer;
	LPDIRECT3DTEXTURE9 m_pSkyBoxTexture[5];

public:
	D3DSkyBoxClass(LPDIRECT3DDEVICE9);
	~D3DSkyBoxClass();

	BOOL InitSkyBox(float);
	BOOL LoadSkyTextureFromFile(const wchar_t*, const wchar_t*, const wchar_t*, const wchar_t*, const wchar_t*);
	VOID RenderSkyBox(D3DXMATRIX*, BOOL);
};

