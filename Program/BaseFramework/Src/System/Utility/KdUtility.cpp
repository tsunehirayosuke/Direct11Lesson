﻿#include "System/KdSystem.h"

#include "KdUtility.h"

// viewから画像情報を取得する
void KdGetTextuxxreInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc)
{
	outDesc = {};

	ID3D11Resource* res;
	view->GetResource(&res);

	ID3D11Texture2D* tex2D;
	if (SUCCEEDED(res->QueryInterface<ID3D11Texture2D>(&tex2D)))
	{
		tex2D->GetDesc(&outDesc);
		tex2D->Release();
	}
	res->Release();
}

void KdMergePrefab(json11::Json& rSrcJson)
{
	//プレハブ指定有の場合は、プレハブ側のものをベースにこのJSONをマージする
	std::string prefabFilename = rSrcJson["Prefab"].string_value();

	if (prefabFilename.size() > 0)
	{
		json11::Json prefJson = KdResFac.GetJSON(prefabFilename);

		//JSON読み込み
		if (prefJson.is_null() == false)
		{
			//マージする
			for (auto&& n : rSrcJson.object_items())
			{
				prefJson[n.first] = n.second;
			}
		//マージしたものに差し替え
			rSrcJson = prefJson;

		}
	}
}
