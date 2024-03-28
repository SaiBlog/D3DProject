#pragma once
#include<d3d9.h>
#include<d3dx9.h>
#include<vector>
#include<fstream>
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //����һ����ȫ�ͷź꣬���ں���COM�ӿ�ָ����ͷ�
#define HR(x) { hr = x; if( FALIED(hr) ){ return hr; }}// HR�꣬����ִ��������ķ���
#define SAFE_DELETE(x) {if(x){delete(x);(p)=NULL;}}// SAFE_DELETE�꣬������Դ�ͷš�

class D3DTerrainClass
{
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPDIRECT3DTEXTURE9 m_pTerrainTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pTerrainVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 m_pTerrainIndexBuffer;


	int m_nCellsPerRow;//ÿ�е�Ԫ������
	int m_nCellsPerCol;//ÿ�е�Ԫ������

	int m_nVerticesPerRow;//ÿ�ж�������
	int m_nVerticesPerCol;
	int m_nTotalVerticesNum;

	FLOAT m_fTerrainLength;
	FLOAT m_fTerrainWidth;
	FLOAT m_fCellSpace;
	FLOAT m_fScaleMulti;//�߶ȱ���

	D3DLOCKED_RECT lockedRect;
	DWORD* imageData;

	std::vector<FLOAT> m_vHeightInfo;

	struct TERRAINVERTEX
	{
		FLOAT _x, _y, _z;
		FLOAT _u, _v;
		TERRAINVERTEX(FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v) :_x(x), _y(y), _z(z), _u(u), _v(v){};


		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;//��û�ж���ʱ����Ҳ�ܷ���FVF��������Ϊ��̬
	};

public:
	D3DTerrainClass(LPDIRECT3DDEVICE9);
	~D3DTerrainClass();

	BOOL InitTerrain(int, int, float, float);
	BOOL LoadTerrainFromFile(const wchar_t* ,const wchar_t*);
	BOOL RenderTerrain(D3DXMATRIX*, bool);
	float GetTerrainMapHeight(int row, int col);
	float ComputeShade(int row, int col);
	float GetTerrainSpace();
	float GetHeight(float x, float z);
	float Lerp(float a, float b, float t);
};