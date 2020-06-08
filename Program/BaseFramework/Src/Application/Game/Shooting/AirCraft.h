#pragma once

class Missile;

#include"../GameObject.h"

class AirCraft : public GameObject
{
public:

	void Deserialize(); //初期化:オブジェクト作成用外部データの解釈
	void Update();		//更新

	void ImGuiUpdate(); //AirCraftクラス専用のImGui更新

	void SetCameraToShader();//追従カメラのビュー行列、射影行列をシェーダーにセット

	void UpdateMove();	//移動更新処理
	void UpdateShoot(); //発射関数
	
private:

	float m_Speed = 0.2f;				//移動速度

	KdMatrix	 m_mOffset;	//追従カメラからの相対座標行列
	KdMatrix	 m_mProj;	//追従カメラの射影行列

	bool		mcanShoot  = true;		//発射可能かどうか
};