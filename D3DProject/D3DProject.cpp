// D3DProject.cpp : 定义应用程序的入口点。

#pragma region D3D头文件
#include<d3d9.h>
#include<d3dx9.h>
#include<time.h>
#include<tchar.h>
#include<dinput.h>
#include"D3DInputClass.h"
#include"D3DCameraClass.h"
#include"D3DTerrainClass.h" 
#include"D3DSkyBoxClass.h"
#include"D3DParticleClass.h"
#include"D3DXModelClass.h"
#include<algorithm>
#include"Assist.h"
#include<D3Dcommon.h>
#pragma endregion

#pragma region D3D链接文件
#pragma comment(lib,"winmm.lib")//调用playsound所需要的库文件
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma endregion

#define WND_WIDTH 1600
#define WND_HEIGHT 1024
#define WND_TITLE L"基于Direct3d的3维游戏模型渲染"
#define D3DFVF_CUSTOMVERTEX  (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

#pragma region SourcePath
#define SkyBoxFrontPath L"SkyBox\\TropicalSunnyDayFront2048.png"
#define SkyBoxBackPath L"SkyBox\\TropicalSunnyDayBack2048.png"
#define SkyBoxLeftPath L"SkyBox\\TropicalSunnyDayLeft2048.png"
#define SkyBoxRightPath L"SkyBox\\TropicalSunnyDayRight2048.png"
#define SkyBoxTopPath L"SkyBox\\TropicalSunnyDayUp2048.png"
#define TerrainTexturePath L"Terrain\\stone.jpg"
#define TerrainHeighMapPath L"Terrain\\heighmap.raw"
#pragma endregion

#pragma region 自定义宏
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }   //定义一个安全释放宏，便于后面COM接口指针的释放
#define HR(x) { hr = x; if( FALIED(hr) ){ return hr; }}// HR宏，方便执行语句错误的返回
#define SAFE_DELETE(x) {if(x){delete(x);(p)=NULL;}}// SAFE_DELETE宏，用于资源释放。
#pragma endregion

#pragma region D3D PARAM
//D3D初始化
LPDIRECT3DDEVICE9 g_pD3DDevice;
D3DXMATRIX g_matWorld;
LPD3DXMESH g_pCylinderMesh;
D3DMATERIAL9 g_mCylinderMaterial;
LPD3DXMESH g_pSphereMesh;
#pragma endregion

#pragma region D3DClass
D3DInputClass* g_pInputDevice;
D3DCameraClass* g_pCameraDevice;
D3DTerrainClass* g_pTerrain;
D3DSkyBoxClass* g_pSkyBox;
D3DParticleClass* g_pParticle;
D3DXModelClass* g_pXModelBigShip;
D3DXModelClass* g_pXModelAirPlane;
D3DXModelClass* g_pXModelWarrior;
struct BoundingSphere {
    D3DXVECTOR3 m_vCenter;
    float m_fRadius;
    BoundingSphere();
    BoundingSphere(D3DXVECTOR3 center, float radius) :m_vCenter(center), m_fRadius(radius) {};
};
#pragma endregion

#pragma region Assist PARAM
wchar_t g_strFps[50];
wchar_t g_strTips[200];
wchar_t g_strRayDirection[50];
wchar_t g_strAdapterName[60];
wchar_t g_strCameraPosition[60];
float fFps;
float g_fBigShipHeight = 5000;
float g_fBigShipDir = 1;

D3DXMATRIX g_matView;//观察矩阵

RECT formatRect;
POINT g_pCursorPos;
Ray g_rayCamera;

Ray ray;
bool Is_GroundedMode = true;
DWORD PickedFaces;



#pragma endregion

#pragma region D3D Function
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Direct3d_Init(HWND,HINSTANCE);//D3D初始化
VOID Direct3d_Update(HWND, FLOAT );//渲染
HRESULT Object_Init(HWND);//物体初始化
VOID Direct3d_CleanUp();
void Direct3d_InputUpdate(HWND, int nCmdShow);
void Direct3d_PhysicalUpdate();

Ray CalcRay(int x, int y);
void TransformRay(Ray* ray, D3DXMATRIX m);
#pragma endregion

#pragma region Assist Function
int Get_Fps();
VOID HelpTextPaint(HWND);
#pragma endregion

#pragma region Font PARAM
LPD3DXFONT g_pFpsText;
LPD3DXFONT g_pAdapterNameText;
LPD3DXFONT g_pTipsText;

#pragma endregion

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = (HICON)::LoadImage(NULL, L"Sai.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
    wcex.hCursor = LoadCursor(NULL, IDC_CROSS);
    wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"1";

    if (!RegisterClassEx(&wcex))			
        return -1;


    HWND hWnd = CreateWindowW(L"1", WND_TITLE,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WND_WIDTH,
        WND_HEIGHT, NULL, NULL, hInstance, NULL);


   if (Direct3d_Init(hWnd,hInstance)!=S_OK)MessageBox(NULL, L"D3D初始化失败！", L"ERROR!", 0);
   
    nCmdShow = SW_SHOWMAXIMIZED;
    MoveWindow(hWnd, 350, 160, WND_WIDTH, WND_HEIGHT, true);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd); 

    

    g_pInputDevice = new D3DInputClass();
    g_pInputDevice->Init(hInstance, hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
    

    //PlaySound(L"BGM\\glassy sky.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    // 主消息循环:
    MSG msg = { 0 };  //初始化msg
    while (msg.message != WM_QUIT)			//使用while循环
    {
        static FLOAT fLastTime = (float)::timeGetTime();
        static FLOAT fCurrTime = (float)::timeGetTime();
        static FLOAT fTimeDelta = 0.0f;

        fCurrTime = (float)::timeGetTime();
        fTimeDelta = (fCurrTime - fLastTime) / 1000.0f;
        fLastTime = fCurrTime;

        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
        {
            TranslateMessage(&msg);		//将虚拟键消息转换为字符消息
            DispatchMessage(&msg);		//该函数分发一个消息给窗口程序。
        }
        else
        {
            Direct3d_InputUpdate(hWnd, nCmdShow);
            Direct3d_PhysicalUpdate();
            Direct3d_Update(hWnd, fTimeDelta);
        }
    }
    UnregisterClass(L"1", wcex.hInstance);  //程序准备结束，注销窗口类

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
        {
        Direct3d_Update(hWnd, 0);
        ValidateRect(hWnd, NULL);//更新客户区显示
        }
        break;

    case WM_KEYDOWN:					
        if (wParam == VK_ESCAPE)  
            DestroyWindow(hWnd);	
        break;

    case WM_DESTROY:
        Direct3d_CleanUp();
        PostQuitMessage(0);
        break;
    

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);//调用缺省的窗口
    }
    return 0;
}

HRESULT Direct3d_Init(HWND hWnd,HINSTANCE hInstance)
{
    //D3D初始化第一步，创接口。
    LPDIRECT3D9 pD3D = NULL;
    pD3D = Direct3DCreate9(D3D_SDK_VERSION);    //获取D3D版本
    if (pD3D == NULL)return E_FAIL;//纠错

    //D3D初始化第二步，取信息。
    D3DCAPS9 caps; int vp = 0;
    pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    if (FAILED(pD3D))return E_FAIL;

    if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;//硬件顶点运算
    else vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;//软件定点运算

    //D3D初始化第三步，填写D3DPRESENT_PARAMETERS中的内容，填内容。
    D3DPRESENT_PARAMETERS d3dPP;
    d3dPP.BackBufferWidth = WND_WIDTH;
    d3dPP.BackBufferHeight = WND_HEIGHT;
    d3dPP.BackBufferFormat = D3DFMT_A8R8G8B8;//ALPHA值，RGB值都为8bit
    d3dPP.BackBufferCount =2;//定义后台缓冲数量

    d3dPP.MultiSampleQuality = 0;
    d3dPP.MultiSampleType = D3DMULTISAMPLE_NONE;

    d3dPP.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dPP.hDeviceWindow = hWnd;
    d3dPP.Windowed = false;//全屏显示
    d3dPP.EnableAutoDepthStencil = true;
    d3dPP.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dPP.Flags = 0;
    d3dPP.FullScreen_RefreshRateInHz = 0;
    d3dPP.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    //D3D初始化第四步，创设备
    pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &d3dPP, &g_pD3DDevice);
    if (FAILED(pD3D))return E_FAIL;


    wchar_t TempName[60] = L"当前显卡型号:";
    D3DADAPTER_IDENTIFIER9 adapter;
    pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapter);
    int len = MultiByteToWideChar(CP_ACP, 0, adapter.Description, -1, NULL, 0);
    MultiByteToWideChar(CP_ACP, 0, adapter.Description, -1, g_strAdapterName, len);
    wcscat_s(TempName, g_strAdapterName);
    wcscpy_s(g_strAdapterName, TempName);


    if (Object_Init(hWnd) != S_OK)return E_FAIL;



    SAFE_RELEASE(pD3D);
    return S_OK;
}

HRESULT Object_Init(HWND hWnd)
{
    D3DXCreateFont(g_pD3DDevice, 40, 0, 0, 1000, false,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"微软雅黑", &g_pAdapterNameText);//创建显卡字体

    D3DXCreateFont(g_pD3DDevice, 36, 0, 0, 1000, false,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"Calibri", &g_pFpsText);//创建FPS字体

    D3DXCreateFont(g_pD3DDevice, 25, 0, 0, 1000, false,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 0, L"Calibri", &g_pTipsText);

    

    //光照
    D3DLIGHT9 light;
    ::ZeroMemory(&light, sizeof(light));
    light.Type = D3DLIGHT_DIRECTIONAL;//设置为方向光也可设置点光源和聚光灯
    light.Diffuse = D3DXCOLOR(1.0f, 0.5f, 0.1f, 1.0f);//漫射光
    light.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);//镜面光
    light.Ambient = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);//环境光
    light.Direction = D3DXVECTOR3(1.0f, 0.0f, 0.0f);   //从X为10000处打光
    //注册光源
    g_pD3DDevice->SetLight(0, &light);
    g_pD3DDevice->LightEnable(0, true);

    g_pD3DDevice->SetRenderState(D3DRS_NORMALIZENORMALS, true);//使法向量规范化
    g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, true);//讲镜面反射设置为启动
   
    
    //BigShip
    g_pXModelBigShip = new D3DXModelClass(g_pD3DDevice);
    g_pXModelBigShip->LoadXModelFromFile(L"Character\\bigship1.X");

    //AirPlane
    g_pXModelAirPlane = new D3DXModelClass(g_pD3DDevice);
    g_pXModelAirPlane->LoadXModelFromFile(L"Character\\airplane 2.X");


    g_pXModelWarrior = new D3DXModelClass(g_pD3DDevice);
    g_pXModelWarrior->LoadXModelFromFile(L"Character\\soldier.X");



    //相机
    g_pCameraDevice = new D3DCameraClass(g_pD3DDevice);
    g_pCameraDevice->Set_CameraPositionVector3(D3DXVECTOR3(0.0f, 2500.0f, -300.0f));
    g_pCameraDevice->Set_ProjMatrix();
    g_pCameraDevice->Set_VeiwMatrix();

    //柱子
    D3DXCreateCylinder(g_pD3DDevice, 200, 200, 10000, 60, 60, &g_pCylinderMesh, 0);
    g_mCylinderMaterial.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    g_mCylinderMaterial.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
    g_mCylinderMaterial.Specular = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
    g_mCylinderMaterial.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

    //地形
    g_pTerrain = new D3DTerrainClass(g_pD3DDevice);
    g_pTerrain->LoadTerrainFromFile(TerrainHeighMapPath, TerrainTexturePath);
    g_pTerrain->InitTerrain(200, 200, 10.0f, 5.0f);

    //天空
    g_pSkyBox = new D3DSkyBoxClass(g_pD3DDevice);
    g_pSkyBox->LoadSkyTextureFromFile(SkyBoxFrontPath, SkyBoxBackPath, SkyBoxRightPath, SkyBoxLeftPath, SkyBoxTopPath);
    g_pSkyBox->InitSkyBox(20000);

    //粒子
    g_pParticle = new D3DParticleClass(g_pD3DDevice);
    g_pParticle->InitParticle();


    return S_OK;
}

VOID Direct3d_Update(HWND hWnd, FLOAT fTimeDelta)
{
    //D3D渲染第一步，清屏
    //g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(100, 0, 0, 0), 1.0f, 0);
    g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_XRGB(100, 255, 255), 1.0f, 0);//未被渲染的颜色


    //D3D渲染第二步，开始绘制
    g_pD3DDevice->BeginScene();

    //D3D渲染第三步，正式绘制
    g_pD3DDevice->LightEnable(0, true);

    //BigShip
    D3DXMATRIX matTransShip, matScalShip, matRotShip, matShip;
    D3DXMatrixScaling(&matScalShip, 100, 100, 100);//让飞机的长宽高按比例放大一伯倍
    D3DXMatrixTranslation(&matTransShip, 0, g_fBigShipHeight, 0);
    D3DXMatrixRotationY(&matRotShip, 0.0005f * timeGetTime());
    matShip = matScalShip * matTransShip * matRotShip * g_matWorld;
    if (g_fBigShipHeight > 9000)g_fBigShipDir = -g_fBigShipDir;
    else if(g_fBigShipHeight<4000)g_fBigShipDir = -g_fBigShipDir;
    g_fBigShipHeight += g_fBigShipDir * 0.0000001 * timeGetTime();
    g_pD3DDevice->SetTransform(D3DTS_WORLD, &matShip);//转换为世界坐标
    g_pXModelBigShip->RenderXModelPMesh();

    //AirPlane
    D3DXMATRIX matScalAir, matTransAir, matAir;
    D3DXMatrixScaling(&matScalAir, 300, 300, 300);//让飞机的长宽高按比例放大一伯倍
    D3DXMatrixTranslation(&matTransAir, 2000, 5000, 2000);
    matAir = matScalAir * matTransAir * g_matWorld;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, &matAir);
    g_pXModelAirPlane->RenderXModelMesh();

    D3DXMATRIX matScalwarr, matTranswarr, matwarr;
    D3DXMatrixScaling(&matScalwarr, 300, 300, 300);//让飞机的长宽高按比例放大一伯倍
    D3DXMatrixTranslation(&matTranswarr, -2000, 5000, 2000);
    matwarr = matScalwarr * matTranswarr * g_matWorld;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, &matwarr);
    g_pXModelWarrior->RenderXModelMesh();
    

    //柱子
    D3DXMATRIX matCy, matCyTrans,matCyRot;
    g_pD3DDevice->SetMaterial(&g_mCylinderMaterial);
    D3DXMatrixRotationX(&matCyRot, D3DX_PI / 2);
    D3DXMatrixTranslation(&matCyTrans, -3000, 5000, 3000);
    matCy = matCyRot * matCyTrans * g_matWorld;
    g_pD3DDevice->SetTransform(D3DTS_WORLD, &matCy);
    g_pCylinderMesh->DrawSubset(0);

    //地形
    D3DXMATRIX matTer, matTransTer, matScalTer;
    D3DXMatrixScaling(&matScalTer, 15, 2, 15);
    D3DXMatrixTranslation(&matTransTer, 0, 1000.0f, 0);
    matTer = matScalTer * matTransTer *  g_matWorld;
    g_pTerrain->RenderTerrain(&matTer, true);

    //天空
    D3DXMATRIX matSky, matTransSky, matRotSky;
    D3DXMatrixTranslation(&matTransSky, 0.0f, 0.0f, 0.0f);
    D3DXMatrixRotationY(&matRotSky, -0.00001f * timeGetTime());   //旋转天空网格, 简单模拟云彩运动效果
    matSky = matTransSky * matRotSky;
    g_pSkyBox->RenderSkyBox(&matSky, false);

    //粒子
    g_pParticle->UpdateParticle(fTimeDelta);
    g_pParticle->RenderParitcle();


    
    HelpTextPaint(hWnd);

    //D3D渲染第四步，结束绘制
    g_pD3DDevice->EndScene();

    //D3D渲染第五步，翻转显示
    g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void Direct3d_InputUpdate(HWND hWnd, int nCmdShow)
{
    g_pInputDevice->Get_Input();


    if (!Is_GroundedMode)
    {
        if (g_pInputDevice->Is_KeyDown(DIK_SPACE))g_pCameraDevice->Move_AlongY(+5);
        if (g_pInputDevice->Is_KeyDown(DIK_F))g_pCameraDevice->Move_AlongY(-5);
    }
        //键盘Input
        if (g_pInputDevice->Is_KeyDown(DIK_A))g_pCameraDevice->Move_AlongX(-3);
        if (g_pInputDevice->Is_KeyDown(DIK_D))g_pCameraDevice->Move_AlongX(+3);

        if (g_pInputDevice->Is_KeyDown(DIK_W))g_pCameraDevice->Move_AlongZ(+3);
        if (g_pInputDevice->Is_KeyDown(DIK_S))g_pCameraDevice->Move_AlongZ(-3);

        if (g_pInputDevice->Is_KeyDown(DIK_Q))g_pCameraDevice->Rotate_AlongZ(+0.004f);
        if (g_pInputDevice->Is_KeyDown(DIK_E))g_pCameraDevice->Rotate_AlongZ(-0.004f);

        if (g_pInputDevice->Is_KeyDown(DIK_T))Is_GroundedMode = !Is_GroundedMode;




    //鼠标
    g_pCameraDevice->Rotate_AlongY(g_pInputDevice->Get_MouseInputX() * 0.006f);
    g_pCameraDevice->Rotate_AlongX(g_pInputDevice->Get_MouseInputY() * 0.006f);

    //设置相机转换
    g_pCameraDevice->Transform_Camera(&g_matView);
    g_pD3DDevice->SetTransform(D3DTS_VIEW,&g_matView);

    D3DXMatrixTranslation(&g_matWorld, 0.0f, 0.0f, 0.0f);

    //以下这段代码用于限制鼠标光标移动区域
    POINT lt, rb;
    RECT rect;
    GetClientRect(hWnd, &rect);  //取得窗口内部矩形
    //将矩形左上点坐标存入lt中
    lt.x = rect.left;
    lt.y = rect.top;
    //将矩形右下坐标存入rb中
    rb.x = rect.right;
    rb.y = rect.bottom;
    //将lt和rb的窗口坐标转换为屏幕坐标
    ClientToScreen(hWnd, &lt);
    ClientToScreen(hWnd, &rb);
    //以屏幕坐标重新设定矩形区域
    rect.left = lt.x;
    rect.top = lt.y;
    rect.right = rb.x;
    rect.bottom = rb.y;
    ////限制鼠标光标移动区域
    //ClipCursor(&rect);

    g_pCursorPos.x = (rect.left + rect.right) >> 1;
    g_pCursorPos.y = (rect.top + rect.bottom) >> 1;
    SetCursorPos(g_pCursorPos.x, g_pCursorPos.y);

    POINT t;
    t.x = g_pCursorPos.x;
    t.y = g_pCursorPos.y;//得到屏幕坐标
    ScreenToClient(hWnd, &t);//转换为窗口坐标

    ray = CalcRay(t.x, t.y);
    D3DXMATRIX view;
    g_pD3DDevice->GetTransform(D3DTS_VIEW, &view);
    D3DXMATRIX viewInverse;
    D3DXMatrixInverse(&viewInverse, 0, &view);
    TransformRay(&ray, viewInverse);



    if (g_pXModelAirPlane->Is_RayIntersected(ray,PickedFaces)&&g_pInputDevice->Is_MouseDown(0))
    {
        MessageBox(hWnd, L"Shooted!", L"InterAct", 0);
    }
}

void Direct3d_PhysicalUpdate()
{

    float Dis_AirToCamera = g_pCameraDevice->GetDistance(D3DXVECTOR3(0, g_fBigShipHeight, 0));
    g_pXModelBigShip->DisChangeMesh((float)(Dis_AirToCamera - 1000.0f));

    if(Is_GroundedMode)
    {
        D3DXVECTOR3 eyes;
        eyes = g_pCameraDevice->Get_CameraPositionVector3();
        eyes.y = g_pTerrain->GetHeight(eyes.x / 15, eyes.z / 15) * 2 + 1000 + 50;
        g_pCameraDevice->Set_CameraPositionVector3(eyes);
    }

}

Ray CalcRay(int x, int y)
{
    D3DVIEWPORT9 vp;
    g_pD3DDevice->GetViewport(&vp);
    D3DXMATRIX proj;
    g_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);

    float px = (2.0f * x / vp.Width - 1.0f) / proj(0, 0);
    float py = (-2.0f * y / vp.Height + 1.0f) / proj(1, 1);

    Ray ray;
    ray.m_vOrigin = D3DXVECTOR3(0,0,0);
    ray.m_vDirection = D3DXVECTOR3(px, py, 1.0f);

    return ray;
}

void TransformRay(Ray* ray, D3DXMATRIX m)
{
    D3DXVec3TransformCoord(&(ray->m_vOrigin), &ray->m_vOrigin, &m);
    D3DXVec3TransformNormal(&ray->m_vDirection, &ray->m_vDirection, &m);
    D3DXVec3Normalize(&ray->m_vDirection, &ray->m_vDirection);

    /*ray->m_vDirection.x = ray->m_vDirection.x * m._11 + ray->m_vDirection.y * m._21 + ray->m_vDirection.z * m._31;
    ray->m_vDirection.y = ray->m_vDirection.x * m._12 + ray->m_vDirection.y * m._22 + ray->m_vDirection.z * m._32;
    ray->m_vDirection.z = ray->m_vDirection.x * m._13 + ray->m_vDirection.y * m._23 + ray->m_vDirection.z * m._33;
    ray->m_vOrigin.x = m._41;
    ray->m_vOrigin.y = m._42;
    ray->m_vOrigin.z = m._43;*/
}

void HelpTextPaint(HWND hWnd)
{
    GetClientRect(hWnd, &formatRect);
    int Tips;

    g_pAdapterNameText->DrawText(NULL, g_strAdapterName, -1, &formatRect,
        DT_TOP | DT_LEFT, D3DXCOLOR(1.0f, 0.5f, 0.5f, 0.5f));//绘制显卡信息
    formatRect.top += 40;
    g_pAdapterNameText->DrawText(NULL, L"Esc退出！", -1, &formatRect,
        DT_TOP | DT_LEFT, D3DXCOLOR(1.0, 0.0f, 1.0f, 0.5f));//绘制显卡信息

    formatRect.top += 40;
    int FpsCount = swprintf_s(g_strFps, 20, L"FPS: %d", Get_Fps());
    g_pFpsText->DrawText(NULL, g_strFps, FpsCount, &formatRect,
        DT_TOP | DT_LEFT, D3DXCOLOR(1.0, 0, 0, 1.0f));//绘制FPS信息

    formatRect.top += 40;
    Tips = swprintf_s(g_strTips, 200, L"CamerePos:(%.1f,%.1f,%.1f)", g_pCameraDevice->Get_CameraPositionVector3().x,
        g_pCameraDevice->Get_CameraPositionVector3().y, g_pCameraDevice->Get_CameraPositionVector3().z);
    g_pTipsText->DrawText(NULL, g_strTips, Tips, &formatRect,
        DT_TOP | DT_LEFT, D3DXCOLOR(1.0, 1.0, 0, 1.0f)); 

    formatRect.top += 40;
    Tips = swprintf_s(g_strTips, 200, L"扣T切换上帝视角", NULL);
    g_pTipsText->DrawText(NULL, g_strTips, Tips, &formatRect,
        DT_TOP | DT_LEFT, D3DXCOLOR(1.0, 1.0, 0, 1.0f));

    formatRect.top += 40;
    Tips = swprintf_s(g_strTips, 200, L"PickedFaces:%d",PickedFaces);
    g_pTipsText->DrawText(NULL, g_strTips, Tips, &formatRect,
        DT_TOP | DT_LEFT, D3DXCOLOR(1.0, 1.0, 0, 1.0f));


}

int Get_Fps()
{
    static int fps;
    static int frameCounter = 0;
    static float currentTime = 0;
    static float lastCountTime;

    frameCounter++;
    currentTime = timeGetTime() * 0.001;

    if (currentTime - lastCountTime > 1.0f)
    {
        fps = frameCounter /(currentTime - lastCountTime);
        lastCountTime = currentTime;
        frameCounter = 0;
    }

    return fps;
}

void Direct3d_CleanUp()
{
    SAFE_RELEASE(g_pAdapterNameText);
    SAFE_RELEASE(g_pD3DDevice);
    SAFE_RELEASE(g_pCylinderMesh);
    SAFE_RELEASE(g_pFpsText);
    SAFE_RELEASE(g_pSphereMesh);
    SAFE_RELEASE(g_pTipsText);
}