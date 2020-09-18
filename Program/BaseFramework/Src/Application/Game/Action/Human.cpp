#include "Human.h"
#include "../Scene.h"

#include "../../Component/CameraComponent.h"
#include "../../Component/InputComponent.h"

const float s_allowToStepHeight = 0.8f;
const float s_landingHeight = 0.1f;

void Human::Deserialize(const json11::Json& jsonObj)
{
	GameObject::Deserialize(jsonObj);//共通部分のDeserialize

	//カメラコンポーネントの設定
	if (m_spCameraComponent)
	{
		m_spCameraComponent->OffsetMatrix().CreateTranslation(0.0f, 1.5f, - 5.0f);
		m_spCameraComponent->OffsetMatrix().RotateX(25.0f * KdToRadians);
	}

	//プレイヤーであればInputComponentを作成
	if ((GetTag() & TAG_Player) != 0)
	{
		Scene::Getinstance().SetTargetCamera(m_spCameraComponent);
		m_spInputComponent = std::make_shared<ActionPlayerInputComponent>(*this);
	}
}

void Human::Update()
{
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	m_prevPos = m_pos;

	UpdateMove();
	UpdateCamera();

	//重力をキャラクターのＹの移動力に加える
	m_force.y -= m_gravity;

	//移動力をキャラクターの座標に足しこむ
	m_pos.x += m_force.x;
	m_pos.y += m_force.y;
	m_pos.z += m_force.z;

	UpdateCollision();

	//ワールド行列を合成
	m_mWorld.CreateRotationX(m_rot.x);
	m_mWorld.RotateY(m_rot.y);
	m_mWorld.RotateZ(m_rot.z);
	m_mWorld.Move(m_pos);

	if (m_spCameraComponent)
	{
		KdMatrix trans;
		trans.CreateTranslation(m_pos.x, m_pos.y, m_pos.z);

		m_spCameraComponent->SetCameraMatrix(trans);
	}
}

void Human::UpdateMove()
{
	if (!m_spCameraComponent) { return; }

	//入力情報の取得
	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);

	KdVec3 moveSide = m_spCameraComponent->GetCameraMatrix().GetAxisX() * inputMove.x;
	KdVec3 moveForward = m_spCameraComponent->GetCameraMatrix().GetAxisZ() * inputMove.y;

	moveForward.y = 0.0f;
	//移動ベクトルの計算
	KdVec3 moveVec = moveSide + moveForward;

	//正規化
	moveVec.Normalize();

	UpdateRotate(moveVec);

	//移動速度に合わせる
	moveVec *= m_moveSpeed;

	//場所に加算
	m_force.x += moveVec.x;
	m_force.z += moveVec.z;
}

void Human::UpdateCamera()
{
	if (!m_spCameraComponent) { return; }

	const Math::Vector2& inputCamera = m_spInputComponent->GetAxis(Input::Axes::R);

	m_spCameraComponent->OffsetMatrix().RotateY(inputCamera.x * m_camRotSpeeed * KdToRadians);
}

void Human::UpdateRotate(const KdVec3& rMoveDir)
{
	//移動してなければ終了
	if (rMoveDir.LengthSquared() == 0.0f) { return; }

	//今のキャラクターの方向ベクトル
	KdVec3 nowDir = m_mWorld.GetAxisZ();
	nowDir.Normalize();

	//キャラクターの今向いている方向の角度を求める
	float nowRadian = atan2(nowDir.x, nowDir.z);

	//移動方向へのベクトル角度を求める
	float targetRadian = atan2(rMoveDir.x, rMoveDir.z);

	float rotateRadian = targetRadian - nowRadian;

	//一回の回転角度をm_rotateAngle度以内に収める
	rotateRadian = std::clamp(rotateRadian, -m_rotateAngle * KdToRadians, m_rotateAngle * KdToRadians);

	m_rot.y += rotateRadian;
}

void Human::UpdateCollision()
{
	float distanceFromGround = FLT_MAX;

	//下方向への判定を行い、着地
	if (CheckGround(distanceFromGround))
	{
		//地面の上にy座標を移動
		m_pos.y += s_allowToStepHeight - distanceFromGround;

		//地面があるので、y方向への移動力は０に
		m_force.y = 0.0f;
	}
}

bool Human::CheckGround(float& rDstDistance)
{
	//レイ判定情報
	RayInfo rayInfo;
	rayInfo.m_pos = m_pos;//キャラクターの位置を発射地点に

	//キャラの足元からレイを発射すると地面と当たらないので少し上げる
	rayInfo.m_pos.y += s_allowToStepHeight;

	//落下中かもしれないので、1フレーム前の座標分も持ち上げる
	rayInfo.m_pos.y += m_prevPos.y - m_pos.y;

	//地面方向へのレイ
	rayInfo.m_dir = { 0.0f,-1.0f,0.0f };

	//レイの結果格納用
	rayInfo.m_maxRange = FLT_MAX;
	KdRayResult finalRayResult;

	//全員とレイ判定
	for (auto& obj : Scene::Getinstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }

		//ステージと当たり判定
		if (!(obj->GetTag() & (TAG_StageObject))) { continue; }
		KdRayResult rayResult;

		if (obj->HitCheckByRay(rayInfo, rayResult))
		{
			//最も当たったところまでの距離が短いものを保持
			if (rayResult.m_distance < finalRayResult.m_distance)
			{
				finalRayResult = rayResult;
			}
		}
	}

	//補正分の長さを結果に反映＆
	float distanceFromGround = FLT_MAX;

	//足元にステージオブジェクトがあった
	if (finalRayResult.m_hit)
	{
		//地面との接触を算出
		distanceFromGround = finalRayResult.m_distance - (m_prevPos.y - m_pos.y);
	}

	//上方向に力がかかっていた場合
	if (m_force.y > 0.0f)
	{
		//着地禁止
		m_isGround = false;
	}
	else
	{
		//地面からの距離が一定未満なら着地とみなす
		m_isGround = (distanceFromGround < (s_allowToStepHeight + s_landingHeight));
	}

	//地面との距離を格納
	rDstDistance = distanceFromGround;

	//着地したかどうか
	return m_isGround;
}
