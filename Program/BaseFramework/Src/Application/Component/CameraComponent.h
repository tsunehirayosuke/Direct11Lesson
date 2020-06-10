#pragma once

#include"../Game/GameObject.h"

class CameraComponent
{
public:
	CameraComponent(GameObject& owner);
	~CameraComponent();

	//オフセット行列取得
	inline KdMatrix& OffsetMatrix() { return m_mOffset; }

	//カメラ行列取得
	inline const KdMatrix& GetCameraMatrix() { return m_mCam; }

	//ビュー行列取得
	inline const KdMatrix& GetViewMatrix() { return m_mView; }

	//カメラ行列・ビュー行列設定(行列mと行列Offsetが合成され、最終的な行列になる)
	void SetCameraMatrix(const KdMatrix& m);

	//カメラ情報(ビュー・射影行列)をシェーダーにセット
	void SetToShader();
protected:

	KdMatrix	 m_mOffset;	//相対座標行列
	KdMatrix	 m_mCam;	//カメラ行列
	KdMatrix	 m_mView;	//ビュー行列
	KdMatrix	 m_mProj;	//射影行列
	GameObject&  m_owner;

};