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