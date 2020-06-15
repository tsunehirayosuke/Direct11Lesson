#pragma once

class CameraComponent;

enum OBJECT_TAG
{
	TAG_None		 = 0x00000000,//属性無し、初期設定
	TAG_Character	 = 0x00000001,//キャラクター属性
	TAG_Player		 = 0x00000002,//プレイヤー属性
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:

	GameObject();
	virtual ~GameObject();

	virtual void Deserialise();
	virtual void Update();
	virtual void Draw();

	inline void SetMatrix(const KdMatrix& rMat) { m_mWorld = rMat; }
	inline bool IsAlive() const { return m_alive; }
	inline void Destroy() { m_alive = false; }

	inline void SetTag(UINT tag) { m_tag = tag; }
	inline UINT GetTag() const { return m_tag; }

	//カメラコンポーネント取得
	std::shared_ptr<CameraComponent> GetCameraComponent() { return m_spCameraComponent; }
protected:

	virtual void Release();

	std::shared_ptr<CameraComponent> m_spCameraComponent = std::make_shared<CameraComponent>(*this);

	std::shared_ptr<kdModel> m_spModel = nullptr;
	KdMatrix	m_mWorld;
	bool		m_alive = true;
	UINT		m_tag = OBJECT_TAG::TAG_None;
};