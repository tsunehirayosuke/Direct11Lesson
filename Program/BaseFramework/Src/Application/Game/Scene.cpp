#include "Scene.h"
#include "GameObject.h"
#include "../Component/CameraComponent.h"


#include"Shooting/AirCraft.h"
#include"Shooting/Missile.h"
#include"Shooting/AnimationEffect.h"
//コンストラクタ
Scene::Scene()
{

}

//デストラクタ
Scene::~Scene()
{

}

//初期化
void Scene::Init()
{
	std::ifstream ifs("Data/test.json");
	if (ifs.fail()) { assert(0 && "Jsonのファイルのパスが違います"); }


	//文字列として全読み込み
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	//文字列のJsonを解析（パース）する
	std::string err;
	json11::Json jsonObj = json11::Json::parse(strJson, err);
	if (err.size() > 0) { assert(0 && "読み込んだファイルのJson変換に失敗"); }

	//値アクセス
	{
		OutputDebugStringA(jsonObj["Name"].string_value().append("\n").c_str());

		OutputDebugStringA(std::to_string(jsonObj["Hp"].int_value()).append("\n").c_str());
	}

	//配列アクセス
	{
		//配列全アクセス
		auto& pos = jsonObj["Position"].array_items();
		for (auto&& p : pos)
		{
			OutputDebugStringA(std::to_string(p.number_value()).append("\n").c_str());
		}
		//配列添字アクセス
		OutputDebugStringA(std::to_string(pos[0].number_value()).append("\n").c_str());
		OutputDebugStringA(std::to_string(pos[1].number_value()).append("\n").c_str());
		OutputDebugStringA(std::to_string(pos[2].number_value()).append("\n").c_str());
	}

	//Object取得
	auto& object = jsonObj["monster"].object_items();
	OutputDebugStringA(object["name"].string_value().append("\n").c_str());
	OutputDebugStringA(std::to_string(object["hp"].int_value()).append("\n").c_str());
	OutputDebugStringA(std::to_string(object["pos"][0].number_value()).append("\n").c_str());
	OutputDebugStringA(std::to_string(object["pos"][1].number_value()).append("\n").c_str());
	OutputDebugStringA(std::to_string(object["pos"][2].number_value()).append("\n").c_str());

	//Object配列取得
	{
		auto& objects = jsonObj["techniques"].array_items();
		for (auto&& object : objects)
		{
			//共通要素はチェック無しでアクセス
			OutputDebugStringA(object["name"].string_value().append("\n").c_str());
			OutputDebugStringA(std::to_string(object["atk"].int_value()).append("\n").c_str());
			OutputDebugStringA(std::to_string(object["hitrate"].number_value()).append("\n").c_str());

			//固有のパラメーターはチェックしてから
			if (object["effect"].is_string())
			{
				OutputDebugStringA(object["effect"].string_value().append("\n").c_str());
			}
		}
	}

	m_pCamera = new EditorCamera();

	m_spsky = KdResourceFactory::GetInstance().GetModel("Data/Sky/Sky.gltf");
	
	LoadScene("Data/Scene/ShootingGame.json");

	std::shared_ptr<AnimationEffect> spExp = std::make_shared<AnimationEffect>();
	spExp->SetAnimationInfo(KdResFac.GetTexture("Data/Texture/Explosion00.png"), 10.0f, 5, 5, 0.0f);
	AddObject(spExp);
}

//解放
void::Scene::Release()
{
	m_spObjects.clear();
}

//更新
void Scene::Update()
{
	if (m_editorCameraEnable)
	{
		m_pCamera->Update();
	}
	auto selectObject = m_wpImguiSelectObj.lock();

	for (auto pObject : m_spObjects)
	{
		if (pObject == selectObject) { continue; }
		pObject->Update();
	}
	for (auto spObjectItr = m_spObjects.begin(); spObjectItr != m_spObjects.end();)
	{
		//寿命が尽きていたらリストから除外
		if ((*spObjectItr)->IsAlive() == false)
		{
			spObjectItr = m_spObjects.erase(spObjectItr);
		}
		else
		{
			++spObjectItr;
		}
	}

	if (m_isRequestChangeScene)
	{
		ExecChangeScene();
	}
}

//描画
void Scene::Draw()
{
	//エディターカメラをシェーダーにセット
	if (m_editorCameraEnable)
	{
		m_pCamera->SetToShader();
	}
	else
	{
		std::shared_ptr<CameraComponent> spCamera = m_wpTargetCamera.lock();
		if (spCamera)
		{
			spCamera->SetToShader();
		}
	}



	//カメラ情報（ビュー行列,射影行列)を各シェーダの定数バッファにセット
	SHADER.m_cb7_Camera.Write();

	//ライト情報をセット
	SHADER.m_cb8_Light.Write();

	
	if (m_spsky)
	{
		//エフェクトシェーダーを描画デバイスにセット
		SHADER.m_effectShader.SetToDevice();

		//
		Math::Matrix skyScale = DirectX::XMMatrixScaling(100.0f, 100.0f, 100.0f);

		SHADER.m_effectShader.SetWorldMatrix(skyScale);

		//モデルの描画(メッシュ情報とマテリアル情報を渡す)

		SHADER.m_effectShader.DrawMesh(m_spsky->GetMesh(0).get(), m_spsky->GetMaterials());
	}

	//不透明物描画
	SHADER.m_standardShader.SetToDevice();


	for (auto pObject : m_spObjects)
	{
		pObject->Draw();
	}

	//半透明物描画
	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());

	//Z情報は使うがZ書き込みOFF
	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteDisable, 0);

	//カリング無し
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullNone);


	for (auto spObj : m_spObjects)
	{
		spObj->DrawEffect();
	}

	D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);
	//カリング有り
	D3D.GetDevContext()->RSSetState(SHADER.m_rs_CullBack);

	//デバッグライン描画
	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());
	{
		AddDebugSphereLine(Math::Vector3(5.0f, 5.0f, 0.0f), 2.0f);

		//Zバッファ使用Off・書き込みOff
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);

		if (m_debugLines.size() >= 1)
		{

			SHADER.m_effectShader.SetWorldMatrix(Math::Matrix());

			SHADER.m_effectShader.DrawVertices(m_debugLines, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		}
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);

		m_debugLines.clear();
	}
}

void Scene::Reset()
{
	m_spObjects.clear();//メインのリストをクリア
	m_wpImguiSelectObj.reset();//ImGuiが選んでいるオブジェクトをクリア
	m_wpTargetCamera.reset();//カメラのターゲット担っているキャラクタのリセット
	m_spsky = nullptr;//空のクリア
}

void Scene::LoadScene(const std::string& sceneFilename)
{
	//GameObjectリストを空にする
	m_spObjects.clear();

	//JSON読み込み
	json11::Json json = KdResFac.GetJSON(sceneFilename);
	if (json.is_null())
	{
		assert(0 && "[LoadScene]jsonファイル読み込み失敗");
		return;
	}

	//オブジェクトリスト取得
	auto& objectDataList = json["GameObjects"].array_items();

	//オブジェクト生成ループ
	for (auto&& objJsonData : objectDataList)
	{
		//オブジェクト作成
		auto newGameObj = CreateGameObject(objJsonData["ClassName"].string_value());

		//プレハブ指定有の場合は、プレハブ側のものをベースにこのJSONをマージする
		KdMergePrefab(objJsonData);

		//オブジェクトのデシリアライズ
		newGameObj->Deserialize(objJsonData);

		//リストへ追加
		AddObject(newGameObj);
	}
}

void Scene::AddObject(std::shared_ptr<GameObject> spObject)
{
	if (spObject == nullptr) { return; }
	m_spObjects.push_back(spObject);
}

void Scene::ImGuiUpdate()
{
	auto selectObject = m_wpImguiSelectObj.lock();


	if (ImGui::Begin("Scene"))
	{
		ImGui::Checkbox("EditorCamera", &m_editorCameraEnable);

		if (ImGui::CollapsingHeader("Object List", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (auto&& rObj : m_spObjects)
			{
				bool selected = (rObj == selectObject);

				ImGui::PushID(rObj.get());

				if (ImGui::Selectable(rObj->GetName(), selected))
				{
					m_wpImguiSelectObj = rObj;
				}

				ImGui::PopID();
			}
		}
	}
	ImGui::End();

	//インスペクターウィンド
	if (ImGui::Begin("Inspector"))
	{
		
		if (selectObject)
		{
			selectObject->ImGuiUpdate();
		}
	}
	ImGui::End();
}

void Scene::ImGuiPrefabFactoryUpdate()
{
	if (ImGui::Begin("PrefabFactory"))
	{
		ImGui::InputText("JsonName", &str);

		if (ImGui::Button("Create"))
		{
			
		}

	}
	ImGui::End();
}

void Scene::AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color)
{
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0.0f,0.0f };

	static constexpr int kDetail = 16;
	
}

void Scene::RequestChangeScene(const std::string& filename)
{
	m_nextSceneFileName = filename;

	m_isRequestChangeScene = true;
}

void Scene::ExecChangeScene()
{
	LoadScene(m_nextSceneFileName);

	m_isRequestChangeScene = false;
}