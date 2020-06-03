#pragma once

class StageObject
{
public:
	StageObject();	//コンストラクタ
	~StageObject();	//デストラクタ

	void Deserialize();	//初期化：オブジェクト生成用外部データの解釈
	void Update();		//更新
	void Draw();		//描画

private:

	void Release();		//解放

	kdModel* m_pModel = nullptr;	//3Dモデルデータ
	Math::Matrix m_mWorld;			//ゲーム内の絶対座標
};