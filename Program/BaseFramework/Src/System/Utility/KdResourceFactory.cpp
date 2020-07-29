#include"KdResourceFactory.h"

std::shared_ptr<kdModel> KdResourceFactory::GetModel(const std::string& filename)
{
	//filenameのモデルがあるか
	auto itFound = m_modelmap.find(filename);
	//ない場合
	if (itFound == m_modelmap.end())
	{
		//生成＆読み込み
		auto newModel = std::make_shared<kdModel>();
		if (newModel->Load(filename) == false)
		{
			return nullptr;
		}
		//リスト(map)に登録
		m_modelmap[filename] = newModel;
		//リソースを返す
		return newModel;
	}
	else
	{
		return(*itFound).second;
	}
}

std::shared_ptr<KdTexture> KdResourceFactory::GetTexture(const std::string& filename)
{
	//filenameのテクスチャーがあるか？
	auto itFound = m_texmap.find(filename);
	//ない場合
	if (itFound == m_texmap.end())
	{
		//生成＆読み込み
		auto newTexture = std::make_shared<KdTexture>();
		if (newTexture->Load(filename) == false)
		{
			//	読み込み失敗時にはnullを返す
			return nullptr;
		}
		//リスト(map)に登録
		m_texmap[filename] = newTexture;
		//リソースを返す
		return newTexture;
	}
	//ある場合
	else
	{
		return (*itFound).second;
	}
}

json11::Json KdResourceFactory::GetJSON(const std::string& filename)
{
	//filenameの物があるかないか
	auto itFound = m_jsonmap.find(filename);
	//ない場合
	if (itFound == m_jsonmap.end())
	{

		//JSONファイルを文字列として読み込む
		json11::Json json = KdLoadJSON(filename);
		if (json.is_null())
		{
			assert(0 && "[GetJSON]jsonファイルが見つからない");

			return nullptr;
		}

		//登録
		m_jsonmap[filename] = json;

		//返す
		return json;
	}
	//ある場合
	else
	{
		return (*itFound).second;
	}
}

//Json読み込み
inline json11::Json KdResourceFactory::KdLoadJSON(const std::string& filename)
{
	//JSONファイルを開く
	std::ifstream ifs(filename);
	if (ifs.fail()) 
	{
		return nullptr;
		assert(0 && "Jsonのファイルのパスが違います"); 
	}


	//文字列として読み込み
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	//文字列のJSONを解析する
	std::string err;

	json11::Json jsonobj = json11::Json::parse(strJson, err);
	if (err.size() > 0)
	{
		assert(0 && "読み込んだファイルのJson変換に失敗");
		return nullptr;
	}

	return jsonobj;
}