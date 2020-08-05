#include"AnimationEffect.h"

AnimationEffect::AnimationEffect(){}

AnimationEffect::~AnimationEffect(){}

void AnimationEffect::SetAnimationInfo(const std::shared_ptr<KdTexture>& rTex, float size, int splitX, int splitY, float angle)
{
	//アニメーションの分割数を設定
	m_poly.Init(size, size, { 1,1,1,1 });
	
	//アニメーションの位置を0コマ目に移動しておく
	m_poly.SetAnimationInfo(splitX, splitY);
	
	//渡されたテクスチャ設定
	m_poly.SetAnimationPos(0);

	m_poly.SetTexture(rTex);

	m_angleZ = angle;
}

void AnimationEffect::Update()
{
	m_poly.Animation(0.5f, false);

	if (m_poly.IsAnimationEnd())
	{
		Destroy();
	}
}

void AnimationEffect::DrawEffect()
{
	//各軸方向の拡大率を取得
	float scaleX = m_mWorld.GetAxisX().Length();
	float scaleY = m_mWorld.GetAxisY().Length();
	float scaleZ = m_mWorld.GetAxisZ().Length();

	//ビルボード処理
	KdMatrix drawMat;
	drawMat.CreateScaling(scaleX, scaleY, scaleZ);//拡大率を設定
	drawMat.RotateZ(m_angleZ * KdToRadians);//Z軸の回転角度を加える

	//カメラの行列の取得
	KdMatrix camMat = SHADER.m_cb7_Camera.GetWork().mV;
	camMat.Inverse();//カメラの逆行列を計算

	//カメラの逆行列を描画する行列との合成
	drawMat *= camMat;

	//座標は自分のものを使う
	drawMat.SetTranslation(m_mWorld.GetTranslation());

	SHADER.m_effectShader.SetWorldMatrix(drawMat);
	SHADER.m_effectShader.WriteToCB();
	m_poly.Draw(0);
}
