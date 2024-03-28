#pragma once
#include<d3d9.h>
#include<d3dx9.h>

#define PARTICLE_TEXTUREPATH1 L"PARTICLE\\snow1.jpg"
#define PARTICLE_TEXTUREPATH2 L"PARTICLE\\snow2.jpg"
#define PARTICLE_TEXTUREPATH3 L"PARTICLE\\snow3.jpg"
#define PARTICLE_TEXTUREPATH4 L"PARTICLE\\snow4.jpg"
#define PARTICLE_TEXTUREPATH5 L"PARTICLE\\snow5.jpg"
#define PARTICLE_TEXTUREPATH6 L"PARTICLE\\snow6.jpg"

#define PARTICLE_NUMBER 10000
#define PARTICLE_FIELDWIDTH 20000
#define PARTICLE_FIELDLENGTH 20000
#define PARTICLE_FIELDHEIGHT 10000
#define D3DFVF_PARTICLEVERTEX D3DFVF_XYZ|D3DFVF_TEX1
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //����һ����ȫ�ͷź꣬���ں���COM�ӿ�ָ����ͷ�
#define HR(x) { hr = x; if( FALIED(hr) ){ return hr; }}// HR�꣬����ִ��������ķ���
#define SAFE_DELETE(x) {if(x){delete(x);(p)=NULL;}}// SAFE_DELETE�꣬������Դ�ͷš�


struct PARTICLEVERTEX
{
	float x, y, z;//����λ��
	float u, v;//������������
};

struct SNOWPARTICLE
{
	float x, y, z;
	float RotateSpeedX;
	float RotateSpeedY;
	float FallSpeed;
	float RotationSpeed;
	int TextureIndex;//������������ʾΪ��һ�������ѩ��
};

class D3DParticleClass
{
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	SNOWPARTICLE m_Snow[PARTICLE_NUMBER];
	LPDIRECT3DTEXTURE9 m_pTexture[6];
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBuffer;

public:
	D3DParticleClass(LPDIRECT3DDEVICE9 pD3DDevice);
	~D3DParticleClass();
	HRESULT InitParticle();
	HRESULT UpdateParticle(float fElasptime);
	HRESULT RenderParitcle();

};

