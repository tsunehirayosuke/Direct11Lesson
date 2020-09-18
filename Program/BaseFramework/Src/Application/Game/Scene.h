#pragma once


#include"EditorCamera.h"

//前方宣言
class GameObject;
class CameraComponent;

class Scene
{
public:
	static Scene& Getinstance()
	{
		static Scene instance;
		return instance;
	}
	
	~Scene();		//デストラクタ

	void RequestChangeScene(const std::string& filename);//シーン変更の受付

	void Init();	//初期化
	
	void Release();	//解放
	void Update();	//更新
	void Draw();	//描画


	//GameObjectのリストを返す
	const std::list<std::shared_ptr<GameObject>>GetObjects()const { return m_spObjects; }

	void AddObject(std::shared_ptr<GameObject> spObject);

	inline void SetTargetCamera(std::shared_ptr<CameraComponent> spCamera) { m_wpTargetCamera = spCamera; }

	void ImGuiUpdate();//ImGuiの更新

	void ImGuiPrefabFactoryUpdate();

	//デバッグスフィア描画
	void AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color = { 1,1,1,1 });
private:

	Scene();        //コンストラクタ

	void LoadScene(const std::string& sceneFilename);

	std::string m_nextSceneFileName = "";//次のシーンのJsonファイル名
	bool m_isRequestChangeScene = false; //シーン遷移のリクエストの有無

	void ExecChangeScene(); //シーンを実際に変更
	void Reset(); //シーンをまたぐときにリセットする

	std::shared_ptr<kdModel> m_spsky = nullptr;	//スカイスフィア
	EditorCamera*	m_pCamera;
	bool			m_editorCameraEnable = true;

	std::list<std::shared_ptr<GameObject>> m_spObjects;

	std::string str = "";

	//ImGuiで選択されたオブジェクト
	std::weak_ptr<GameObject>	m_wpImguiSelectObj;

	//ターゲットのカメラ
	std::weak_ptr<CameraComponent>m_wpTargetCamera;

	//デバッグライン描画用の頂点配列」
	std::vector<KdEffectShader::Vertex> m_debugLines;

};