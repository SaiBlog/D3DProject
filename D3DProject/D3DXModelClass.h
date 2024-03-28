#pragma once
#include<d3d9.h>
#include<d3dx9.h>
#include<tchar.h>
#include"Assist.h"

struct XMODELVERTEX
{
	D3DXVECTOR3 p;
	D3DXVECTOR3 n;
	FLOAT tu, tv;

	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
};

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //����һ����ȫ�ͷź꣬���ں���COM�ӿ�ָ����ͷ�
#define HR(x) { hr = x; if( FALIED(hr) ){ return hr; }}// HR�꣬����ִ��������ķ���
#define SAFE_DELETE(x) {if(x){delete(x);(p)=NULL;}}// SAFE_DELETE�꣬������Դ�ͷš�
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

class D3DXModelClass
{
private:
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	DWORD m_wNumMtrl;
	LPD3DXMESH m_pMesh;
	D3DMATERIAL9* m_pMaterial;
	LPDIRECT3DTEXTURE9* m_pTexture;


	LPDIRECT3DVERTEXBUFFER9 m_pVerticesBuffer;
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;



public:
	LPD3DXPMESH m_pPMesh;
	D3DXModelClass();
	D3DXModelClass(LPDIRECT3DDEVICE9 pD3DDevice);
	~D3DXModelClass();

	VOID DisChangeMesh(float Dis);
	virtual HRESULT LoadXModelFromFile(const wchar_t* strFileName);
	virtual HRESULT RenderXModelPMesh();//��������
	virtual HRESULT RenderXModelMesh();//��ͨXFile

	bool Is_RayIntersected(Ray ray,DWORD& Faces);
	bool IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir,
		D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2,
		FLOAT* t, FLOAT* u, FLOAT* v);
};

