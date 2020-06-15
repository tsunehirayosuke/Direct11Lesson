#include "Scene.h"
#include "GameObject.h"
#include "../Component/CameraComponent.h"

#include"Shooting/StageObject.h"
#include"Shooting/AirCraft.h"
#include"Shooting/Missile.h"
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
	m_pCamera = new EditorCamera();

	m_spsky = KdResourceFactory::GetInstance().GetModel("Data/Sky/Sky.gltf");
	Deserialize();
}

void Scene::Deserialize()
{
	std::shared_ptr<StageObject> spGround = std::make_shared<StageObject>();
	if (spGround)
	{
		spGround->Deserialize();
		m_objects.push_back(spGround);
	}

	std::shared_ptr<AirCraft> spAircraft = std::make_shared<AirCraft>();
	if (spAircraft)
	{
		spAircraft->SetTag(OBJECT_TAG::TAG_Player);
		spAircraft->Deserialize();
		m_objects.push_back(spAircraft);
	}
}

//解放
void::Scene::Release()
{
	m_objects.clear();
}

//更新
void Scene::Update()
{
	if (m_editorCameraEnable)
	{
		m_pCamera->Update();
	}
	for (auto pObject : m_objects)
	{
		pObject->Update();
	}
	for (auto spObjectItr = m_objects.begin(); spObjectItr != m_objects.end();)
	{
		//寿命が尽きていたらリストから除外
		if ((*spObjectItr)->IsAlive() == false)
		{
			spObjectItr = m_objects.erase(spObjectItr);
		}
		else
		{
			++spObjectItr;
		}
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

	//エフェクトシェーダーを描画デバイスにセット
	SHADER.m_effectShader.SetToDevice();

	//
	Math::Matrix skyScale = DirectX::XMMatrixScaling(100.0f, 100.0f, 100.0f);

	SHADER.m_effectShader.SetWorldMatrix(skyScale);

	//モデルの描画(メッシュ情報とマテリアル情報を渡す)
	if (m_spsky)
	{
		SHADER.m_effectShader.DrawMesh(m_spsky->GetMesh().get(), m_spsky->GetMaterials());
	}

	//不透明物描画
	SHADER.m_standardShader.SetToDevice();

	
	for (auto pObject : m_objects)
	{
		pObject->Draw();
	}

	//デバッグライン描画
	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());
	{
		AddDebugLine(Math::Vector3(), Math::Vector3(0.0f, 10.0f, 0.0f));

		AddDebugSphereLine(Math::Vector3(5.0f, 5.0f, 0.0f),2.0f);

		AddDebugCoordinateAxisLine(Math::Vector3(0.0f, 5.0f, 5.0f), 3.0f);

		//Zバッファ使用Off・書き込みOff
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable,0);

		if (m_debugLines.size() >= 1)
		{

			SHADER.m_effectShader.SetWorldMatrix(Math::Matrix());

			SHADER.m_effectShader.DrawVertices(m_debugLines, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		}
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);
	}
}

void Scene::AddObject(std::shared_ptr<GameObject> spObject)
{
	if (spObject == nullptr) { return; }
	m_objects.push_back(spObject);
}

void Scene::ImGuiUpdate()
{
	if (ImGui::Begin("Scene"))
	{
		ImGui::Checkbox("EditorCamera", &m_editorCameraEnable);

	}
		ImGui::End();
}

//デバッグライン描画
void Scene::AddDebugLine(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Color& color)
{
	//ラインの開始地点
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0.0f,0.0f };
	ver.Pos = p1;
	m_debugLines.push_back(ver);

	//ラインの終了地点
	ver.Pos = p2;
	m_debugLines.push_back(ver);
	
}

void Scene::AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color)
{
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0.0f,0.0f };

	static constexpr int kDetail = 16;
	for (UINT i = 0; i < kDetail + 1; i++)
	{
		//XZ平面
		ver.Pos = pos;
		ver.Pos.x += cos((float)i * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)i * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.Pos = pos;
		ver.Pos.x += cos((float)(i+1) * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)(i+1) * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		//XY平面
		ver.Pos = pos;
		ver.Pos.x += cos((float)i * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.y += sin((float)i * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.Pos = pos;
		ver.Pos.x += cos((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.y += sin((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		//YZ平面
		ver.Pos = pos;
		ver.Pos.y += cos((float)i * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)i * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.Pos = pos;
		ver.Pos.y += cos((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);
	}
}

void Scene::AddDebugCoordinateAxisLine(const Math::Vector3& pos, float scale)
{
	KdEffectShader::Vertex ver;
	ver.Color = { 1,1,1,1, };
	ver.UV = { 0.0f,0.0f };

	//X軸
	ver.Pos = pos;
	ver.Color = { 1,0,0,1, };
	m_debugLines.push_back(ver);

	ver.Pos.x += 1.0f * scale;
	m_debugLines.push_back(ver);

	//Y軸
	ver.Pos = pos;
	ver.Color = { 0,1,0,1, };
	m_debugLines.push_back(ver);

	ver.Pos.y += 1.0f * scale;
	m_debugLines.push_back(ver);

	//Z軸
	ver.Pos = pos;
	ver.Color = { 0,0,1,1, };
	m_debugLines.push_back(ver);

	ver.Pos.z += 1.0f * scale;
	m_debugLines.push_back(ver);
}