#pragma once
#include<d3d9.h>
#include<d3dx9.h>

class D3DCameraClass
{
	//三个方向向量
	D3DXVECTOR3 m_vRightVector3;
	D3DXVECTOR3 m_vUpVector3;
	D3DXVECTOR3 m_vLookVector3;
	D3DXVECTOR3 m_vCameraPostionVector3;
	LPDIRECT3DDEVICE9 m_pCameraDevice;

	D3DXMATRIX m_mProj;//投影变换
	D3DXMATRIX m_mVeiw;//取景变换


public:
	D3DCameraClass(LPDIRECT3DDEVICE9);
	~D3DCameraClass();

	D3DXMATRIX Get_ProjMatrix();
	D3DXMATRIX Get_ViewMatrix();


	D3DXVECTOR3 Get_CameraPositionVector3();

	void Transform_Camera(D3DXMATRIX*);

	void Move_AlongX(float);
	void Move_AlongY(float);
	void Move_AlongZ(float);

	void Rotate_AlongX(float);
	void Rotate_AlongY(float);
	void Rotate_AlongZ(float);

	void Set_CameraPositionVector3(D3DXVECTOR3);
	void Set_ProjMatrix(D3DXMATRIX* pMatrix = NULL);
	void Set_VeiwMatrix(D3DXMATRIX* pMatrix = NULL);

	float GetDistance(D3DXVECTOR3 v);

	D3DXMATRIX Get_View();

};
