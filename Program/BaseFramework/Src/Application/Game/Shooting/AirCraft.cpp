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

		kdModel::Node* propNode = m_spModelComponent->FindNode("propeller");//文字列をもとにプロペラノードの検索
		if (propNode)
		{
			propNode->m_localTransform.CreateTranslation(0.0f, 0.0f, 2.8f);//プレイヤーのプロペラだけ前に進める
		}
		m_propRotSpeed = 0.3;//プロペラの回転速度

	}
	else
	{
		//敵飛行機入力
		m_spInputComponent = std::make_shared<EnemyInputComponent>(*this);

		kdModel::Node* propNode = m_spModelComponent->FindNode("propeller");//文字列をもとにプロペラノードの検索
		if (propNode)
		{
			propNode->m_localTransform.CreateTranslation(0.0f, 0.0f, 2.8f);//プレイヤーのプロペラだけ前に進める
		}
		m_propRotSpeed = 0.3;//プロペラの回転速度
	}

	m_spActionState = std::make_shared<ActionFly>();

	m_propTrail.SetTexture(KdResFac.GetTexture("Data/Texture/sabelline.png"));
}

void AirCraft::Update()
{
	if (m_spInputComponent)
	{
		m_spInputComponent->Update();
	}

	m_prevPos = m_mWorld.GetTranslation();

	if (m_spActionState)
	{
		m_spActionState->Update(*this);
	}

	if (m_spCameraComponent)
	{
		m_spCameraComponent->SetCameraMatrix(m_mWorld);
	}

	UpdatePropeller();
}


void::AirCraft::UpdateMove()
{
	if (m_spInputComponent == nullptr)
	{
		return;
	}

	const Math::Vector2& inputMove = m_spInputComponent->GetAxis(Input::Axes::L);
	const Math::Vector2& inputRotate = m_spInputComponent->GetAxis(Input::Axes::R);


	//移動ベクトル作成

	KdVec3 move = { inputMove.x,0.0f,inputMove.y };

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

				KdMatrix mLaunch;
				mLaunch.CreateRotationX((rand() % 120 - 60.0f) * KdToRadians);
				mLaunch.RotateY((rand() % 120 - 60.0f) * KdToRadians);
				mLaunch *= m_mWorld;

				spMissile->SetMatrix(m_mWorld);

				spMissile->SetOwner(shared_from_this());

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

	m_laser = (m_spInputComponent->GetButton(Input::Buttons::A) != InputComponent::Free);
}


#include"EffectObject.h"
void AirCraft::UpdateCollision()
{
	if (m_laser)
	{
		RayInfo rayInfo;
		rayInfo.m_pos = m_prevPos;
		rayInfo.m_dir = m_mWorld.GetAxisZ();
		rayInfo.m_dir.Normalize();
		rayInfo.m_maxRange = m_laserRange;

		KdRayResult rayResult;

		for (auto& obj : Scene::Getinstance().GetObjects())
		{
			//自分自身は無視
			if (obj.get() == this) { continue; }

			//背景オブジェクトとキャラクターが対象
			if (!(obj->GetTag() & (TAG_StageObject | TAG_Character))) { continue; }

			//判定実行
			if (obj->HitCheckByRay(rayInfo, rayResult))
			{
				//当たったなら爆発をインスタンス化
				std::shared_ptr<EffectObject> effectObj = std::make_shared<EffectObject>();

				//相手の飛行機へのダメージ通知
				OnNotify_Damage(m_attackPow);

				//ミサイルやレーザーの攻撃力はJsonに入れる
				if (effectObj)
				{
					//キャラクターのリストに爆発の追加
					Scene::Getinstance().AddObject(effectObj);

					//レーザーのヒット位置　＝　レイの発射位置　＋　
					//(レイの発射方向ベクトル　＊　レイが当たった地点までの距離)
					KdVec3 hitPos(rayInfo.m_pos);

					hitPos = hitPos + (rayInfo.m_dir * rayResult.m_distance);

					//爆発エフェクトの行列を計算
					KdMatrix mMat;

					mMat.CreateTranslation(hitPos.x, hitPos.y, hitPos.z);

					effectObj->SetMatrix(mMat);
				}
			}
		}
	}

	//一回の移動量と移動方向を計算
	KdVec3 moveVec = m_mWorld.GetTranslation() - m_prevPos;//動く前→今の場所のベクトル
	float moveDistance = moveVec.Length();//一回の移動量

	//動いていないなら判定しない
	if (moveDistance == 0.0f) { return; }

	//球判定情報を作成
	SphereInfo info;
	info.m_pos = m_mWorld.GetTranslation();
	info.m_radius = m_colRadius;

	for (auto& obj : Scene::Getinstance().GetObjects())
	{
		//自分自身
		if (obj.get() == this) { continue; }

		//キャラクターとのみ当たり判定
		if (!(obj->GetTag() & TAG_Character)) { continue; }

		//当たり判定
		if (obj->HitCheckBySphere(info))
		{
			//移動する前の位置に戻る
			m_mWorld.SetTranslation(m_prevPos);

			Scene::Getinstance().AddDebugSphereLine(
				m_mWorld.GetTranslation(), 2.0f, { 1.0f,0.0f,0.0f,1.0f }
			);
		}
	}

	//レイによる当たり判定
	//レイ情報の作成
	RayInfo rayInfo;
	rayInfo.m_pos = m_prevPos;			//ひとつ前の場所から
	rayInfo.m_dir = moveVec;			//動いた方向に向かって
	rayInfo.m_maxRange = moveDistance;	//動いた分だけ判定

	rayInfo.m_dir.Normalize();

	KdRayResult rayResult;

	for (auto& obj : Scene::Getinstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }

		//背景タグ以外は無視
		if (!(obj->GetTag() & TAG_StageObject)) { continue; }

		//判定実行
		if (obj->HitCheckByRay(rayInfo, rayResult))
		{
			m_mWorld.SetTranslation(m_prevPos);
		}
	}
}
void AirCraft::UpdatePropeller()
{
	kdModel::Node* propNode = m_spModelComponent->FindNode("propeller");
	if (propNode)
	{
		//回転量分プロペラのローカル行列を回転
		propNode->m_localTransform.RotateZ(m_propRotSpeed);

		//プロペラの中心座標(World)
		KdMatrix propCenterMat;
		propCenterMat *= propNode->m_localTransform * m_mWorld;

		//プロペラの外周座標(World)
		KdMatrix propOuterMat;

		//そこからY軸へ少しずらした位置
		propOuterMat.CreateTranslation(0.0f, 1.8f, 0.0f);
		propOuterMat *= propCenterMat;

		//Strip描画をするため2つで1ペア追加
		m_propTrail.AddPoint(propCenterMat);
		m_propTrail.AddPoint(propOuterMat);

		//30個より多く登録されてたら
		if (m_propTrail.GetNumPoints() > 30)
		{
			//Strip描画するために２つで1ペア消す
			m_propTrail.DelPoint_Back();
			m_propTrail.DelPoint_Back();
		}
	}
}


void AirCraft::Draw()
{
	GameObject::Draw();//基底クラスのDrawを呼び出す


	//レーザー描画
	if (m_laser)
	{
		//レーザーの終点を求める
		KdVec3 laserStart(m_prevPos);
		KdVec3 laserEnd;
		KdVec3 laserDir(m_mWorld.GetAxisZ());

		laserDir.Normalize();//拡大が入っているとき1以上なので正規化

		laserDir *= m_laserRange;//レーザーの射程分方向ベクトルを伸ばす

		laserEnd = laserStart + laserDir;//レーザーの終点は発射位置ベクトル+レーザーの長さ分
	}
}
void AirCraft::OnNotify_Damage(int damage)
{
	m_hp -= damage;

	if (m_hp <= 0)
	{
		m_spActionState = std::make_shared<ActionCrash>();
	}
}

void AirCraft::DrawEffect()
{
	D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Add, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);

	SHADER.m_effectShader.SetWorldMatrix(KdMatrix());
	SHADER.m_effectShader.WriteToCB();

	m_propTrail.DrawStrip();

	D3D.GetDevContext()->OMSetBlendState(SHADER.m_bs_Alpha, Math::Color(0, 0, 0, 0), 0xFFFFFFFF);
}

void AirCraft::ActionFly::Update(AirCraft& owner)
{
	owner.UpdateMove();

	owner.UpdateCollision();

	owner.UpdateShoot();
}

void AirCraft::ActionCrash::Update(AirCraft& owner)
{
	if (!(--m_timer))
	{
		owner.Destroy();
	}

	KdMatrix rotation;
	rotation.CreateRotationX(0.08f);
	rotation.RotateY(0.055f);
	rotation.RotateZ(0.03f);

	owner.m_mWorld = rotation * owner.m_mWorld;

	owner.m_mWorld.Move(KdVec3(0.0f, -0.2f, 0.0f));
}
/*{
	//Scene::Getinstance().AddObject(spObject);

	float minDistance = FLT_MAX;
	std::shared_ptr<GameObject> spTarget = nullptr;
	for (auto& obj : Scene::Getinstance().GetObjects())
	{
		//自分自身は無視
		if (obj.get() == this) { continue; }

		if (obj->GetTag() & TAG_AttackHit)
		{
			//(ターゲットの座標-自身の座標)の長さの2乗　
			float distance = KdVec3(obj->GetMatrix().GetTranslation() - m_mWorld.GetTranslation()).LengthSquared();

			//一番近いオブジェクトとの距離よりも近ければ
			if (distance < minDistance)
			{
				//誘導する予定のターゲットを今チェックしたGameObjectに置き換え
				spTarget = obj;

				//一番近いオブジェクトとの距離を今のものに更新
				minDistance = distance;
			}
		}

	}
	//spMissile->SetTarget(spTarget);
}*/