#pragma once

class Missile
{
public:
	Missile();//コンストラクタ
	~Missile();//デストラクタ


	void Deserialize(); //初期化:オブジェクト作成用外部データの解釈
	void Update();		//更新
	void Draw();		//描画

	void Move(const KdVec3& v);

	inline void SetMatrix(const KdMatrix& rMat) { m_mWorld = rMat; }
	inline bool IsAlive() const { return m_alive; }
	inline void Destroy() { m_alive = false; }
private:

	void Release();		//開放
	kdModel*	m_pModel = nullptr;
	KdMatrix	m_mWorld;
	float		m_speed  = 0.5f;
	bool        m_alive;
	int         m_lifeSpan = 0;
};