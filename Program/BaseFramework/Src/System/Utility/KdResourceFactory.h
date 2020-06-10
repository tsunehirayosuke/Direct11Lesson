#pragma once

/////////////////////////////////////
///リソース管理クラス
///・デザインパターンのFlyweightパターンを採用
/////////////////////////////////////

class KdResourceFactory
{
public:
	//モデルデータ取得
	std::shared_ptr<kdModel> GetModel(const std::string& filename);

	//管理を破棄する
	void Clear()
	{
		m_modelmap.clear();
	}
private:
	//モデルデータ管理マップ
	std::unordered_map<std::string, std::shared_ptr<kdModel>> m_modelmap;

	/////////////////////////////////////
	///シングルトン化
	/////////////////////////////////////
private:
	KdResourceFactory() {};
public:
	static KdResourceFactory& GetInstance()
	{
		static KdResourceFactory instance;
		return instance;
	}
};