#include"AirCraft.h"
#include"Missile.h"
#include "../Scene.h"
#include "../../Component/CameraComponent.h"
#include "../../Component/InputComponent.h"

void AirCraft::Deserialize()
{
	m_spModel = KdResourceFactory::GetInstance().GetModel("Data/Aircraft/Aircraft_body.gltf");


	//初期配列座標を地面から少し浮いた位置にする
	m_mWorld.CreateTranslation(0.0f, 5.0f, 0.0f);

	if (m_spCameraComponent)
	{
		m_spCameraComponent->OffsetMatrix().CreateTranslation(0.0f, 1.5f, -10.0f);
	}

	if ((GetTag() & OBJECT_TAG::TAG_Player) != 0)
	{
		Scene::Getinstance().SetTargetCamera(m_spCameraComponent);
	}

	//プレイヤー入力
	m_spInputComponent = std::make_shared<PlayerInputComponent>(*this);
}

void AirCraft::Update()
{
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}
	UpdateMove();

	UpdateShoot();

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
	KdVec3 rotate;

	if (GetAsyncKeyState('W') & 0x8000) { rotate.x = 1.0f; }
	if (GetAsyncKeyState('A') & 0x8000) { rotate.z = -1.0f; }
	if (GetAsyncKeyState('S') & 0x8000) { rotate.x = -1.0f; }
	if (GetAsyncKeyState('D') & 0x8000) { rotate.z = 1.0f; }

	//回転行列作成
	KdMatrix rotateMat;
	rotateMat.CreateRotationX(rotate.x * KdToRadians);
	rotateMat.RotateZ(rotate.z * KdToRadians);

	//ワールド行列に合成
	m_mWorld = rotateMat * m_mWorld;
}

void AirCraft::UpdateShoot()
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (mcanShoot)
		{
			std::shared_ptr<Missile> spMissile = std::make_shared<Missile>();
			if (spMissile)
			{

				spMissile->Deserialize();
				spMissile->SetMatrix(m_mWorld);

				Scene::Getinstance().AddObject(spMissile);
			}
			mcanShoot = false;
		}
	}
	else
	{
		mcanShoot = true;
	}
}