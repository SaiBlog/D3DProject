#include "D3DXModelClass.h"


D3DXModelClass::D3DXModelClass(LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
}

D3DXModelClass::D3DXModelClass()
{

}

D3DXModelClass::~D3DXModelClass()
{
	SAFE_DELETE_ARRAY(m_pMaterial);
	if (m_pTexture)
	{
		SAFE_DELETE_ARRAY(m_pMesh);
		SAFE_DELETE_ARRAY(m_pTexture);
	}
}

HRESULT D3DXModelClass::RenderXModelPMesh()
{
	for (int i = 0; i < m_wNumMtrl; i++)
	{
		m_pD3DDevice->SetMaterial(&m_pMaterial[i]);
		m_pD3DDevice->SetTexture(0, m_pTexture[i]);

		m_pPMesh->DrawSubset(i);//对网格数据子集进行绘制
	}

	return S_OK;
}

HRESULT D3DXModelClass::RenderXModelMesh()
{
	for (int i = 0; i < m_wNumMtrl; i++)
	{
		m_pD3DDevice->SetMaterial(&m_pMaterial[i]);
		m_pD3DDevice->SetTexture(0, m_pTexture[i]);

		//m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);//设置为线框模式
		m_pMesh->DrawSubset(i);//对网格数据子集进行绘制
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);//调回实体模式


	}
	return S_OK;
}

bool D3DXModelClass::Is_RayIntersected(Ray ray,DWORD& Faces)
{
	LPD3DXMESH pMesh;

	m_pMesh->CloneMeshFVF(m_pMesh->GetOptions(),
		XMODELVERTEX::FVF, m_pD3DDevice, &pMesh);

	LPDIRECT3DVERTEXBUFFER9 pVB;
	LPDIRECT3DINDEXBUFFER9 pIB;

	pMesh->GetVertexBuffer(&pVB);
	pMesh->GetIndexBuffer(&pIB);

	WORD* pIndices;
	XMODELVERTEX* pVertices;

	pIB->Lock(0, 0, (void**)&pIndices, 0);
	pVB->Lock(0, 0, (void**)&pVertices, 0);

	DWORD dwNumFaces = pMesh->GetNumFaces();
	FLOAT fBary1, fBary2;
	FLOAT fDist;
	for (DWORD i = 0; i < dwNumFaces; i++)
	{
		D3DXVECTOR3 v0 = pVertices[pIndices[3 * i + 0]].p * 300;
		D3DXVECTOR3 v1 = pVertices[pIndices[3 * i + 1]].p * 300;
		D3DXVECTOR3 v2 = pVertices[pIndices[3 * i + 2]].p * 300;
		D3DXVECTOR3 trans(2000, 5000, 2000);

		// Check if the pick ray passes through this point
		if (IntersectTriangle(ray.m_vOrigin-trans , ray.m_vDirection, v0, v1, v2,
			&fDist, &fBary1, &fBary2))
		{
			Faces = i;



			SAFE_RELEASE(m_pVerticesBuffer);
			SAFE_RELEASE(pIB);
			SAFE_RELEASE(pVB);
			return true;
		}
	}
	SAFE_RELEASE(m_pVerticesBuffer);
	SAFE_RELEASE(pIB);
	SAFE_RELEASE(pVB);
	return false;
}

bool D3DXModelClass::IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, FLOAT* t, FLOAT* u, FLOAT* v)
{
	D3DXVECTOR3 edge1 = v1 - v0;
	D3DXVECTOR3 edge2 = v2 - v0;

	D3DXVECTOR3 pvec;
	D3DXVec3Cross(&pvec, &dir, &edge2);

	FLOAT det = D3DXVec3Dot(&edge1, &pvec);

	D3DXVECTOR3 tvec;
	if (det > 0)
	{
		tvec = orig - v0;
	}
	else
	{
		tvec = v0 - orig;
		det = -det;
	}

	if (det < 0.0001f)
		return FALSE;

	*u = D3DXVec3Dot(&tvec, &pvec);
	if (*u < 0.0f || *u > det)
		return FALSE;

	D3DXVECTOR3 qvec;
	D3DXVec3Cross(&qvec, &tvec, &edge1);

	*v = D3DXVec3Dot(&dir, &qvec);
	if (*v < 0.0f || *u + *v > det)
		return FALSE;

	*t = D3DXVec3Dot(&edge2, &qvec);
	FLOAT fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return TRUE;
}

VOID D3DXModelClass::DisChangeMesh(float Dis)
{
	Dis = max(0, Dis);
	if (Dis > 8000)m_pPMesh->SetNumFaces(m_pPMesh->GetMinFaces());
	else m_pPMesh->SetNumFaces(m_pPMesh->GetMaxFaces());
}

HRESULT D3DXModelClass::LoadXModelFromFile(const wchar_t* strFileName)
{
	LPD3DXBUFFER pMtrlBuffer;
	LPD3DXBUFFER pAdjBuffer;

	D3DXLoadMeshFromX(strFileName, D3DXMESH_MANAGED, m_pD3DDevice, &pAdjBuffer, &pMtrlBuffer, NULL, &m_wNumMtrl, &m_pMesh);

	D3DXMATERIAL* pD3DMaterial;
	pD3DMaterial = (D3DXMATERIAL*)pMtrlBuffer->GetBufferPointer();
	m_pMaterial = new D3DMATERIAL9[m_wNumMtrl];
	m_pTexture = new LPDIRECT3DTEXTURE9[m_wNumMtrl];


	for (int i = 0; i < m_wNumMtrl; i++)
	{
		m_pMaterial[i] = pD3DMaterial[i].MatD3D;
		m_pMaterial[i].Ambient = m_pMaterial[i].Diffuse;

		m_pTexture[i] = NULL;
		if (pD3DMaterial[i].pTextureFilename != NULL && strlen(pD3DMaterial[i].pTextureFilename) > 0)
		{
			if (FAILED(D3DXCreateTextureFromFileA(m_pD3DDevice, pD3DMaterial[i].pTextureFilename, &m_pTexture[i])))
			{
				MessageBox(NULL, L"SORRY~!没有找到纹理文件!", L"XFileModelClass类读取文件错误", MB_OK);
			}
		}
	}


	if (FAILED(D3DXGeneratePMesh(m_pMesh, (DWORD*)pAdjBuffer->GetBufferPointer(), 0, 0, 1, D3DXMESHSIMP_FACE, &m_pPMesh)))
	{
		MessageBox(0, L"PMesh Failed", 0, 0);
		return E_FAIL;
	}
	DWORD maxFaces = m_pPMesh->GetMaxFaces();
	m_pPMesh->SetNumFaces(maxFaces);


	return S_OK;

}
