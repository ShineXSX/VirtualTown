#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "MiniTownSettings.h"
#include "Camera.h"
#include "Realize.h"
#include "SkyBox.h"
#include "LoadModel.h"
#include "Collision.h"
class GameApp : public D3DApp
{

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

private:
	bool InitResource();
	
private:
	
	ComPtr<ID2D1SolidColorBrush> m_pColorBrush;				    // 单色笔刷
	ComPtr<IDWriteFont> m_pFont;								// 字体
	ComPtr<IDWriteTextFormat> m_pTextFormat;					// 文本格式

	Town_Ins m_Sphere;										// 球
	Town_Ins m_Ground;										// 地面
	Town_Ins m_Cylinder;									    // 圆柱
	Town_Ins m_Tree1, m_Tree2, m_Tree3, m_Tree4, m_Tree5, m_Tree6, m_Tree7, m_Tree8, m_Tree9, m_Tree10;
																// 树
	Town_Ins m_Warehouse1, m_Warehouse2, m_House1, m_House2, m_House3, m_House4, m_Windmill;
																// 楼房
	Town_Ins m_Roadc1, m_Roadc2, m_Roadc3, m_Roadc4;
																//街道corner
	Town_Ins m_Roadl1, m_Roadl2, m_Roadl3, m_Roadl4, m_Roadl5, m_Roadl6, m_Roadl7, m_Roadl8, m_Roadl9, m_Roadl10, m_Roadl11, m_Roadl12;
																//街道line
	Town_Ins m_Roadr1, m_Roadr2, m_Roadr3, m_Roadr4, m_Roadr5, m_Roadr6, m_Roadr7, m_Roadr8, m_Roadr9, m_Roadr10, m_Roadr11, m_Roadr12;
																//街道row
	Town_Ins m_Roadt1, m_Roadt2, m_Roadt3, m_Roadt4;			//街道T

	Town_Ins m_Roadx;											//街道X

	Town_Ins m_Car;												//汽车
	BasicEffect m_BasicEffect;								    // 对象渲染特效管理
	
	SkyEffect m_SkyEffect;									    // 天空盒特效管理
	std::unique_ptr<SkyBox> m_SkyBox;					    // 天空盒(白天)

	std::shared_ptr<Camera> m_Camera;						    // 摄像机

	ObjLoader m_ObjLoader;									    // 模型读取对象
};


#endif