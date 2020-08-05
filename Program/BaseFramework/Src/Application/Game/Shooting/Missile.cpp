#include"Missile.h"

#include"Application//main.h"
#include "../../Component/ModelComponent.h"
#include "../Scene.h"



void Missile::Deserialize(const json11::Json& jsonObj)
{
	m_lifeSpan = APP.m_maxFps * 10;

	if (jsonObj.is_null()) { return; }

	GameObject::Deserialize(jsonObj);

	if (jsonObj["Speed"].is_null() == false)
	{
		m_speed = jsonObj["Speed"].number_value();
	}

	//煙テクスチャ
	m_trailSmoke.SetTexture(KdResFac.GetTexture("Data/Texture/smokeline2.png"));
}

void Missile::Update()
{
	if (m_alive == false) { return; }

	if (--m_lifeSpan <= 0)
	{
		Destroy();
	}

	//ターゲットをshared_ptr化
	auto target = m_wpTarget.lock();
	if (target)
	{
		//自身からターゲットへのベクトル
		KdVec3 vTarget = target->GetMatrix().GetTranslation() - m_mWorld.GetTranslation();

		//単位ベクトル化：自身からターゲットへの向かう長さ1のベクトル
		vTarget.Normalize();

		//自分のZ方向(前方向)
		KdVec3 vZ = m_mWorld.GetAxisZ();

		//拡大率が入っていると計算がおかしくなるため単位ベクトル化
		vZ.Normalize();

		//※※※回転軸作成(この軸で回転する)※※※
		KdVec3 vRotAxis = KdVec3::Cross(vZ, vTarget);

		//0ベクトルなら回転しない
		if (vRotAxis.LengthSquared() != 0)
		{
			//自分のZ方向ベクトルと自身からターゲットへ向かうベクトルの内積
			float d = KdVec3::Dot(vZ, vTarget);

			//誤差で-1～1以外になる可能性大なので、クランプする
			if (d > 1.0f)d = 1.0f;
			else if (d > -1.0f)d = -1.0f;

			//自分の前方向ベクトルと自身からターゲットへ向かうベクトルの角度
			float radian = acos(d);

			//角度制限　1フレームにつき最大で1度以上回転しない
			if (radian > 1.0f * KdToRadians)
			{
				radian = 1.0f * KdToRadians;
			}

			//※※※radian　(ここまでで回転角度が決まった)※※※

			KdMatrix mRot;
			mRot.CreateRotationAxis(vRotAxis, radian);
			auto pos = m_mWorld.GetTranslation();
			m_mWorld.SetTranslation({ 0,0,0 });
			m_mWorld *= mRot;
			m_mWorld.SetTranslation(pos);
		}
	}
	KdVec3 move = m_mWorld.GetAxisZ();
	move.Normalize();

	move *= m_speed;

	m_mWorld._41 += move.x;
	m_mWorld._42 += move.y;
	m_mWorld._43 += move.z;

	m_prevPos = m_mWorld.GetTranslation();

	UpdateCollision();

	UpdateTrail();
}

#include"AirCraft.h"
#include"EffectObject.h"
void Missile::UpdateCollision()
{
	KdVec3 moveVec = m_mWorld.GetTranslation() - m_prevPos;

	float moveDistance = moveVec.Length();

	if (moveDistance == 0.0f) { return; }


	//球判定情報を作成
	SphereInfo sInfo;
	sInfo.m_pos = m_mWorld.GetTranslation();
	sInfo.m_radius = m_colRadius;

	//発射した主人のshared_ptr取得
	auto spOwner = m_wpOwner.lock();



	for (auto& obj : Scene::Getinstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }

		//発射した主人も無視
		if (obj.get() == spOwner.get()) { continue; }

		if (!(obj->GetTag() & TAG_Character)) { continue; }

		//TAG_Characterとは球判定を行う
		if (obj->HitCheckBySphere(sInfo))
		{
			//std::dynamic_pointer_cast = 基底クラスをダウンキャストするときに使う。失敗するとnullptrが帰る
			//重い、多発する場合は設計ミス

			std::shared_ptr<AirCraft> aircraft = std::dynamic_pointer_cast<AirCraft>(obj);
			if (aircraft)
			{
				aircraft->OnNotify_Damage(m_attackPow);

			}
			Explosion();

			Destroy();

			return;
		}

	}

	//レイ情報の作成
	RayInfo rInfo;
	rInfo.m_pos = m_prevPos;
	rInfo.m_dir = moveVec;
	rInfo.m_dir.Normalize();
	rInfo.m_maxRange = moveDistance;

	KdRayResult rayResult;

	//TAG_StageObjectとはレイ判定を行う

	for (auto& obj : Scene::Getinstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }

		//発射した主人も無視
		if (obj.get() == spOwner.get()) { continue; }

		if (!(obj->GetTag() & TAG_StageObject)) { continue; }

		//TAG_Characterとは球判定を行う
		if (obj->HitCheckByRay(rInfo,rayResult))
		{
			Explosion();

			Destroy();

			return;
		}

	}
}

#include "AnimationEffect.h"
void Missile::Explosion()
{
	//アニメーションエフェクトをインスタンス化
	std::shared_ptr<AnimationEffect> effect = std::make_shared<AnimationEffect>();

	//爆発のテクスチャとアニメーション情報を渡す
	effect->SetAnimationInfo(
		KdResFac.GetTexture("Data/Texture/Explosion00.png"), 10.0f, 5, 5, rand() % 360);

	//場所をミサイル（自分）の位置に合わせる
	effect->SetMatrix(m_mWorld);

	//リストに追加
	Scene::Getinstance().AddObject(effect);
}

void Missile::UpdateTrail()
{
	m_trailRotate += 1.0f;

	KdMatrix mTrail;
	mTrail.RotateZ(m_trailRotate);
	mTrail *= m_mWorld;

	//軌跡の座標を先頭に追加
	m_trailSmoke.AddPoint(mTrail);

	//軌跡の数制限(100以前の軌跡を消去する)
	if (m_trailSmoke.GetNumPoints() > 100)
	{
		m_trailSmoke.DelPoint_Back();
	}
}

void Missile::DrawEffect()
{
	if (!m_alive) { return; }

	SHADER.m_effectShader.SetWorldMatrix(KdMatrix());

	SHADER.m_effectShader.WriteToCB();

	m_trailSmoke.DrawBillboard(0.5f);
}
