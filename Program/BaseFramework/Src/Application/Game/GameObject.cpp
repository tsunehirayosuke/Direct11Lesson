﻿#include"GameObject.h"

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
	if (!m_spModelComponent) { return false; }

	if (m_spModelComponent->GetMesh() == nullptr) { return false; }

	return KdRayToMesh(rInfo.m_pos, rInfo.m_dir, rInfo.m_maxRange, 
		*(m_spModelComponent->GetMesh()), m_mWorld, rResult);
}

std::shared_ptr<GameObject> CreateGameObject(const std::string& name)
{
	if (name == "GameObject")
	{
		return std::make_shared<GameObject>();
	}
	else if (name == "Aircraft")
	{
		return std::make_shared<AirCraft>();
	}

	//文字列が既存のクラスに一致しない
	assert(0 && "存在しないGameObjectクラスです!");

	return nullptr;
}
