#include"GameObject.h"

#include"../Component/CameraComponent.h"
#include"../Component/InputComponent.h"
#include"../Component/ModelComponent.h"

#include"Shooting/AirCraft.h"
GameObject::GameObject()
{

}

GameObject::~GameObject()
{
	Release();
}

void GameObject::Deserialize(const json11::Json& jsonObj)
{
	if (jsonObj.is_null()) { return; }

	if (jsonObj["Name"].is_null() == false)
	{
		m_name = jsonObj["Name"].string_value();
	}

	//タグ
	if (jsonObj["Tag"].is_null() == false)
	{
		m_tag = jsonObj["Tag"].int_value();
	}

	//モデル==================================

	if (m_spModelComponent)
	{
		m_spModelComponent->SetModel(KdResFac.GetModel(jsonObj["ModelFileName"].string_value()));
	}

	//行列======================================
	KdMatrix mTrans,mRotate,mScale;

	//座標
	const std::vector<json11::Json>& rPos = jsonObj["Pos"].array_items();
	if (rPos.size() == 3)
	{
		mTrans.CreateTranslation((float)rPos[0].number_value(),
			(float)rPos[1].number_value(), (float)rPos[2].number_value());
	}

	//回転
	const std::vector<json11::Json>& rRot = jsonObj["Rot"].array_items();
	if (rRot.size() == 3)
	{
		mRotate.CreateRotationX((float)rRot[0].number_value() * KdToRadians);
		mRotate.RotateY((float)rRot[1].number_value() * KdToRadians);
		mRotate.RotateZ((float)rRot[2].number_value() * KdToRadians);
	}

	//座標
	const std::vector<json11::Json>& rScale = jsonObj["Scale"].array_items();
	if (rScale.size() == 3)
	{
		mScale.CreateScaling((float)rScale[0].number_value(),
			(float)rScale[1].number_value(), (float)rScale[2].number_value());
	}

	m_mWorld = mScale * mRotate * mTrans;
}

void GameObject::Update(){}

void GameObject::Draw()
{
	if (m_spModelComponent == nullptr){return;}
	m_spModelComponent->Draw();
}

void GameObject::ImGuiUpdate()
{
	ImGui::InputText("Name", &m_name);

	if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::CheckboxFlags("Character", &m_tag, TAG_Character);
		ImGui::CheckboxFlags("Player", &m_tag, TAG_Player);
		ImGui::CheckboxFlags("StageObject", &m_tag, TAG_StageObject);
		ImGui::CheckboxFlags("AttackHit", &m_tag, TAG_AttackHit);

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		KdVec3 pos = m_mWorld.GetTranslation();
		KdVec3 rot = m_mWorld.GetAngles();

		bool isChange = false;

		isChange |= ImGui::DragFloat3("Position", &pos.x, 0.01f);
		isChange |= ImGui::DragFloat3("Rotation", &rot.x, 0.1f);

		if (isChange)
		{
			rot *= KdToRadians;

			KdMatrix mR;
			mR.RotateX(rot.x);
			mR.RotateY(rot.y);
			mR.RotateZ(rot.z);

			m_mWorld = mR;

			m_mWorld.SetTranslation(pos);
		}
		if (ImGui::Button(u8"JSONテキストコピー"))
		{
			std::string s = KdFormat("\"Pos\":[%.1f,%.1f,%.1f],\n", pos.x, pos.y, pos.z);
			s += KdFormat("\"Pos\":[%.1f,%.1f,%.1f],\n", rot.x, rot.y, rot.z);
			ImGui::SetClipboardText(s.c_str());
		}

		ImGui::TreePop();
	}
}

void GameObject::Release() 
{

}


//球による当たり判定(距離判定）
bool GameObject::HitCheckBySphere(const SphereInfo& rInfo)
{
	//当たった距離計算
	float hitRange = rInfo.m_radius + m_colRadius;

	//自分の座標ベクトル
	KdVec3 myPos = m_mWorld.GetTranslation();

	//二点間のベクトルを計算
	KdVec3 betweenVec = rInfo.m_pos - myPos;

	//二点間の距離を計算
	float distance = betweenVec.Length();

	bool isHit = false;
	if (distance <= hitRange)
	{
		isHit = true;
	}

	return isHit;
}

//レイによる当たり判定
bool GameObject::HitCheckByRay(const RayInfo& rInfo, KdRayResult& rResult)
{
	//判定をする対象のモデルがない場合は当っていない
	for (auto& node : m_spModelComponent->GetNodes())
	{
		KdRayResult tmpResult;

		KdRayToMesh(rInfo.m_pos, rInfo.m_dir, rInfo.m_maxRange, *(node.m_spMesh),
			node.m_localTransform * m_mWorld, tmpResult);
		if (tmpResult.m_distance < rResult.m_distance)
		{
			rResult = tmpResult;
		}
	}

	return rResult.m_hit;
}

#include "Shooting/ShootingGameProcess.h"
#include "Action/ActionGameProcess.h"

#include "Action/Human.h"

std::shared_ptr<GameObject> CreateGameObject(const std::string& name)
{
	if (name == "Human")
	{
		return std::make_shared<Human>();
	}

	if (name == "GameObject")
	{
		return std::make_shared<GameObject>();
	}
	else if (name == "Aircraft")
	{
		return std::make_shared<AirCraft>();
	}

	if (name == "ShootingGameProcess")
	{
		return std::make_shared<ShootingGameProcess>();
	}

	if (name == "ActionGameProcess")
	{
		return std::make_shared<ActionGameProcess>();
	}

	//文字列が既存のクラスに一致しない
	assert(0 && "存在しないGameObjectクラスです!");

	return nullptr;
}
