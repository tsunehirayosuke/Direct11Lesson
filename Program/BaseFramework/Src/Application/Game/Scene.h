#pragma once


#include"EditorCamera.h"

//前方宣言
class GameObject;

class Scene
{
public:
	static Scene& Getinstance()
	{
		static Scene instance;
		return instance;
	}
	
	~Scene();		//デストラクタ

	void Init();	//初期化
	void Release();	//解放
	void Update();	//更新
	void Draw();	//描画

	void AddObject(GameObject* pObject);

	void ImGuiUpdate();//ImGuiの更新

	//デバッグライン描画
	void AddDebugLine(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Color& color = { 1,1,1,1 });

	//デバッグスフィア描画
	void AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color = { 1,1,1,1 });

	//デバッグ座標軸描画
	void AddDebugCoordinateAxisLine(const Math::Vector3& pos, float scale = 1.0f);
private:

	Scene();        //コンストラクタ

	kdModel			m_sky;					//スカイスフィア
	EditorCamera*	m_pCamera;
	bool			m_editorCameraEnable = true;

	std::list<GameObject*> m_objects;

	//デバッグライン描画用の頂点配列」
	std::vector<KdEffectShader::Vertex> m_debugLines;

};