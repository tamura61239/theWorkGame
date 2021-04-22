#pragma once
#include <stdio.h>
#include<vector>

//ファイル操作クラス
class FileFunction
{
public:
	template<class T>
	//ファイルの読み込み
	static void Load(T& data, const char* fileName, const char* readMode)
	{
		FILE* fp;
		//ファイルを開くする
		if (fopen_s(&fp, fileName, readMode) != 0)return;
		//ファイルのサイズを読み取る
		long size = 0;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		//ファイルからデータを読み取る
		fread(&data, size, 1, fp);
		//ファイルを閉じる
		fclose(fp);
	}
	template<class T>
	//ファイルの読み込み
	static void LoadArray(T* data, const char* fileName, const char* readMode)
	{
		FILE* fp;
		//ファイルを開くする
		if (fopen_s(&fp, fileName, readMode) != 0)return;
		//ファイルのサイズを読み取る
		long size = 0;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int num = size / sizeof(T);
		//ファイルからデータを読み取る
		fread(&data[0], sizeof(T), num, fp);
		//ファイルを閉じる
		fclose(fp);
	}
	template<class T>
	//ファイルの読み込み
	static void LoadArray(std::vector<T>& data, const char* fileName, const char* readMode)
	{
		FILE* fp;
		//ファイルを開くする
		if (fopen_s(&fp, fileName, readMode) != 0)return;
		//ファイルのサイズを読み取る
		long size = 0;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		int num = size / sizeof(T);
		data.resize(num);
		//ファイルからデータを読み取る
		fread(&data[0], sizeof(T), num, fp);
		//ファイルを閉じる
		fclose(fp);
	}

	template<class T>
	//ファイルの書き込み
	static void Save(T& data, const char* fileName, const char* writeMode)
	{
		FILE* fp;
		//ファイルを開くする
		fopen_s(&fp, fileName, writeMode);
		//ファイルにデータを書き込む
		fwrite(&data, sizeof(T), 1, fp);
		//ファイルを閉じる
		fclose(fp);

	}
	template<class T>
	//ファイルの書き込み
	static void SaveArray(T* data, int num, const char* fileName, const char* writeMode)
	{
		FILE* fp;
		//ファイルを開くする
		fopen_s(&fp, fileName, writeMode);
		//ファイルにデータを書き込む
		fwrite(&data[0], sizeof(T), num, fp);
		//ファイルを閉じる
		fclose(fp);

	}

};