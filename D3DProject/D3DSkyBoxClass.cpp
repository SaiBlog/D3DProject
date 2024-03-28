#include "D3DSkyBoxClass.h"


D3DSkyBoxClass::D3DSkyBoxClass(LPDIRECT3DDEVICE9 pDevice)
{
	m_pD3DDevice = pDevice;
	m_Length = 0;
	m_pSkyBoxVertexBuffer = NULL;
	for (int i = 0; i < 5; i++)
	{
		m_pSkyBoxTexture[i] = NULL;
	}

}

D3DSkyBoxClass::~D3DSkyBoxClass()
{
	SAFE_RELEASE(m_pSkyBoxVertexBuffer);
	for (int i = 0; i < 5; i++)
	{
		SAFE_RELEASE(m_pSkyBoxTexture[i]);
	}
}

BOOL D3DSkyBoxClass::LoadSkyTextureFromFile(const wchar_t* pFrontTextureFileName, const wchar_t* pBackTextureFileName, 
	const wchar_t* pLeftTextureFileName, const wchar_t* pRightTextureFileName, const wchar_t* pTopTextureFileName)
{
	D3DXCreateTextureFromFile(m_pD3DDevice, pFrontTextureFileName, &m_pSkyBoxTexture[0]);
	D3DXCreateTextureFromFile(m_pD3DDevice, pBackTextureFileName, &m_pSkyBoxTexture[1]);
	D3DXCreateTextureFromFile(m_pD3DDevice, pLeftTextureFileName, &m_pSkyBoxTexture[2]);
	D3DXCreateTextureFromFile(m_pD3DDevice, pRightTextureFileName, &m_pSkyBoxTexture[3]);
	D3DXCreateTextureFromFile(m_pD3DDevice, pTopTextureFileName, &m_pSkyBoxTexture[4]);

	return TRUE;
}


BOOL D3DSkyBoxClass::InitSkyBox(float Length)
{
	m_Length = Length;

	m_pD3DDevice->CreateVertexBuffer(20 * sizeof(SKYBOXVERTEX), 0, D3DFVF_SKYBOX, D3DPOOL_MANAGED, &m_pSkyBoxVertexBuffer, 0);

	SKYBOXVERTEX Vertices[] =
	{
		//前面的四个顶点
		{ -m_Length / 2, 0.0f,    m_Length / 2, 0.0f, 1.0f, },
		{ -m_Length / 2, m_Length / 2,   m_Length / 2, 0.0f, 0.0f, },
		{  m_Length / 2, 0.0f,    m_Length / 2, 1.0f, 1.0f, },
		{  m_Length / 2, m_Length / 2,   m_Length / 2, 1.0f, 0.0f, },

		//背面的四个顶点
		{  m_Length / 2, 0.0f,   -m_Length / 2, 0.0f, 1.0f, },
		{  m_Length / 2, m_Length / 2,  -m_Length / 2, 0.0f, 0.0f, },
		{ -m_Length / 2, 0.0f,   -m_Length / 2, 1.0f, 1.0f, },
		{ -m_Length / 2, m_Length / 2,  -m_Length / 2, 1.0f, 0.0f, },

		//左面的四个顶点
		{ -m_Length / 2, 0.0f,   -m_Length / 2, 0.0f, 1.0f, },
		{ -m_Length / 2, m_Length / 2,  -m_Length / 2, 0.0f, 0.0f, },
		{ -m_Length / 2, 0.0f,    m_Length / 2, 1.0f, 1.0f, },
		{ -m_Length / 2, m_Length / 2,   m_Length / 2, 1.0f, 0.0f, },

		//右面的四个顶点
		{ m_Length / 2, 0.0f,   m_Length / 2, 0.0f, 1.0f, },
		{ m_Length / 2, m_Length / 2,  m_Length / 2, 0.0f, 0.0f, },
		{ m_Length / 2, 0.0f,  -m_Length / 2, 1.0f, 1.0f, },
		{ m_Length / 2, m_Length / 2, -m_Length / 2, 1.0f, 0.0f, },

		//上面的四个顶点
		{  m_Length / 2, m_Length / 2, -m_Length / 2, 1.0f, 0.0f, },
		{  m_Length / 2, m_Length / 2,  m_Length / 2, 1.0f, 1.0f, },
		{ -m_Length / 2, m_Length / 2, -m_Length / 2, 0.0f, 0.0f, },
		{ -m_Length / 2, m_Length / 2,  m_Length / 2, 0.0f, 1.0f, },

	};

	void* pVertices;
	m_pSkyBoxVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	memcpy(pVertices, Vertices, sizeof(Vertices));
	m_pSkyBoxVertexBuffer->Unlock();

	return TRUE;
}

VOID D3DSkyBoxClass::RenderSkyBox(D3DXMATRIX* pMatrix, BOOL bRenderFrame)
{
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);  //将纹理颜色混合的第一个参数的颜色值用于输出
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);   //纹理颜色混合的第一个参数的值就取纹理颜色值
	m_pD3DDevice->SetTransform(D3DTS_WORLD, pMatrix);
	m_pD3DDevice->SetStreamSource(0, m_pSkyBoxVertexBuffer, 0, sizeof(SKYBOXVERTEX));
	m_pD3DDevice->SetFVF(D3DFVF_SKYBOX);

	for (int i = 0; i < 5; i++)
	{
		m_pD3DDevice->SetTexture(0, m_pSkyBoxTexture[i]);
		m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 4, 2);
	}

	if (bRenderFrame)
	{
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		for (int i = 0; i < 5; i++)//绘制顶点
		{
			m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, i * 4, 2);
		}
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

}