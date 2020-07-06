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

	json11::Json GetJSON(const std::string& filename);

	//管理を破棄する
	void Clear()
	{
		m_modelmap.clear();
		m_jsonmap.clear();
	}
private:
	//JSON読み込み
	json11::Json KdLoadJSON(const std::string& filename);

	//モデルデータ管理マップ
	std::unordered_map<std::string, std::shared_ptr<kdModel>> m_modelmap;

	std::unordered_map<std::string, json11::Json> m_jsonmap;


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

#define KdResFac KdResourceFactory::GetInstance()