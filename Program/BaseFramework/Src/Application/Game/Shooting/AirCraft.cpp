#include"AirCraft.h"
#include"Missile.h"
#include "../Scene.h"
#include "../../Component/CameraComponent.h"
#include "../../Component/InputComponent.h"
#include "../../Component/ModelComponent.h"

void AirCraft::Deserialize(const json11::Json& jsonObj)
{

	GameObject::Deserialize(jsonObj);


	if (m_spCameraComponent)
	{
		m_spCameraComponent->OffsetMatrix().CreateTranslation(0.0f, 1.5f, -10.0f);
	}

	if ((GetTag() & OBJECT_TAG::TAG_Player) != 0)
	{
		Scene::Getinstance().SetTargetCamera(m_spCameraComponent);

		//プレイヤー入力
		m_spInputComponent = std::make_shared<PlayerInputComponent>(*this);
	}
	else
	{
		//敵飛行機入力
		m_spInputComponent = std::make_shared<EnemyInputComponent>(*this);
	}

}

void AirCraft::Update()
{
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	m_prevPos = m_mWorld.GetTranslation();

	UpdateMove();

	UpdateShoot();

	UpdateCollision();

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}
}


void AirCraft::ImGuiUpdate()
{
	if (ImGui::TreeNodeEx("AirCraft", ImGuiTreeNodeFlags_DefaultOpen))
	{
		KdVec3 pos;
		pos = m_mWorld.GetTranslation();

		//ImGui::Text("Position[x:%.2f] [y:%.2f] [z:%.2f]", pos.x, pos.y, pos.z);

		if (ImGui::DragFloat3("Position", &pos.x, 0.01f))
		{
			KdMatrix mTrans;
			mTrans.CreateTranslation(pos.x, pos.y, pos.z);

			m_mWorld = mTrans;
		}

		ImGui::TreePop();
	}
}


void::AirCraft::UpdateMove()
{
	if (m_spInputComponent==nullptr)
	{
		return;
	}

	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);
	const Math::Vector2& inputRotate = m_spInputComponent->GetAxis(Input::Axes::R);


	//移動ベクトル作成
	
	KdVec3 move = {inputMove.x,0.0f,inputMove.y};

	move.Normalize();

	//移動底度補正
	move *= m_Speed;

	/*
	m_mWorld._41 += move.x;
	m_mWorld._42 += move.y;
	m_mWorld._43 += move.z;
	*/

	//移動行列作成
	KdMatrix moveMat;
	moveMat.CreateTranslation(move.x, move.y, move.z);

	//ワールド行列に合成
	m_mWorld = DirectX::XMMatrixMultiply(moveMat, m_mWorld);
	m_mWorld = moveMat * m_mWorld;

	//回転ベクトルの作成
	KdVec3 rotate = { inputRotate.y,0.0f,inputRotate.x };

	//回転行列作成
	KdMatrix rotateMat;
	rotateMat.CreateRotationX(rotate.x * KdToRadians);
	rotateMat.RotateZ(rotate.z * KdToRadians);

	//ワールド行列に合成
	m_mWorld = rotateMat * m_mWorld;
}

void AirCraft::UpdateShoot()
{
	if (m_spInputComponent == nullptr)
	{
		return;
	}

	if (m_spInputComponent->GetButton(Input::Buttons::A))
	{
		if (mcanShoot)
		{
			std::shared_ptr<Missile> spMissile = std::make_shared<Missile>();
			if (spMissile)
			{

				spMissile->Deserialize(KdResFac.GetJSON("Data/Scene/Missile.json"));
				spMissile->SetMatrix(m_mWorld);

				Scene::Getinstance().AddObject(spMissile);

				//全ゲームオブジェクトのリストからミサイルが当たる対象を探す
				for (auto object : Scene::Getinstance().GetObjects())
				{
					//発射した飛行機自身は無視
					if (object.get() == this) { continue; }

					if ((object->GetTag() & TAG_AttackHit))
					{
						spMissile->SetTarget(object);

						break;
					}
				}
			}
			mcanShoot = false;
		}
	}
	else
	{
		mcanShoot = true;
	}
}

void AirCraft::UpdateCollision()
{
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_colRadius;

	for(auto& obj : Scene::Getinstance().GetObjects())
	{
		//自分自身
		if (obj.get() == this) { continue; }

		//キャラクターとのみ当たり判定
		if (!(obj->GetTag() & TAG_Character)) { continue; }

		//当たり判定
		if (obj->HitCheckBySphere(info))
		{
			Scene::Getinstance().AddDebugSphereLine(
				m_mWorld.GetTranslation(), 2.0f, { 1.0f,0.0f,0.0f,1.0f }
			);

			//移動する前の位置に戻る
			m_mWorld.SetTranslstion(m_prevPos);
		}
	}
}
