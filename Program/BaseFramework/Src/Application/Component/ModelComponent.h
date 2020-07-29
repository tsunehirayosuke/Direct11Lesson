#pragma once

#include"../Game/GameObject.h"

//モデルコンポーネント

class ModelComponent
{
public:
	ModelComponent(GameObject& owner) :m_owner(owner){}

	//有効フラグ
	bool IsEnable()const { return m_enable; }
	void SetEnable(bool enable) { m_enable = enable; }

	//モデル取得
	inline const std::shared_ptr<kdModel> GetModel()const { return m_spModel; }

	//メッシュ取得
	inline const std::shared_ptr<KdMesh>GetMesh()const
	{
		if (m_spModel == nullptr) { return nullptr; }
		return m_spModel->GetMesh();
	}

	//モデルセット
	inline void SetModel(const std::shared_ptr<kdModel>& model) { m_spModel = model; }

	//StandardShaderで描画
	void Draw();

private:
	//有効
	bool m_enable = true;

	//モデルデータの参照
	std::shared_ptr<kdModel> m_spModel;

	GameObject& m_owner;
};