#include "D3DParticleClass.h"




D3DParticleClass::D3DParticleClass(LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
	m_pVertexBuffer = NULL;
	for (int i = 0; i < 6; i++)
	{
		m_pTexture[i] = NULL;
	}
	for (int i = 0; i < PARTICLE_NUMBER; i++)
	{
		m_Snow[i] = { 0 };
	}
}

D3DParticleClass::~D3DParticleClass()
{
	SAFE_RELEASE(m_pVertexBuffer);
	for (int i = 0; i < 6; i++)SAFE_RELEASE(m_pTexture[i]);
}

HRESULT D3DParticleClass::InitParticle()
{
	srand(GetTickCount());
	for (int i = 0; i < PARTICLE_NUMBER; i++)
	{
		m_Snow[i].x = float(rand() % PARTICLE_FIELDLENGTH - PARTICLE_FIELDLENGTH / 2);
		m_Snow[i].y = float(rand() % PARTICLE_FIELDHEIGHT);
		m_Snow[i].z = float(rand() % PARTICLE_FIELDWIDTH - PARTICLE_FIELDWIDTH / 2);

		m_Snow[i].RotateSpeedY = (rand() % 100) / 50.0f * D3DX_PI;
		m_Snow[i].RotateSpeedX = (rand() % 100) / 50.0f * D3DX_PI;
		m_Snow[i].FallSpeed = 200.0f + rand() % 500;
		m_Snow[i].RotationSpeed = 5.0f + rand() % 10 / 10.0f;
		m_Snow[i].TextureIndex = rand() % 6;
	}

	m_pD3DDevice->CreateVertexBuffer(4 * sizeof(PARTICLEVERTEX), 0,
		D3DFVF_PARTICLEVERTEX, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);

	PARTICLEVERTEX Vertices[] = {
		{ -20.0f, 0.0f, 0.0f,   0.0f, 1.0f, },
		{ -20.0f, 40.0f, 0.0f,   0.0f, 0.0f, },
		{  20.0f, 0.0f, 0.0f,   1.0f, 1.0f, },
		{  20.0f, 40.0f, 0.0f,   1.0f, 0.0f, }
	};

	VOID* pVertices;
	m_pVertexBuffer->Lock(0, sizeof(Vertices), (void**)&pVertices, 0);
	memcpy(pVertices, Vertices, sizeof(Vertices));
	m_pVertexBuffer->Unlock();

	D3DXCreateTextureFromFile(m_pD3DDevice, PARTICLE_TEXTUREPATH1, &m_pTexture[0]);
	D3DXCreateTextureFromFile(m_pD3DDevice, PARTICLE_TEXTUREPATH2, &m_pTexture[1]);
	D3DXCreateTextureFromFile(m_pD3DDevice, PARTICLE_TEXTUREPATH3, &m_pTexture[2]);
	D3DXCreateTextureFromFile(m_pD3DDevice, PARTICLE_TEXTUREPATH4, &m_pTexture[3]);
	D3DXCreateTextureFromFile(m_pD3DDevice, PARTICLE_TEXTUREPATH5, &m_pTexture[4]);
	D3DXCreateTextureFromFile(m_pD3DDevice, PARTICLE_TEXTUREPATH6, &m_pTexture[5]);




	return S_OK;
}

HRESULT D3DParticleClass::UpdateParticle(float fElasptime)
{
	for (int i = 0; i < PARTICLE_NUMBER; i++)
	{
		m_Snow[i].y -= m_Snow[i].FallSpeed * fElasptime;
		if (m_Snow[i].y < 1000)m_Snow[i].y = PARTICLE_FIELDHEIGHT;
		m_Snow[i].RotateSpeedX += fElasptime * m_Snow[i].RotationSpeed;
		m_Snow[i].RotateSpeedY += fElasptime * m_Snow[i].RotationSpeed;
	}

	return S_OK;
}

HRESULT D3DParticleClass::RenderParitcle()
{
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);//关闭光照 

	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);   //将纹理颜色混合的第一个参数的颜色值用于输出
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);   //纹理颜色混合的第一个参数的值就取纹理颜色值

	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);  //缩小过滤状态采用线性纹理过滤
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR); //放大过滤状态采用线性纹理过滤

	//???Alpha去黑边
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);//打开Alpha混合
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);//源混合系数设为1
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);//目标混合系数设为1
	//

	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);//不剔除任何面

	for (int i = 0; i < PARTICLE_NUMBER; i++)
	{
		static D3DXMATRIX matPitch, matTrans, matFinal, matYaw;

		D3DXMatrixRotationX(&matPitch, m_Snow[i].RotateSpeedX);
		D3DXMatrixRotationY(&matYaw, m_Snow[i].RotateSpeedY);

		D3DXMatrixTranslation(&matTrans, m_Snow[i].x, m_Snow[i].y, m_Snow[i].z);

		matFinal = matYaw * matPitch * matTrans;

		m_pD3DDevice->SetTransform(D3DTS_WORLD, &matFinal);

		m_pD3DDevice->SetTexture(0, m_pTexture[m_Snow[i].TextureIndex]);
		m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PARTICLEVERTEX));
		m_pD3DDevice->SetFVF(D3DFVF_PARTICLEVERTEX);
		m_pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}




	//复原
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);//剔除逆三角形
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);




	return S_OK;
}
