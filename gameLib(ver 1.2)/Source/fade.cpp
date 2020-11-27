#include "fade.h"
#include<string>
#ifdef USE_IMGUI
#include<imgui.h>
#endif

void Fade::StartLoad()
{
	FILE* fp;
	for (int i = 0; i < static_cast<int>(FADE_SCENE::MAX); i++)
	{
		std::string fileName = { "Data/file/fade_out" };
		fileName += std::to_string(i) + ".bin";
		if (fopen_s(&fp, fileName.c_str(), "rb") == 0)
		{
			fread(&mFadeDatas[i].mData, sizeof(FadeData), 1, fp);
			fclose(fp);
		}
		else
		{
			mFadeDatas[i].mData.mColor = VECTOR4F(0, 0, 0, 0);
			mFadeDatas[i].mData.mInEndTime = 0;
			mFadeDatas[i].mData.mOutEndTime = 0;
		}
		mFadeDatas[i].mScene = static_cast<FADE_SCENE>(i);
		mFadeDatas[i].mEndFlag = false;
	}
}

Fade::Fade(ID3D11Device* device, FADE_SCENE scene)
	:mTestFlag(false), mState(FADE_MODO::NONE), mEditorScene(static_cast<int>(scene))
{
	StartLoad();
	mNowFadeData = mFadeDatas[mEditorScene];
	mSprite = std::make_unique<Sprite>(device, L"Data/image/siro.png");
}

void Fade::SetFadeScene(FADE_SCENE scene)
{
	mEditorScene = static_cast<int>(scene);
	mNowFadeData = mFadeDatas[mEditorScene];
}


void Fade::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("fade out");
	if (!mTestFlag)
	{
		static const int sceneMax = static_cast<int>(FADE_SCENE::MAX);
		static const char* fadeNames[sceneMax] = { "TITLE","SELECT","GAME","RESULT" };
		ImGui::Combo("scene", &mEditorScene, fadeNames, sceneMax);
		float* color[3] = { &mFadeDatas[mEditorScene].mData.mColor.x,&mFadeDatas[mEditorScene].mData.mColor.y ,&mFadeDatas[mEditorScene].mData.mColor.z };
		ImGui::ColorEdit3("color", *color);
		ImGui::InputFloat("fade in time", &mFadeDatas[mEditorScene].mData.mInEndTime, 0.1f);
		ImGui::InputFloat("fade out time", &mFadeDatas[mEditorScene].mData.mOutEndTime, 0.1f);
		if (ImGui::Button("save"))
		{
			Save(mEditorScene);
		}
		if (ImGui::Button("load"))
		{
			Load(mEditorScene);
		}
	}
	else
	{
		int state = static_cast<int>(mState);
		if (ImGui::RadioButton("in", &state, 1))
		{
			mState = FADE_MODO::FADEIN;
			mCheckFadeData.mData.mColor.w = 1;
		}
		if (ImGui::RadioButton("out", &state, 2))
		{
			mState = FADE_MODO::FADEOUT;
			mCheckFadeData.mData.mColor.w = 0;
		}
		//mState = static_cast<FADE_MODO>(state);
		ImGui::Text("state:%d", state);
	}
	int scene = static_cast<int>(mNowFadeData.mScene);
	if (ImGui::Checkbox("test move", &mTestFlag))
	{
		mCheckFadeData = mFadeDatas[scene];
		mState = FADE_MODO::NONE;
	}
	ImGui::End();
	if (mState == FADE_MODO::NONE)
	{
		mNowFadeData = mFadeDatas[scene];
	}
#endif
}

void Fade::Update(float elapsdTime)
{
	if (mTestFlag)
	{
		Move(elapsdTime, &mCheckFadeData);
	}
	else
	{
		Move(elapsdTime, &mNowFadeData);
	}
}

void Fade::Render(ID3D11DeviceContext* context)
{
	
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);
	if (mTestFlag)
	{
		mSprite->Render(context, VECTOR2F(0, 0), VECTOR2F(viewport.Width, viewport.Height), VECTOR2F(0, 0), VECTOR2F(1024, 1024), 0, mCheckFadeData.mData.mColor);
	}
	else if(mState!=FADE_MODO::NONE)
	{
		mSprite->Render(context, VECTOR2F(0, 0), VECTOR2F(viewport.Width, viewport.Height), VECTOR2F(0, 0), VECTOR2F(1024, 1024), 0, mNowFadeData.mData.mColor);
	}
}

void Fade::Load(int scene)
{
	FILE* fp;
	int nowScene = scene;
	std::string fileName = { "Data/file/fade_out" };
	fileName += std::to_string(nowScene) + ".bin";
	if (fopen_s(&fp, fileName.c_str(), "rb") == 0)
	{
		fread(&mFadeDatas[nowScene].mData, sizeof(FadeData), 1, fp);
		fclose(fp);
	}

}

void Fade::Save(int scene)
{
	FILE* fp;
	int nowScene = scene;
	std::string fileName = { "Data/file/fade_out" };
	fileName += std::to_string(nowScene) + ".bin";
	fopen_s(&fp, fileName.c_str(), "wb");
	fwrite(&mFadeDatas[nowScene].mData, sizeof(FadeData), 1, fp);
	fclose(fp);
}

void Fade::Move(float elapsdTime, FadeScene* scene)
{
	switch (mState)
	{
	case FADE_MODO::NONE:
		break;
	case FADE_MODO::FADEIN:
		scene->mData.mColor.w -= elapsdTime * (1 / scene->mData.mInEndTime);
		if (scene->mData.mColor.w <= 0)
		{
			scene->mEndFlag = true;
		}
		break;
	case FADE_MODO::FADEOUT:
		scene->mData.mColor.w += elapsdTime * (1 / scene->mData.mOutEndTime);
		if (scene->mData.mColor.w >= 1)
		{
			scene->mEndFlag = true;
		}
		break;

	}

}
