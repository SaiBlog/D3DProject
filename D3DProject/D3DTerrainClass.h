#pragma once
#include<d3d9.h>
#include<d3dx9.h>
#include<vector>
#include<fstream>
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //定义一个安全释放宏，便于后面COM接口指针的释放
#define HR(x) { hr = x; if( FALIED(hr) ){ return hr; }}// HR宏，方便执行语句错误的返回
#define SAFE_DELETE(x) {if(x){delete(x);(p)=NULL;}}// SAFE_DELETE宏，用于资源释放。

class D3DTerrainClass
{
	LPDIRECT3DDEVICE9 m_pD3DDevice;
	LPDIRECT3DTEXTURE9 m_pTerrainTexture;
	LPDIRECT3DVERTEXBUFFER9 m_pTerrainVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 m_pTerrainIndexBuffer;


	int m_nCellsPerRow;//每行单元格数量
	int m_nCellsPerCol;//每列单元格数量

	int m_nVerticesPerRow;//每行顶点数量
	int m_nVerticesPerCol;
	int m_nTotalVerticesNum;

	FLOAT m_fTerrainLength;
	FLOAT m_fTerrainWidth;
	FLOAT m_fCellSpace;
	FLOAT m_fScaleMulti;//高度倍率

	D3DLOCKED_RECT lockedRect;
	DWORD* imageData;

	std::vector<FLOAT> m_vHeightInfo;

	struct TERRAINVERTEX
	{
		FLOAT _x, _y, _z;
		FLOAT _u, _v;
		TERRAINVERTEX(FLOAT x, FLOAT y, FLOAT z, FLOAT u, FLOAT v) :_x(x), _y(y), _z(z), _u(u), _v(v){};


		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;//在没有对象时我们也能访问FVF所以设置为静态
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