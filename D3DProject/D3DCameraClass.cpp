#include "D3DCameraClass.h"

#define WND_WIDTH 1200
#define WND_HEIGHT 800

D3DCameraClass::D3DCameraClass(LPDIRECT3DDEVICE9 pD3DDevice)
{
	m_pCameraDevice = pD3DDevice;
	m_vCameraPostionVector3 = D3DXVECTOR3(0,2000.0f,-600.0f);
	m_vRightVector3 = D3DXVECTOR3(1.0f, 0, 0);
	m_vUpVector3 = D3DXVECTOR3(0, 1.0f, 0);
	m_vLookVector3 = D3DXVECTOR3(0, 0, 1.0f);
}

void D3DCameraClass::Transform_Camera(D3DXMATRIX* pMatrix)
{
	D3DXVec3Normalize(&m_vLookVector3, &m_vLookVector3);
	D3DXVec3Cross(&m_vUpVector3, &m_vLookVector3, &m_vRightVector3);//注意叉乘顺序
	D3DXVec3Normalize(&m_vUpVector3, &m_vUpVector3);
	D3DXVec3Cross(&m_vRightVector3, &m_vUpVector3, &m_vLookVector3);//注意叉乘顺序
	D3DXVec3Normalize(&m_vRightVector3, &m_vRightVector3);


	//写出V矩阵
	pMatrix->_11 = m_vRightVector3.x;           // Rx
	pMatrix->_12 = m_vUpVector3.x;              // Ux
	pMatrix->_13 = m_vLookVector3.x;            // Lx
	pMatrix->_14 = 0.0f;
	//依次写出取景变换矩阵的第二行
	pMatrix->_21 = m_vRightVector3.y;           // Ry
	pMatrix->_22 = m_vUpVector3.y;              // Uy
	pMatrix->_23 = m_vLookVector3.y;            // Ly
	pMatrix->_24 = 0.0f;
	//依次写出取景变换矩阵的第三行
	pMatrix->_31 = m_vRightVector3.z;           // Rz
	pMatrix->_32 = m_vUpVector3.z;              // Uz
	pMatrix->_33 = m_vLookVector3.z;            // Lz
	pMatrix->_34 = 0.0f;
	//依次写出取景变换矩阵的第四行
	pMatrix->_41 = -D3DXVec3Dot(&m_vRightVector3, &m_vCameraPostionVector3);    // -P*R
	pMatrix->_42 = -D3DXVec3Dot(&m_vUpVector3, &m_vCameraPostionVector3);       // -P*U
	pMatrix->_43 = -D3DXVec3Dot(&m_vLookVector3, &m_vCameraPostionVector3);     // -P*L
	pMatrix->_44 = 1.0f;
}

void D3DCameraClass::Set_ProjMatrix(D3DXMATRIX* pMatrix)//投影变换FOV
{
	if (pMatrix != NULL)m_mProj = *pMatrix;
	else D3DXMatrixPerspectiveFovLH(&m_mProj, D3DX_PI / 1.8f, (float)((double)WND_WIDTH / WND_HEIGHT), 1.0f, 50000.0f);
	m_pCameraDevice->SetTransform(D3DTS_PROJECTION, &m_mProj);
}

void D3DCameraClass::Set_VeiwMatrix(D3DXMATRIX* pMatrix)
{
	if (pMatrix != NULL)m_mVeiw = *pMatrix;
	else Transform_Camera(&m_mVeiw);
	m_pCameraDevice->SetTransform(D3DTS_VIEW, &m_mVeiw);
}

float D3DCameraClass::GetDistance(D3DXVECTOR3 v)
{
	float dx = fabs(v.x - m_vCameraPostionVector3.x);
	float dy = fabs(v.y - m_vCameraPostionVector3.y);
	float dz = fabs(v.z - m_vCameraPostionVector3.z);
	float dis = sqrtf(dx * dx + dy * dy + dz * dz);
	return dis;
}

D3DXMATRIX D3DCameraClass::Get_View()
{
	return m_mVeiw;
}

void D3DCameraClass::Move_AlongX(float x)
{
	m_vCameraPostionVector3 += m_vRightVector3 * x;
}

void D3DCameraClass::Move_AlongY(float x)
{
	m_vCameraPostionVector3 += m_vUpVector3 * x;
}

void D3DCameraClass::Move_AlongZ(float x)
{
	m_vCameraPostionVector3 += m_vLookVector3 * x;
}

void D3DCameraClass::Set_CameraPositionVector3(D3DXVECTOR3 pvNewPostion)
{
	m_vCameraPostionVector3 = pvNewPostion;
}

void D3DCameraClass::Rotate_AlongX(float x)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_vRightVector3, x);

	D3DXVec3TransformCoord(&m_vUpVector3, &m_vUpVector3, &T);
	D3DXVec3TransformCoord(&m_vLookVector3, &m_vLookVector3, &T);
}

void D3DCameraClass::Rotate_AlongY(float x)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_vUpVector3, x);

	D3DXVec3TransformCoord(&m_vRightVector3, &m_vRightVector3, &T);
	D3DXVec3TransformCoord(&m_vLookVector3, &m_vLookVector3, &T);
}

void D3DCameraClass::Rotate_AlongZ(float x)
{
	D3DXMATRIX T;
	D3DXMatrixRotationAxis(&T, &m_vLookVector3, x);

	D3DXVec3TransformCoord(&m_vRightVector3, &m_vRightVector3, &T);
	D3DXVec3TransformCoord(&m_vUpVector3, &m_vUpVector3, &T);
}

D3DXVECTOR3 D3DCameraClass::Get_CameraPositionVector3()
{
	return m_vCameraPostionVector3;
}

D3DCameraClass::~D3DCameraClass()
{

}

D3DXMATRIX D3DCameraClass::Get_ProjMatrix()
{
	return m_mProj;
}

D3DXMATRIX D3DCameraClass::Get_ViewMatrix()
{
	return m_mVeiw;
}
