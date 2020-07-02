#pragma once

class CameraComponent;
class InputComponent;
class ModelComponent;

struct SphereInfo;

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

	virtual void Deserialize(const json11::Json& jsonObj);
	virtual void Update();
	virtual void Draw();

	inline const KdMatrix& GetMatrix() const { return m_mWorld; }
	inline void SetMatrix(const KdMatrix& rMat) { m_mWorld = rMat; }
	inline bool IsAlive() const { return m_alive; }
	inline void Destroy() { m_alive = false; }

	inline void SetTag(UINT tag) { m_tag = tag; }
	inline UINT GetTag() const { return m_tag; }

	//カメラコンポーネント取得
	std::shared_ptr<CameraComponent> GetCameraComponent() { return m_spCameraComponent; }

	//球による当たり判定
	bool HitCheckBySphere(const SphereInfo& rInfo);
protected:

	virtual void Release();

	//カメラコンポーネント取得
	std::shared_ptr<CameraComponent> m_spCameraComponent = std::make_shared<CameraComponent>(*this);

	//インップコンポーネント取得
	std::shared_ptr<InputComponent> m_spInputComponent = std::make_shared<InputComponent>(*this);

	//モデルコンポーネント取得
	std::shared_ptr<ModelComponent> m_spModelComponent = std::make_shared<ModelComponent>(*this);
	
	float m_colRadius = 2.0f;

	KdMatrix	m_mWorld;
	bool		m_alive = true;
	UINT		m_tag = OBJECT_TAG::TAG_None;
	std::string m_name = "GameObject";
};

std::shared_ptr<GameObject> CreateGameObject(const std::string& name);

//球判定に使う球データ
struct SphereInfo
{
	KdVec3 m_pos{};
	float m_radius = 0.0f;
};
