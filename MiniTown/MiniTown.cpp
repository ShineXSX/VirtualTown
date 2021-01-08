#include "MiniTown.h"
#include "Tex.h"
#include "DXTrace.h"
using namespace DirectX;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;
	Render::InitAll(m_pd3dDevice.Get());

	//初始化基本特效
	if (!m_BasicEffect.InitAll(m_pd3dDevice.Get()))
		return false;

	//初始化天空盒特效
	if (!m_SkyEffect.InitAll(m_pd3dDevice.Get()))
		return false;

	if (!InitResource())
		return false;

	//初始化鼠标
	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);

	return true;
}

void GameApp::OnResize()
{
	assert(m_pd2dFactory);
	assert(m_pdwriteFactory);
	D3DApp::OnResize();
	// 为D2D创建DXGI表面渲染目标
	ComPtr<IDXGISurface> surface;
	HR(m_pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, m_pd2dRenderTarget.GetAddressOf());
	surface.Reset();


	if (hr == S_OK)
	{
		//确定文字格式
		HR(m_pd2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			m_pColorBrush.GetAddressOf()));
		HR(m_pdwriteFactory->CreateTextFormat(L"黑体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 15, L"zh-cn",
			m_pTextFormat.GetAddressOf()));
	}
	else
	{
		assert(m_pd2dRenderTarget);
	}

	//镜头变更显示
	if (m_Camera != nullptr)
	{
		m_Camera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
		m_Camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
		m_BasicEffect.SetProjMatrix(m_Camera->GetProjXM());
	}
}

void GameApp::UpdateScene(float dt)
{

	//-----------------------------------------------
	//镜头操作
	//-----------------------------------------------

	//鼠标调整视角
	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	auto cam1st = std::dynamic_pointer_cast<CamControl>(m_Camera);

	m_pMouse->ResetScrollWheelValue();

	//方向移动(按住shift会加速)

	Keyboard::State keyState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyState);

	if (keyState.IsKeyDown(Keyboard::W))
		cam1st->MoveForward(dt * 6.0f);
	if (keyState.IsKeyDown(Keyboard::W)&& keyState.IsKeyDown(Keyboard::LeftShift))
		cam1st->MoveForward(dt * 10.0f);

	if (keyState.IsKeyDown(Keyboard::S))
		cam1st->MoveForward(dt * -6.0f);
	if (keyState.IsKeyDown(Keyboard::S) && keyState.IsKeyDown(Keyboard::LeftShift))
		cam1st->MoveForward(dt * -10.0f);

	if (keyState.IsKeyDown(Keyboard::A))
		cam1st->Move(dt * -6.0f);
	if (keyState.IsKeyDown(Keyboard::A) && keyState.IsKeyDown(Keyboard::LeftShift))
		cam1st->Move(dt * -10.0f);

	if (keyState.IsKeyDown(Keyboard::D))
		cam1st->Move(dt * 6.0f);
	if (keyState.IsKeyDown(Keyboard::D) && keyState.IsKeyDown(Keyboard::LeftShift))
		cam1st->Move(dt * 10.0f);

	if (mouseState.positionMode == Mouse::MODE_RELATIVE)
	{
		cam1st->UpSpot(mouseState.y * dt * 1.25f);
		cam1st->RightSpot(mouseState.x * dt * 1.25f);
	}
	
	//更新观察矩阵
	m_BasicEffect.SetViewMatrix(m_Camera->GetViewXM());
	m_BasicEffect.SetEyePos(m_Camera->GetPosition());

	//退出
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::F12))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
}

void GameApp::DrawScene()
{
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	//-----------------------------------------------
	//3D场景绘制
	//-----------------------------------------------

	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::Black));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_BasicEffect.SetRenderDefault(m_pd3dImmediateContext.Get(), BasicEffect::RenderObject);
	m_BasicEffect.SetReflectionEnabled(true);
	m_BasicEffect.SetTextureUsed(true);
	m_BasicEffect.SetReflectionEnabled(false);

	m_Ground.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	m_Tree1.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Tree2.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Tree3.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Tree4.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Tree5.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Tree6.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Tree7.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Tree8.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Tree9.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Tree10.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	m_Warehouse1.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Warehouse2.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_House1.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_House2.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_House3.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_House4.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Windmill.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	m_Roadc1.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadc2.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadc3.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadc4.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	m_Roadt1.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadt2.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadt3.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadt4.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	m_Roadl1.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl2.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl3.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl4.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl5.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl6.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl7.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl8.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl9.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl10.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl11.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadl12.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	m_Roadr1.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr2.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr3.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr4.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr5.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr6.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr7.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr8.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr9.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr10.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr11.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);
	m_Roadr12.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	m_Roadx.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	m_Car.Draw(m_pd3dImmediateContext.Get(), m_BasicEffect);

	//天空盒绘制
	m_SkyEffect.SetRenderDefault(m_pd3dImmediateContext.Get());
	m_SkyBox->Draw(m_pd3dImmediateContext.Get(), m_SkyEffect, *m_Camera);
	


	//-----------------------------------------------
	//加载文字
	//-----------------------------------------------
	if (m_pd2dRenderTarget != nullptr)
	{
		m_pd2dRenderTarget->BeginDraw();
		float fps = CalcFPS();
		std::wstring text = 
			L" W/S/A/D移动(同时按下shift加速)\n"
			L" F12退出";
			
		m_pd2dRenderTarget->DrawTextW(text.c_str(), (UINT32)text.length(), m_pTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, m_pColorBrush.Get());
		HR(m_pd2dRenderTarget->EndDraw());
	}

	HR(m_pSwapChain->Present(0, 0));
}



bool GameApp::InitResource()
{
	
	//----------------------------------------
	//初始化model
	//----------------------------------------
	
	Model model;
	

	// 地面
	model.SetMesh(m_pd3dDevice.Get(), Geometry::CreatePlane(XMFLOAT2(300.0f, 300.0f), XMFLOAT2(5.0f, 5.0f)));
	model.modelParts[0].material.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	model.modelParts[0].material.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	model.modelParts[0].material.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f); 
	model.modelParts[0].material.reflect = XMFLOAT4();
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(),
		L"..\\Model\\terrain\\grass.dds",
		nullptr,
		model.modelParts[0].texDiffuse.GetAddressOf()));
	m_Ground.SetModel(std::move(model));
	m_Ground.GetTransformation().SetPosition(0.0f, -1.0f, 0.0f);
	
	//10棵树
	m_ObjLoader.LoadObj(L"..\\Model\\trees\\tree.obj");
	m_Tree1.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_tree1 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox tree1Box = m_Tree1.GetLocalBoundingBox();
	tree1Box.Transform(tree1Box, s_tree1);
	Transformation& tree1Transformation = m_Tree1.GetTransformation();
	tree1Transformation.SetScale(0.115f, 0.115f, 0.115f);
	tree1Transformation.SetPosition(-10.0f, -(tree1Box.Center.y - tree1Box.Extents.y + 1.0f), -20.0f);

	// 初始化树2
	m_ObjLoader.LoadObj(L"..\\Model\\trees\\tree.obj");
	m_Tree2.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_tree2 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox tree2Box = m_Tree2.GetLocalBoundingBox();
	tree2Box.Transform(tree2Box, s_tree2);
	Transformation& tree2Transformation = m_Tree2.GetTransformation();
	tree2Transformation.SetScale(0.115f, 0.115f, 0.115f);
	tree2Transformation.SetPosition(-5.0f, -(tree2Box.Center.y - tree2Box.Extents.y + 1.0f), -20.0f);

	//初始化树3
	m_ObjLoader.LoadObj(L"..\\Model\\trees\\tree.obj");
	m_Tree3.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_tree3 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox tree3Box = m_Tree3.GetLocalBoundingBox();
	tree3Box.Transform(tree3Box, s_tree3);
	Transformation& tree3Transformation = m_Tree3.GetTransformation();
	tree3Transformation.SetScale(0.115f, 0.115f, 0.115f);
	tree3Transformation.SetPosition(0.0f, -(tree3Box.Center.y - tree3Box.Extents.y + 1.0f), -20.0f);

	//初始化树4
	m_ObjLoader.LoadObj(L"..\\Model\\trees\\tree.obj");
	m_Tree4.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_tree4 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox tree4Box = m_Tree4.GetLocalBoundingBox();
	tree4Box.Transform(tree4Box, s_tree4);
	Transformation& tree4Transformation = m_Tree4.GetTransformation();
	tree4Transformation.SetScale(0.115f, 0.115f, 0.115f);
	tree4Transformation.SetPosition(5.0f, -(tree4Box.Center.y - tree4Box.Extents.y + 1.0f), -20.0f);

	//初始化树5
	m_ObjLoader.LoadObj(L"..\\Model\\trees\\tree.obj");
	m_Tree5.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_tree5 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox tree5Box = m_Tree5.GetLocalBoundingBox();
	tree5Box.Transform(tree5Box, s_tree5);
	Transformation& tree5Transformation = m_Tree5.GetTransformation();
	tree5Transformation.SetScale(0.115f, 0.115f, 0.115f);
	tree5Transformation.SetPosition(10.0f, -(tree5Box.Center.y - tree5Box.Extents.y + 1.0f), -20.0f);

	// 初始化第二排tree5
	m_ObjLoader.LoadObj(L"..\\Model\\trees\\tree.obj");
	m_Tree6.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	//获取树木包围盒
	XMMATRIX s_tree6 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox tree6Box = m_Tree6.GetLocalBoundingBox();
	tree6Box.Transform(tree6Box, s_tree6);
	//防止浮空并调整位置
	Transformation& tree6Transformation = m_Tree6.GetTransformation();
	tree6Transformation.SetScale(0.115f, 0.115f, 0.115f);
	tree6Transformation.SetPosition(-10.0f, -(tree6Box.Center.y - tree6Box.Extents.y + 1.0f), 20.0f);

	// 初始化树2
	m_ObjLoader.LoadObj(L"..\\Model\\trees\\tree.obj");
	m_Tree7.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_tree7 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox tree7Box = m_Tree7.GetLocalBoundingBox();
	tree7Box.Transform(tree7Box, s_tree7);
	Transformation& tree7Transformation = m_Tree7.GetTransformation();
	tree7Transformation.SetScale(0.115f, 0.115f, 0.115f);
	tree7Transformation.SetPosition(-5.0f, -(tree7Box.Center.y - tree7Box.Extents.y + 1.0f), 20.0f);

	//初始化树3
	m_ObjLoader.LoadObj(L"..\\Model\\trees\\tree.obj");
	m_Tree8.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_tree8 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox tree8Box = m_Tree8.GetLocalBoundingBox();
	tree8Box.Transform(tree8Box, s_tree8);
	Transformation& tree8Transformation = m_Tree8.GetTransformation();
	tree8Transformation.SetScale(0.115f, 0.115f, 0.115f);
	tree8Transformation.SetPosition(0.0f, -(tree8Box.Center.y - tree8Box.Extents.y + 1.0f), 20.0f);

	//初始化树4
	m_ObjLoader.LoadObj(L"..\\Model\\trees\\tree.obj");
	m_Tree9.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_tree9 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox tree9Box = m_Tree9.GetLocalBoundingBox();
	tree9Box.Transform(tree9Box, s_tree9);
	Transformation& tree9Transformation = m_Tree9.GetTransformation();
	tree9Transformation.SetScale(0.115f, 0.115f, 0.115f);
	tree9Transformation.SetPosition(5.0f, -(tree9Box.Center.y - tree9Box.Extents.y + 1.0f), 20.0f);

	//初始化树5
	m_ObjLoader.LoadObj(L"..\\Model\\trees\\tree.obj");
	m_Tree10.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_tree10 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox tree10Box = m_Tree10.GetLocalBoundingBox();
	tree10Box.Transform(tree10Box, s_tree10);
	Transformation& tree10Transformation = m_Tree10.GetTransformation();
	tree10Transformation.SetScale(0.115f, 0.115f, 0.115f);
	tree10Transformation.SetPosition(10.0f, -(tree10Box.Center.y - tree10Box.Extents.y + 1.0f), 20.0f);

	//2个厂房
	m_ObjLoader.LoadObj(L"..\\Model\\buildings\\warehouse.obj");
	m_Warehouse1.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_warehouse1 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox warehouse1Box = m_Warehouse1.GetLocalBoundingBox();
	warehouse1Box.Transform(warehouse1Box, s_warehouse1);
	Transformation& warehouse1Transformation = m_Warehouse1.GetTransformation();
	warehouse1Transformation.SetScale(0.025f, 0.025f, 0.025f);
	warehouse1Transformation.SetPosition(-22.5f, -0.45f, 7.5f);
	warehouse1Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\buildings\\warehouse.obj");
	m_Warehouse2.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_warehouse2 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox warehouse2Box = m_Warehouse2.GetLocalBoundingBox();
	warehouse2Box.Transform(warehouse2Box, s_warehouse2);
	Transformation& warehouse2Transformation = m_Warehouse2.GetTransformation();
	warehouse2Transformation.SetScale(0.025f, 0.025f, 0.025f);
	warehouse2Transformation.SetPosition(-22.5f, -0.45f, -7.5f);
	warehouse2Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);
	//4个house
	m_ObjLoader.LoadObj(L"..\\Model\\buildings\\house.obj");
	m_House1.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_house1 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox house1Box = m_House1.GetLocalBoundingBox();
	house1Box.Transform(house1Box, s_house1);
	Transformation& house1Transformation = m_House1.GetTransformation();
	house1Transformation.SetScale(0.1f, 0.1f, 0.1f);
	house1Transformation.SetPosition(-7.5f, -0.3f, -7.5f);

	m_ObjLoader.LoadObj(L"..\\Model\\buildings\\house.obj");
	m_House2.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_house2 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox house2Box = m_House2.GetLocalBoundingBox();
	house2Box.Transform(house2Box, s_house2);
	Transformation& house2Transformation = m_House2.GetTransformation();
	house2Transformation.SetScale(0.1f, 0.1f, 0.1f);
	house2Transformation.SetPosition(-7.5f, -0.3f, 7.5f);

	m_ObjLoader.LoadObj(L"..\\Model\\buildings\\house.obj");
	m_House3.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_house3 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox house3Box = m_House3.GetLocalBoundingBox();
	house3Box.Transform(house3Box, s_house3);
	Transformation& house3Transformation = m_House3.GetTransformation();
	house3Transformation.SetScale(0.1f, 0.1f, 0.1f);
	house3Transformation.SetPosition(7.5f, -0.3f, -7.5f);

	m_ObjLoader.LoadObj(L"..\\Model\\buildings\\house.obj");
	m_House4.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_house4 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox house4Box = m_House4.GetLocalBoundingBox();
	house4Box.Transform(house4Box, s_house4);
	Transformation& house4Transformation = m_House4.GetTransformation();
	house4Transformation.SetScale(0.1f, 0.1f, 0.1f);
	house4Transformation.SetPosition(7.5f, -0.3f, 7.5f);

	//1个磨坊
	m_ObjLoader.LoadObj(L"..\\Model\\buildings\\windmill.obj");
	m_Windmill.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_windmill = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox windmillBox = m_Windmill.GetLocalBoundingBox();
	windmillBox.Transform(windmillBox, s_windmill);
	Transformation& windmillTransformation = m_Windmill.GetTransformation();
	windmillTransformation.SetScale(0.20f, 0.20f, 0.20f);
	windmillTransformation.SetPosition(22.5f, 9.0f, 0.0f);
	windmillTransformation.SetRotationAngle(0.0f, 3.16f, 0.0f);

	//4个corner 
	//左下角
	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_corner.obj");
	m_Roadc1.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadc1 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadc1Box = m_Roadc1.GetLocalBoundingBox();
	roadc1Box.Transform(roadc1Box, s_roadc1);
	Transformation& roadc1Transformation = m_Roadc1.GetTransformation();
	roadc1Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadc1Transformation.SetPosition(-15.0f, -1.0f, -15.0f);
	//左上角
	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_corner.obj");
	m_Roadc2.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadc2 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadc2Box = m_Roadc2.GetLocalBoundingBox();
	roadc2Box.Transform(roadc2Box, s_roadc2);
	Transformation& roadc2Transformation = m_Roadc2.GetTransformation();
	roadc2Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadc2Transformation.SetPosition(-15.0f, -1.0f, 15.0f);
	roadc2Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);
	//右下角
	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_corner.obj");
	m_Roadc3.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadc3 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadc3Box = m_Roadc3.GetLocalBoundingBox();
	roadc3Box.Transform(roadc3Box, s_roadc3);
	Transformation& roadc3Transformation = m_Roadc3.GetTransformation();
	roadc3Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadc3Transformation.SetPosition(15.0f, -1.0f, -15.0f);
	roadc3Transformation.SetRotationAngle(0.0f, 4.74f, 0.0f);
	//右上角
	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_corner.obj");
	m_Roadc4.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadc4 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadc4Box = m_Roadc4.GetLocalBoundingBox();
	roadc4Box.Transform(roadc4Box, s_roadc4);
	Transformation& roadc4Transformation = m_Roadc4.GetTransformation();
	roadc4Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadc4Transformation.SetPosition(15.0f, -1.0f, 15.0f);
	roadc4Transformation.SetRotationAngle(0.0f, 3.16f, 0.0f);
	//12个line
	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl1.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl1 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl1Box = m_Roadl1.GetLocalBoundingBox();
	roadl1Box.Transform(roadl1Box, s_roadl1);
	Transformation& roadl1Transformation = m_Roadl1.GetTransformation();
	roadl1Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl1Transformation.SetPosition(-15.0f, -1.0f, -5.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl2.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl2 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl2Box = m_Roadl2.GetLocalBoundingBox();
	roadl2Box.Transform(roadl2Box, s_roadl2);
	Transformation& roadl2Transformation = m_Roadl2.GetTransformation();
	roadl2Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl2Transformation.SetPosition(-15.0f, -1.0f, 5.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl3.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl3 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl3Box = m_Roadl3.GetLocalBoundingBox();
	roadl3Box.Transform(roadl3Box, s_roadl3);
	Transformation& roadl3Transformation = m_Roadl3.GetTransformation();
	roadl3Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl3Transformation.SetPosition(15.0f, -1.0f, -5.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl4.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl4 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl4Box = m_Roadl4.GetLocalBoundingBox();
	roadl4Box.Transform(roadl4Box, s_roadl4);
	Transformation& roadl4Transformation = m_Roadl4.GetTransformation();
	roadl4Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl4Transformation.SetPosition(15.0f, -1.0f, 5.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl5.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl5 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl5Box = m_Roadl5.GetLocalBoundingBox();
	roadl5Box.Transform(roadl5Box, s_roadl5);
	Transformation& roadl5Transformation = m_Roadl5.GetTransformation();
	roadl5Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl5Transformation.SetPosition(0.0f, -1.0f, -5.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl6.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl6 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl6Box = m_Roadl6.GetLocalBoundingBox();
	roadl6Box.Transform(roadl6Box, s_roadl6);
	Transformation& roadl6Transformation = m_Roadl6.GetTransformation();
	roadl6Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl6Transformation.SetPosition(0.0f, -1.0f, 5.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl7.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl7 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl7Box = m_Roadl7.GetLocalBoundingBox();
	roadl7Box.Transform(roadl7Box, s_roadl7);
	Transformation& roadl7Transformation = m_Roadl7.GetTransformation();
	roadl7Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl7Transformation.SetPosition(-15.0f, -1.0f, 10.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl8.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl8 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl8Box = m_Roadl8.GetLocalBoundingBox();
	roadl8Box.Transform(roadl8Box, s_roadl8);
	Transformation& roadl8Transformation = m_Roadl8.GetTransformation();
	roadl8Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl8Transformation.SetPosition(0.0f, -1.0f, 10.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl9.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl9 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl9Box = m_Roadl9.GetLocalBoundingBox();
	roadl9Box.Transform(roadl9Box, s_roadl9);
	Transformation& roadl9Transformation = m_Roadl9.GetTransformation();
	roadl9Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl9Transformation.SetPosition(15.0f, -1.0f, 10.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl10.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl10 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl10Box = m_Roadl10.GetLocalBoundingBox();
	roadl10Box.Transform(roadl10Box, s_roadl10);
	Transformation& roadl10Transformation = m_Roadl10.GetTransformation();
	roadl10Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl10Transformation.SetPosition(-15.0f, -1.0f, -10.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl11.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl11 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl11Box = m_Roadl11.GetLocalBoundingBox();
	roadl11Box.Transform(roadl11Box, s_roadl11);
	Transformation& roadl11Transformation = m_Roadl11.GetTransformation();
	roadl11Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl11Transformation.SetPosition(0.0f, -1.0f, -10.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadl12.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadl12 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadl12Box = m_Roadl12.GetLocalBoundingBox();
	roadl12Box.Transform(roadl12Box, s_roadl12);
	Transformation& roadl12Transformation = m_Roadl12.GetTransformation();
	roadl12Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadl12Transformation.SetPosition(15.0f, -1.0f, -10.0f);
	//12个row
	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr1.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr1 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr1Box = m_Roadr1.GetLocalBoundingBox();
	roadr1Box.Transform(roadr1Box, s_roadr1);
	Transformation& roadr1Transformation = m_Roadr1.GetTransformation();
	roadr1Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr1Transformation.SetPosition(-10.0f, -1.0f, 15.0f);
	roadr1Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr2.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr2 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr2Box = m_Roadr2.GetLocalBoundingBox();
	roadr2Box.Transform(roadr2Box, s_roadr2);
	Transformation& roadr2Transformation = m_Roadr2.GetTransformation();
	roadr2Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr2Transformation.SetPosition(-5.0f, -1.0f, 15.0f);
	roadr2Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr3.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr3 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr3Box = m_Roadr3.GetLocalBoundingBox();
	roadr3Box.Transform(roadr3Box, s_roadr3);
	Transformation& roadr3Transformation = m_Roadr3.GetTransformation();
	roadr3Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr3Transformation.SetPosition(5.0f, -1.0f, 15.0f);
	roadr3Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr4.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr4 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr4Box = m_Roadr4.GetLocalBoundingBox();
	roadr4Box.Transform(roadr4Box, s_roadr4);
	Transformation& roadr4Transformation = m_Roadr4.GetTransformation();
	roadr4Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr4Transformation.SetPosition(10.0f, -1.0f, 15.0f);
	roadr4Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr5.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr5 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr5Box = m_Roadr5.GetLocalBoundingBox();
	roadr5Box.Transform(roadr5Box, s_roadr5);
	Transformation& roadr5Transformation = m_Roadr5.GetTransformation();
	roadr5Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr5Transformation.SetPosition(-10.0f, -1.0f, 0.0f);
	roadr5Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr6.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr6 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr6Box = m_Roadr6.GetLocalBoundingBox();
	roadr6Box.Transform(roadr6Box, s_roadr6);
	Transformation& roadr6Transformation = m_Roadr6.GetTransformation();
	roadr6Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr6Transformation.SetPosition(-5.0f, -1.0f, 0.0f);
	roadr6Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr7.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr7 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr7Box = m_Roadr7.GetLocalBoundingBox();
	roadr7Box.Transform(roadr7Box, s_roadr7);
	Transformation& roadr7Transformation = m_Roadr7.GetTransformation();
	roadr7Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr7Transformation.SetPosition(5.0f, -1.0f, 0.0f);
	roadr7Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr8.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr8 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr8Box = m_Roadr8.GetLocalBoundingBox();
	roadr8Box.Transform(roadr8Box, s_roadr8);
	Transformation& roadr8Transformation = m_Roadr8.GetTransformation();
	roadr8Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr8Transformation.SetPosition(10.0f, -1.0f, 0.0f);
	roadr8Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr9.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr9 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr9Box = m_Roadr9.GetLocalBoundingBox();
	roadr9Box.Transform(roadr9Box, s_roadr9);
	Transformation& roadr9Transformation = m_Roadr9.GetTransformation();
	roadr9Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr9Transformation.SetPosition(-10.0f, -1.0f, -15.0f);
	roadr9Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr10.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr10 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr10Box = m_Roadr10.GetLocalBoundingBox();
	roadr10Box.Transform(roadr10Box, s_roadr10);
	Transformation& roadr10Transformation = m_Roadr10.GetTransformation();
	roadr10Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr10Transformation.SetPosition(-5.0f, -1.0f, -15.0f);
	roadr10Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr11.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr11 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr11Box = m_Roadr11.GetLocalBoundingBox();
	roadr11Box.Transform(roadr11Box, s_roadr11);
	Transformation& roadr11Transformation = m_Roadr11.GetTransformation();
	roadr11Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr11Transformation.SetPosition(5.0f, -1.0f, -15.0f);
	roadr11Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_line.obj");
	m_Roadr12.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadr12 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadr12Box = m_Roadr12.GetLocalBoundingBox();
	roadr12Box.Transform(roadr12Box, s_roadr12);
	Transformation& roadr12Transformation = m_Roadr12.GetTransformation();
	roadr12Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadr12Transformation.SetPosition(10.0f, -1.0f, -15.0f);
	roadr12Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);
	//4个T
	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_T.obj");
	m_Roadt1.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadt1 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadt1Box = m_Roadt1.GetLocalBoundingBox();
	roadt1Box.Transform(roadt1Box, s_roadt1);
	Transformation& roadt1Transformation = m_Roadt1.GetTransformation();
	roadt1Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadt1Transformation.SetPosition(-15.0f, -1.0f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_T.obj");
	m_Roadt2.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadt2 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadt2Box = m_Roadt2.GetLocalBoundingBox();
	roadt2Box.Transform(roadt2Box, s_roadt2);
	Transformation& roadt2Transformation = m_Roadt2.GetTransformation();
	roadt2Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadt2Transformation.SetPosition(15.0f, -1.0f, 0.0f);
	roadt2Transformation.SetRotationAngle(0.0f, 3.16f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_T.obj");
	m_Roadt3.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadt3 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadt3Box = m_Roadt3.GetLocalBoundingBox();
	roadt3Box.Transform(roadt3Box, s_roadt3);
	Transformation& roadt3Transformation = m_Roadt3.GetTransformation();
	roadt3Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadt3Transformation.SetPosition(0.0f, -1.0f, 15.0f);
	roadt3Transformation.SetRotationAngle(0.0f, 1.58f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_T.obj");
	m_Roadt4.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadt4 = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadt4Box = m_Roadt4.GetLocalBoundingBox();
	roadt4Box.Transform(roadt4Box, s_roadt4);
	Transformation& roadt4Transformation = m_Roadt4.GetTransformation();
	roadt4Transformation.SetScale(0.13f, 0.13f, 0.13f);
	roadt4Transformation.SetPosition(0.0f, -1.0f, -15.0f);
	roadt4Transformation.SetRotationAngle(0.0f, 4.74f, 0.0f);
	//1个X
	m_ObjLoader.LoadObj(L"..\\Model\\street_tile\\street_cross.obj");
	m_Roadx.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_roadx = XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox roadxBox = m_Roadx.GetLocalBoundingBox();
	roadxBox.Transform(roadxBox, s_roadx);
	Transformation& roadxTransformation = m_Roadx.GetTransformation();
	roadxTransformation.SetScale(0.13f, 0.13f, 0.13f);
	roadxTransformation.SetPosition(0.0f, -1.0f, 0.0f);

	m_ObjLoader.LoadObj(L"..\\Model\\cars\\1car.obj");
	m_Car.SetModel(Model(m_pd3dDevice.Get(), m_ObjLoader));
	XMMATRIX s_car=XMMatrixScaling(0.525f, 0.515f, 0.515f);
	BoundingBox carBox = m_Car.GetLocalBoundingBox();
	carBox.Transform(carBox, s_car);
	Transformation& carTransformation = m_Car.GetTransformation();
	carTransformation.SetScale(0.13f, 0.13f, 0.13f);
	carTransformation.SetPosition(-15.0f, -1.0f, 15.0f);
	carTransformation.SetRotationAngle(0.0f, -1.58f, 0.0f);

	//-----------------------------------------------
	//初始化天空盒
	//-----------------------------------------------
	m_SkyBox = std::make_unique<SkyBox>();
	HR(m_SkyBox->InitSkyBox(m_pd3dDevice.Get(), m_pd3dImmediateContext.Get(),
		L"..\\Texture\\daylight.jpg",
		1000.0f));
	m_BasicEffect.SetTextureCube(m_SkyBox->GetTextureCube());

	//-----------------------------------------------
	//初始化镜头
	//-----------------------------------------------
	auto camera = std::shared_ptr<CamControl>(new CamControl);
	m_Camera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->SetFrustum(XM_PI / 3, AspectRatio(), 1.0f, 1000.0f);
	camera->LookTo(XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	m_BasicEffect.SetViewMatrix(camera->GetViewXM());
	m_BasicEffect.SetProjMatrix(camera->GetProjXM());


	//------------------------------------------------
	//初始化光照
	//------------------------------------------------

	DirectionalLight dirLight[4];
	dirLight[0].ambient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);
	dirLight[0].diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight[0].specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	dirLight[0].direction = XMFLOAT3(-0.577f, -0.577f, 0.577f);
	dirLight[1] = dirLight[0];
	dirLight[1].direction = XMFLOAT3(0.577f, -0.577f, 0.577f);
	dirLight[2] = dirLight[0];
	dirLight[2].direction = XMFLOAT3(0.577f, -0.577f, -0.577f);
	dirLight[3] = dirLight[0];
	dirLight[3].direction = XMFLOAT3(-0.577f, -0.577f, -0.577f);
	for (int i = 0; i < 4; ++i)
		m_BasicEffect.SetDirLight(i, dirLight[i]);
	return true;
}

