#include"D3DTerrainClass.h"

D3DTerrainClass::D3DTerrainClass(LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_pD3DDevice = pD3DDevice;
	m_fCellSpace = 0;
	m_fScaleMulti = 0;
	m_fTerrainLength = 0;
	m_fTerrainWidth = 0;
	m_nCellsPerCol = 0;
	m_nCellsPerRow = 0;
	m_nTotalVerticesNum = 0;
	m_nVerticesPerCol = 0;
	m_nVerticesPerRow = 0;
	m_pTerrainIndexBuffer = NULL;
	m_pTerrainTexture = NULL;
	m_pTerrainVertexBuffer = NULL;
}

D3DTerrainClass::~D3DTerrainClass()
{
	SAFE_RELEASE(m_pTerrainIndexBuffer);
	SAFE_RELEASE(m_pTerrainTexture);
	SAFE_RELEASE(m_pTerrainVertexBuffer);
}

BOOL D3DTerrainClass::LoadTerrainFromFile(const wchar_t* pLoadTerrainFileName,const wchar_t* pLoadTextureFileName)
{
	// ���ļ��ж�ȡ�߶���Ϣ
	std::ifstream inFile;
	inFile.open(pLoadTerrainFileName, std::ios::binary);   //�ö����Ƶķ�ʽ���ļ�

	inFile.seekg(0, std::ios::end);							//���ļ�ָ���ƶ����ļ�ĩβ
	std::vector<BYTE> inData(inFile.tellg());			//��ģ�嶨��һ��vector<BYTE>���͵ı���inData����ʼ������ֵΪ��������ǰλ�ã�����������С

	inFile.seekg(std::ios::beg);								//���ļ�ָ���ƶ����ļ��Ŀ�ͷ��׼����ȡ�߶���Ϣ
	inFile.read((char*)&inData[0], inData.size());	//�ؼ���һ������ȡ�����߶���Ϣ
	inFile.close();													//�������������Թر��ļ���

	m_vHeightInfo.resize(inData.size());					//��m_vHeightInfo�ߴ�ȡΪ�������ĳߴ�
	//������������������inData�е�ֵ����m_vHeightInfo
	for (unsigned int i = 0; i < inData.size(); i++)
		m_vHeightInfo[i] = inData[i];

	// ���ص�������
	if (FAILED(D3DXCreateTextureFromFile(m_pD3DDevice, pLoadTextureFileName, &m_pTerrainTexture)))
		return FALSE;	

	return TRUE;
}

BOOL D3DTerrainClass::InitTerrain(int nRows, int nCols, float fSpace, float fScales)
{
	m_nCellsPerCol = nCols;
	m_nCellsPerRow = nRows;
	m_fCellSpace = fSpace;
	m_fScaleMulti = fScales;

	m_nVerticesPerCol = nCols + 1;
	m_nVerticesPerRow = nRows + 1;	
	m_nTotalVerticesNum = m_nVerticesPerCol * m_nVerticesPerRow;

	m_fTerrainLength = m_nCellsPerRow * m_fCellSpace;
	m_fTerrainWidth = m_fCellSpace * m_nCellsPerCol;

	for (int i = 0; i < m_vHeightInfo[i]; i++)m_vHeightInfo[i] *= m_fScaleMulti;

	//1�������㻺��
	if (FAILED(m_pD3DDevice->CreateVertexBuffer(m_nTotalVerticesNum * sizeof(TERRAINVERTEX),
		D3DUSAGE_WRITEONLY, TERRAINVERTEX::FVF, D3DPOOL_MANAGED, &m_pTerrainVertexBuffer, 0)))
		return FALSE;

	//2����
	TERRAINVERTEX* pTerrainVertices;
	m_pTerrainVertexBuffer->Lock(0, 0, (void**)&pTerrainVertices, 0);

	float fStartZ = -m_fTerrainLength / 2, fEndZ = m_fTerrainLength / 2;
	float fStartX = -m_fTerrainWidth / 2, fEndX = m_fTerrainWidth / 2;

	float fCoordU = 1.0f / m_nCellsPerRow;
	float fCoordV = 1.0f / m_nCellsPerCol;
	
	int nIndex = 0, i = 0, j = 0;
	for (float x = fStartX; x < fEndX; i++, x += m_fCellSpace)
	{
		j = 0;
		for (float z = fStartZ; z < fEndZ; j++, z += m_fCellSpace)
		{
			nIndex = i * m_nCellsPerRow + j;
			pTerrainVertices[nIndex] = TERRAINVERTEX(x, m_vHeightInfo[nIndex], z, j * fCoordV, i * fCoordU);
		}
	}
	
	m_pTerrainVertexBuffer->Unlock();

	if (FAILED(m_pD3DDevice->CreateIndexBuffer(m_nTotalVerticesNum * 6 * sizeof(TERRAINVERTEX), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pTerrainIndexBuffer, 0)))return false;

	WORD* pIndices = NULL;
	m_pTerrainIndexBuffer->Lock(0, 0, (void**)&pIndices, 0);

	nIndex = 0;
	for (int col = 0; col < m_nCellsPerCol - 1; col++)   //����ÿ��
	{
		for (int row = 0; row < m_nCellsPerRow - 1; row++)  //����ÿ��
		{
			//������ABC����������
			pIndices[nIndex] = col * m_nCellsPerRow + row;			//����A
			pIndices[nIndex + 1] = col * m_nCellsPerRow + row + 1;
			pIndices[nIndex + 2] = (col + 1) * m_nCellsPerRow + row;
			//������CBD����������
			pIndices[nIndex + 3] = (col + 1) * m_nCellsPerRow + row;
			pIndices[nIndex + 4] = col * m_nCellsPerRow + row + 1;
			pIndices[nIndex + 5] = (col + 1) * m_nCellsPerRow + row + 1;
			//������һ����Ԫ����������6
			nIndex += 6;  //�����Լ�6

		}
	}

	m_pTerrainIndexBuffer->Unlock();



	return TRUE;

}

BOOL D3DTerrainClass::RenderTerrain(D3DXMATRIX* pMatrix, bool bDrawFrame = false)
{
	m_pD3DDevice->SetTransform(D3DTS_WORLD, pMatrix);

	m_pD3DDevice->SetStreamSource(0, m_pTerrainVertexBuffer, 0, sizeof(TERRAINVERTEX));
	m_pD3DDevice->SetFVF(TERRAINVERTEX::FVF);
	m_pD3DDevice->SetIndices(m_pTerrainIndexBuffer);


	m_pD3DDevice->SetTexture(0, m_pTerrainTexture);

	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	

	m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nTotalVerticesNum, 0, m_nTotalVerticesNum * 2);

	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, true);


	if (bDrawFrame)
	{
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);//����Ϊ�߿�ģʽ
		m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nTotalVerticesNum, 0, m_nTotalVerticesNum * 2);
		m_pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);//����ʵ��ģʽ
	}

	return TRUE;
}

float D3DTerrainClass::GetTerrainMapHeight(int row, int col)
{
	return m_vHeightInfo[row * m_nCellsPerRow + col];
}

float D3DTerrainClass::ComputeShade(int row, int col)
{
	D3DXVECTOR3 directionToLight;//��Դ����ĵ�λ������
	directionToLight.x = -1;
	directionToLight.y = 0;
	directionToLight.z = 0; 

	float heightA = GetTerrainMapHeight(row, col);
	float heightB = GetTerrainMapHeight(row, col + 1);
	float heightC = GetTerrainMapHeight(row + 1, col);
	
	//��������ƽ������
	D3DXVECTOR3 u(m_fCellSpace, heightB - heightA, 0.0f);
	D3DXVECTOR3 v(0.0f, heightC - heightA, -m_fCellSpace);
	
	//�õ�ƽ��ĵ�λ������
	D3DXVECTOR3 n;
	D3DXVec3Cross(&n, &u, &v);//�õ�u,v�Ĳ��
	D3DXVec3Normalize(&n, &n);//��n��λ��

	float cosine = D3DXVec3Dot(&n, &directionToLight);

	if (cosine < 0)cosine = 0;
	return cosine;
}

float D3DTerrainClass::GetTerrainSpace()
{
	return m_fCellSpace;
}

float D3DTerrainClass::GetHeight(float x, float z)
{
	x = m_fTerrainLength / 2.0f + x;
	z = m_fTerrainWidth / 2.0f + z;

	x /= m_fCellSpace;
	z /= m_fCellSpace;

	float row = ::floorf(x);
	float col = ::floorf(z);

	float A = GetTerrainMapHeight(row, col);
	float B = GetTerrainMapHeight(row, col + 1);
	float C = GetTerrainMapHeight(row + 1, col);
	float D = GetTerrainMapHeight(row + 1, col + 1);

	float dx = x - row;
	float dz = z - col;

	float height = 0.0f;
	if (dz < 1.0f - dx) 
	{
		float uy = B - A;
		float vy = C - A; 


		height = A + Lerp(0.0f, uy, dx) + Lerp(0.0f, vy, dz);
	}
	else 
	{
		float uy = C - D; 
		float vy = B - D;


		height = D + Lerp(0.0f, uy, 1.0f - dx) + Lerp(0.0f, vy, 1.0f - dz);
	}

	return height;
}

float D3DTerrainClass::Lerp(float a, float b, float t)
{
	return a - (a * t) + (b * t);
}




