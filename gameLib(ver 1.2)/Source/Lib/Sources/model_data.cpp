#include "model_data.h"
#include <Shlwapi.h>
#include <fstream>
#include"fbx_load.h"
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

ModelData::ModelData(const char* filename,  const bool textureFlag ,int filetype)
{
	//既にシリアライズしたファイルがあるかどうかを調べる
	std::string fbxName = filename;
	size_t engineer = fbxName.find_last_of(".") + 1;
	std::string name = fbxName.substr(0, engineer);
	name += "bin";
	if (PathFileExistsA((name).c_str()))
	{//あったらそのファイルからデータを取得
		std::ifstream ifs;

		ifs.open((name).c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);

	}
	else
	{//なかったらFbxLoadクラスを使ってfbxファイルからデータを取得
		FbxLoader load;
		load.Load(filename, *this, textureFlag, filetype);
		//データをシリアライズしてセーブする
		std::ofstream ofs;
		ofs.open((name).c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}

}
