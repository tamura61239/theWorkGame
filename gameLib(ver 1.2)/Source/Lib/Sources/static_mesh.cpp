#include "static_mesh.h"
#include <Shlwapi.h>
#include <fstream>
#include <vector>
#include <functional>


#include "misc.h"
#include"texture.h"
#include"shader.h"
#include"camera_manager.h"
/*************************型変換関数************************/
void fbxamatrix_to_xmfloat4x4(const FbxAMatrix& fbxamatrix, DirectX::XMFLOAT4X4& xmfloat4x4)
{
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
}

/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/
/****************************コンストラクタ*************************/

StaticMesh::StaticMesh(ID3D11Device* device, const char* fileName, SHADER_TYPE shaderType, bool pathOrganize, int organizeType)
{
	std::string fbxName = fileName;
	size_t engineer = fbxName.find_last_of(".") + 1;
	std::string name = fbxName.substr(0, engineer);
	name += "bin";
	if (PathFileExistsA((name).c_str()))
	{
		std::ifstream ifs;

		ifs.open((name).c_str(), std::ios::binary);
		cereal::BinaryInputArchive i_archive(ifs);
		i_archive(*this);

	}
	else
	{
		CreateMesh(device, fileName, pathOrganize, organizeType);
		std::ofstream ofs;
		ofs.open((name).c_str(), std::ios::binary);
		cereal::BinaryOutputArchive o_archive(ofs);
		o_archive(*this);
	}
	SetMinAndMaxPosition();
	CreateBuffers(device);
	CreateShaderResourceView(device, shaderType);
	mShaderType = shaderType;
}

/**************************メッシュの最大値と最小値を取得***************************/

void StaticMesh::SetMinAndMaxPosition()
{
	mMinPosition = mMaxPosition = VECTOR3F(0, 0, 0);
	for (auto& mesh : mMeshes)
	{
		for (auto& vertex : mesh.vertices)
		{
			mMinPosition.x = mMinPosition.x < vertex.position.x ? mMinPosition.x : vertex.position.x;
			mMinPosition.y = mMinPosition.y < vertex.position.y ? mMinPosition.y : vertex.position.y;
			mMinPosition.z = mMinPosition.z < vertex.position.z ? mMinPosition.z : vertex.position.z;

			mMaxPosition.x = mMaxPosition.x > vertex.position.x ? mMaxPosition.x : vertex.position.x;
			mMaxPosition.y = mMaxPosition.y > vertex.position.y ? mMaxPosition.y : vertex.position.y;
			mMaxPosition.z = mMaxPosition.z > vertex.position.z ? mMaxPosition.z : vertex.position.z;

		}
	}
}
/**************************FBXからデータを取得***************************/

void StaticMesh::CreateMesh(ID3D11Device* device, const char* fileName, bool pathOrganize, int organizeType)
{
	// Create the FBX SDK manager
	FbxManager* manager = FbxManager::Create();

	// Create an IOSettings object. IOSROOT is defined in Fbxiosettingspath.h.
	manager->SetIOSettings(FbxIOSettings::Create(manager, IOSROOT));

	// Create an importer.
	FbxImporter* importer = FbxImporter::Create(manager, "");

	// Initialize the importer.
	bool importStatus = false;
	importStatus = importer->Initialize(fileName, -1, manager->GetIOSettings());
	_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

	// Create a new scene so it can be populated by the imported file.
	FbxScene* scene = FbxScene::Create(manager, "");

	// Import the contents of the file into the scene.
	importStatus = importer->Import(scene);
	_ASSERT_EXPR_A(importStatus, importer->GetStatus().GetErrorString());

	// Convert mesh, NURBS and patch into triangle mesh
	fbxsdk::FbxGeometryConverter geometryConverter(manager);
	geometryConverter.Triangulate(scene, /*replace*/true);

	// Fetch node attributes and materials under this node recursively. Currently only mesh.	
	std::vector<FbxNode*> fetchedMeshes;
	std::function<void(FbxNode*)> traverse = [&](FbxNode* node)
	{
		if (node)
		{
			FbxNodeAttribute* fbxNodeAttribute = node->GetNodeAttribute();
			if (fbxNodeAttribute)
			{
				switch (fbxNodeAttribute->GetAttributeType())
				{
				case FbxNodeAttribute::eMesh:
					fetchedMeshes.push_back(node);
					break;
				}
			}
			for (int i = 0; i < node->GetChildCount(); i++)
			{
				traverse(node->GetChild(i));
			}
		}
	};
	traverse(scene->GetRootNode());

	mMeshes.resize(fetchedMeshes.size());

	//FbxMesh *fbx_mesh = fetched_meshes.at(0)->GetMesh();  // Currently only one mesh.
	for (size_t i = 0; i < fetchedMeshes.size(); i++)
	{
		FbxMesh* fbxMesh = fetchedMeshes.at(i)->GetMesh();
		Mesh& mesh = mMeshes.at(i);

		FbxAMatrix globalTransform = fbxMesh->GetNode()->EvaluateGlobalTransform(0);
		fbxamatrix_to_xmfloat4x4(globalTransform, mesh.globalTransform);

		// Fetch material properties.
		const int numberOfMaterials = fbxMesh->GetNode()->GetMaterialCount();

		//subsets.resize(number_of_materials);
		//subsets.resize(number_of_materials > 0 ? number_of_materials : 1);

		mesh.subsets.resize(numberOfMaterials > 0 ? numberOfMaterials : 1);
		for (int indexOfMaterial = 0; indexOfMaterial < numberOfMaterials; ++indexOfMaterial)
		{
			//subset &subset = subsets.at(index_of_material);
			Subset& subset = mesh.subsets.at(indexOfMaterial);

			const FbxSurfaceMaterial* surfaceMaterial = fbxMesh->GetNode()->GetMaterial(indexOfMaterial);

			FbxTettureNameLoad(FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, subset.diffuse, surfaceMaterial, fileName, pathOrganize, organizeType);
			FbxTettureNameLoad(FbxSurfaceMaterial::sNormalMap, FbxSurfaceMaterial::sBumpFactor, subset.normal, surfaceMaterial, fileName, pathOrganize, organizeType);
			FbxTettureNameLoad(FbxSurfaceMaterial::sBump, FbxSurfaceMaterial::sBumpFactor, subset.bump, surfaceMaterial, fileName, pathOrganize, organizeType);
		}

		// Count the polygon count of each material
		if (numberOfMaterials > 0)
		{
			// Count the faces of each material
			const int numberOfPolygons = fbxMesh->GetPolygonCount();
			for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; ++indexOfPolygon)
			{
				const u_int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);

				//subsets.at(materialIndex).indexCount += 3;
				mesh.subsets.at(materialIndex).indexCount += 3;

			}

			// Record the offset (how many vertex)
			int offset = 0;

			//for (subset &subset : subsets)
			for (Subset& subset : mesh.subsets)
			{
				subset.indexStart = offset;
				offset += subset.indexCount;
				// This will be used as counter in the following procedures, reset to zero
				subset.indexCount = 0;
			}
		}

		// Fetch mesh data
		u_int vertexCount = 0;

		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);

		const FbxVector4* arrayOfControlPoints = fbxMesh->GetControlPoints();
		const int numberOfPolygons = fbxMesh->GetPolygonCount();
		mesh.indices.resize(numberOfPolygons * 3);
		Face f;
		for (int indexOfPolygon = 0; indexOfPolygon < numberOfPolygons; indexOfPolygon++)
		{
			// The material for current face.
			int indexOfMaterial = 0;
			if (numberOfMaterials > 0)
			{
				indexOfMaterial = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(indexOfPolygon);
			}

			// Where should I save the vertex attribute index, according to the material
			//subset &subset = subsets.at(index_of_material);
			Subset& subset = mesh.subsets.at(indexOfMaterial);
			const int indexOffset = subset.indexStart + subset.indexCount;
			f.materialIndex = indexOfMaterial;
			for (int indexOfVertex = 0; indexOfVertex < 3; indexOfVertex++)
			{
				Vertex vertex;
				const int indexOfControlPoint = fbxMesh->GetPolygonVertex(indexOfPolygon, indexOfVertex);
				vertex.position.x = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][0]);
				vertex.position.y = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][1]);
				vertex.position.z = static_cast<float>(arrayOfControlPoints[indexOfControlPoint][2]);

				if (fbxMesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(indexOfPolygon, indexOfVertex, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}

				if (fbxMesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbxMesh->GetPolygonVertexUV(indexOfPolygon, indexOfVertex, uvNames[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				//面ごとの頂点データ
				DirectX::XMVECTOR facePosition = DirectX::XMLoadFloat3(&vertex.position);
				DirectX::XMMATRIX fW = DirectX::XMLoadFloat4x4(&mesh.globalTransform);
				DirectX::XMVECTOR fLP = DirectX::XMVector3TransformCoord(facePosition, fW);
				DirectX::XMStoreFloat3(&f.position[indexOfVertex], fLP);
				mesh.vertices.push_back(vertex);
				mesh.indices.at(indexOffset + indexOfVertex) = static_cast<u_int>(vertexCount);
				vertexCount += 1;
			}
			subset.indexCount += 3;
			//面データの保存
			mesh.faces.push_back(f);
		}
		//create_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());
	}
	manager->Destroy();

}


/*******************************FBXのテクスチャデータを取得**********************************/
void StaticMesh::FbxTettureNameLoad(const char* property_name, const char* factor_name, Material& material,
	const FbxSurfaceMaterial* surfaceMaterial, const char* fileName, bool pathOrganize, int organizeType)
{
	const FbxProperty property = surfaceMaterial->FindProperty(property_name);
	const FbxProperty factor = surfaceMaterial->FindProperty(factor_name);
	if (property.IsValid() && factor.IsValid())
	{
		FbxDouble3 color = property.Get<FbxDouble3>();
		double f = factor.Get<FbxDouble>();
		material.color.x = static_cast<float>(color[0] * f);
		material.color.y = static_cast<float>(color[1] * f);
		material.color.z = static_cast<float>(color[2] * f);
		material.color.w = 1.0f;
	}
	if (property.IsValid())
	{
		const int numberOfTextures = property.GetSrcObjectCount<FbxFileTexture>();
		if (numberOfTextures)
		{
			const FbxFileTexture* fileTexture = property.GetSrcObject<FbxFileTexture>();
			if (fileTexture)
			{
				const char* filename = fileTexture->GetRelativeFileName();

				wchar_t textureUnicode[256];

				if (pathOrganize)
				{
					char file[256], textureFileName[256], textureFileType[256];

					_splitpath_s(filename, nullptr, 0, nullptr, 0, textureFileName, 256, textureFileType, 256);

					if (strcmp(textureFileType, ".tga") == 0)
					{
						switch (organizeType)
						{
						case 0:
							strcat_s(textureFileType, 256, ".png");
							break;
						case 1:
							strcpy_s(textureFileType, 256, ".png");
							break;
						case 2:
							break;
						}
					}
					char dirname[256];
					::_splitpath_s(fileName, nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);

					::_makepath_s(file, 256, nullptr, dirname, textureFileName, textureFileType);


					MultiByteToWideChar(CP_ACP, 0, file, static_cast<int>(strlen(file) + 1), textureUnicode, 1024);

				}
				else
				{
					wchar_t fbxUnicode[256];
					MultiByteToWideChar(CP_ACP, 0, fileName, static_cast<int>(strlen(fileName) + 1), fbxUnicode, 1024);
					MultiByteToWideChar(CP_ACP, 0, fileTexture->GetFileName(), static_cast<int>(strlen(fileTexture->GetFileName()) + 1), textureUnicode, 1024);
					CombineResourcePath(textureUnicode, fbxUnicode, textureUnicode);
				}
				material.textureName = textureUnicode;





				//material.textureFilename = texture_unicode;
			}
		}
	}

}
/*****************************バッファの生成******************************/
void StaticMesh::CreateBuffers(ID3D11Device* device)
{
	HRESULT hr;

	for (auto& mesh : mMeshes)
	{
		//頂点バッファの生成
		{
			D3D11_BUFFER_DESC desc = {};
			D3D11_SUBRESOURCE_DATA data = {};

			desc.ByteWidth = sizeof(Vertex) * mesh.vertices.size();
			//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			data.pSysMem = mesh.vertices.data();
			data.SysMemPitch = 0; //Not use for vertex buffers.mm 
			data.SysMemSlicePitch = 0; //Not use for vertex buffers.

			hr = device->CreateBuffer(&desc, &data, mesh.vertexBuffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		//インテックスバッファの生成
		{
			D3D11_BUFFER_DESC desc = {};
			D3D11_SUBRESOURCE_DATA data = {};

			desc.ByteWidth = sizeof(u_int) * mesh.indices.size();
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			data.pSysMem = mesh.indices.data();
			data.SysMemPitch = 0; //Not use for index buffers.
			data.SysMemSlicePitch = 0; //Not use for index buffers.
			hr = device->CreateBuffer(&desc, &data, mesh.indexBuffer.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
	}
}
/*******************************テクスチャを生成**********************************/
void StaticMesh::CreateShaderResourceView(ID3D11Device* device, SHADER_TYPE shaderType)
{
	for (auto& mesh : mMeshes)
	{
		for (auto& subset : mesh.subsets)
		{
			if (wcscmp(subset.diffuse.textureName.c_str(), L"") != 0)
			{
				D3D11_TEXTURE2D_DESC texture2dDesc;
				LoadTextureFromFile(device, subset.diffuse.textureName.c_str(), subset.diffuse.SRV.GetAddressOf(), &texture2dDesc);
			}
			if (!subset.diffuse.SRV)
			{
				MakeDummyTexture(device, subset.diffuse.SRV.GetAddressOf());
			}
			if (wcscmp(subset.normal.textureName.c_str(), L"") != 0 && shaderType == SHADER_TYPE::NORMAL)
			{
				D3D11_TEXTURE2D_DESC texture2dDesc;
				LoadTextureFromFile(device, subset.normal.textureName.c_str(), subset.normal.SRV.GetAddressOf(), &texture2dDesc);
			}
			if (!subset.normal.SRV)
			{
				MakeDummyTexture(device, subset.normal.SRV.GetAddressOf());
			}
			if (wcscmp(subset.bump.textureName.c_str(), L"") != 0 && shaderType == SHADER_TYPE::NORMAL)
			{
				D3D11_TEXTURE2D_DESC texture2dDesc;
				LoadTextureFromFile(device, subset.bump.textureName.c_str(), subset.bump.SRV.GetAddressOf(), &texture2dDesc);
			}
			if (!subset.bump.SRV)
			{
				MakeDummyTexture(device, subset.bump.SRV.GetAddressOf());
			}

		}
	}
}

/*****************************************************/
//　　　　　　　　　　レイピック関数
/*****************************************************/

int StaticMesh::RayPick(const VECTOR3F& startPosition, const VECTOR3F& endPosition, VECTOR3F* outPosition, VECTOR3F* outNormal, float* outLength)
{
	int ret = -1;
	DirectX::XMVECTOR start = DirectX::XMLoadFloat3(&startPosition);
	DirectX::XMVECTOR end = DirectX::XMLoadFloat3(&endPosition);
	DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(end, start);
	DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(vec);
	float neart;
	DirectX::XMStoreFloat(&neart, length);

	DirectX::XMVECTOR position, normal;
	for (auto& mesh : mMeshes)
	{
		for (auto& face : mesh.faces)
		{
			//面頂点取得
			DirectX::XMVECTOR a = DirectX::XMLoadFloat3(&face.position[0]);
			DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&face.position[1]);
			DirectX::XMVECTOR c = DirectX::XMLoadFloat3(&face.position[2]);
			//3辺算出
			DirectX::XMVECTOR ab = DirectX::XMVectorSubtract(b, a);
			DirectX::XMVECTOR bc = DirectX::XMVectorSubtract(c, b);
			DirectX::XMVECTOR ca = DirectX::XMVectorSubtract(a, c);
			//外積による法線算出
			DirectX::XMVECTOR n = DirectX::XMVector3Cross(ab, bc);
			//内積の結果がプラスならば裏向き
			DirectX::XMVECTOR dot = DirectX::XMVector3Dot(dir, n);
			float fdot;
			DirectX::XMStoreFloat(&fdot, dot);
			if (fdot >= 0)continue;
			//交点算出
			DirectX::XMVECTOR cp;
			DirectX::XMVECTOR as = DirectX::XMVectorSubtract(a, start);
			DirectX::XMVECTOR d1 = DirectX::XMVector3Dot(n, as);
			DirectX::XMVECTOR x = DirectX::XMVectorDivide(d1, dot);//割り算
			float xleng;
			DirectX::XMStoreFloat(&xleng, x);
			if (xleng < .0f || xleng > neart) continue;
			cp = DirectX::XMVectorAdd(DirectX::XMVectorMultiply(dir, x), start);//足し算、かけ算
			//内点判定
			DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(a, cp);
			DirectX::XMVECTOR temp = DirectX::XMVector3Cross(v1, ab);
			DirectX::XMVECTOR work = DirectX::XMVector3Dot(temp, n);
			float fwork;
			DirectX::XMStoreFloat(&fwork, work);
			if (fwork < 0.0f)continue;
			DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(b, cp);
			temp = DirectX::XMVector3Cross(v2, bc);
			work = DirectX::XMVector3Dot(temp, n);
			DirectX::XMStoreFloat(&fwork, work);
			if (fwork < 0.0f)continue;
			DirectX::XMVECTOR v3 = DirectX::XMVectorSubtract(c, cp);
			temp = DirectX::XMVector3Cross(v3, ca);
			work = DirectX::XMVector3Dot(temp, n);
			DirectX::XMStoreFloat(&fwork, work);
			if (fwork < 0.0f)continue;

			//情報保存
			position = cp;
			normal = n;
			neart = xleng;
			ret = face.materialIndex;
		}
	}
	if (ret != -1)
	{
		DirectX::XMStoreFloat3(outPosition, position);
		DirectX::XMStoreFloat3(outNormal, normal);
	}
	*outLength = neart;

	return ret;
}
/*****************************************************/
//　　　　　　　　　　セット関数
/*****************************************************/
void StaticMesh::SetShaderResouceView(ID3D11DeviceContext* context, Subset& subset)
{
	switch (mShaderType)
	{
	case SHADER_TYPE::USEALLY:
		context->PSSetShaderResources(0, 1, subset.diffuse.SRV.GetAddressOf());
		break;
	case SHADER_TYPE::NORMAL:
		context->PSSetShaderResources(0, 1, subset.diffuse.SRV.GetAddressOf());
		context->PSSetShaderResources(1, 1, subset.normal.SRV.GetAddressOf());
		context->PSSetShaderResources(2, 1, subset.bump.SRV.GetAddressOf());
		break;
	}
}

/**********************************テクスチャデータを変更する***************************************/
void StaticMesh::ChangeShaderResourceView(ID3D11Device* device, SHADER_TYPE shaderType, std::vector<TextureMapData> data)
{
	for (auto& mesh : mMeshes)
	{
		for (auto& subset : mesh.subsets)
		{
			for (auto& d : data)
			{
				if (wcscmp(subset.diffuse.textureName.c_str(), d.deffuseName) == 0)
				{
					subset.normal.textureName = d.normalName;
					subset.bump.textureName = d.bumpName;
					break;
				}
			}
		}
	}
	CreateShaderResourceView(device, shaderType);
}
/***********************************描画クラス**********************************/
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
MeshRender::MeshRender(ID3D11Device* device)
{
	HRESULT hr = S_OK;

	//シェーダーを生成する
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/static_mesh_vs.cso", "", "Data/shader/static_mesh_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc)));
		mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/static_mesh_normal_vs.cso", "", "Data/shader/static_mesh_normal_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc)));
		mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/static_mesh_motion_data_vs.cso", "Data/shader/static_mesh_motion_data_gs.cso", "Data/shader/static_mesh_motion_data_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc)));
		mShadowShader = std::make_unique<DrowShader>(device, "Data/shader/static_mesh_shadow_vs.cso", "", "Data/shader/static_mesh_shadow_ps.cso");
	}

	// 定数バッファを生成する
	{
		mCbScene = std::make_unique<ConstantBuffer<CbScene>>(device);
		mCbObj = std::make_unique<ConstantBuffer<CbObj>>(device);
		mCbBeforeObj = std::make_unique<ConstantBuffer<FLOAT4X4>>(device);
	}
}
/*****************************************************/
//　　　　　　　　　　シャドウマップの描画関数
/*****************************************************/
/***************************描画開始**************************/

void MeshRender::ShadowBegin(ID3D11DeviceContext* context)
{
	mShadowShader->Activate(context);
}
/***************************描画**************************/

void MeshRender::ShadowRender(ID3D11DeviceContext* context, StaticMesh* obj, const FLOAT4X4& world, const VECTOR4F& color)
{

	for (StaticMesh::Mesh& mesh : obj->mMeshes)
	{
		//定数バッファのデータを更新
		mCbObj->data.color = color;
		DirectX::XMStoreFloat4x4(&mCbObj->data.world, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&world));
		//GPU側にデータを送る
		mCbObj->Activate(context, 1, true);
		u_int stride = sizeof(StaticMesh::Vertex);
		u_int offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//描画
		for (StaticMesh::Subset& subset : mesh.subsets)
		{
			context->DrawIndexed(subset.indexCount, subset.indexStart, 0);
		}
		//GPU側に送ったデータを元に戻す
		mCbObj->DeActivate(context);
	}
}
/***************************描画終了**************************/

void MeshRender::ShadowEnd(ID3D11DeviceContext* context)
{
	//GPU側に送ったデータを元に戻す
	mShadowShader->Deactivate(context);
}
/*****************************************************/
//　　　　　　　　　　通常描画関数
/*****************************************************/
/***************************描画開始**************************/

void MeshRender::Begin(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const bool w)
{
	//定数バッファのデータを更新
	mCbScene->data.view = view;
	mCbScene->data.projection = projection;
	//GPU側にデータを送る
	mCbScene->Activate(context, 0, true, true, true);
}
/***************************描画**************************/

void MeshRender::Render(ID3D11DeviceContext* context, StaticMesh* obj, const FLOAT4X4& world, const VECTOR4F color)
{
	SHADER_TYPE shaderType = obj->GetShaderType();
	mShader[static_cast<int>(shaderType)]->Activate(context);

	for (StaticMesh::Mesh& mesh : obj->mMeshes)
	{
		//GPU側にデータを送る
		u_int stride = sizeof(StaticMesh::Vertex);
		u_int offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//定数バッファのデータを更新
		DirectX::XMStoreFloat4x4(&mCbObj->data.world, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&world));

		for (StaticMesh::Subset& subset : mesh.subsets)
		{
			mCbObj->data.color.x = subset.diffuse.color.x * color.x;
			mCbObj->data.color.y = subset.diffuse.color.y * color.y;
			mCbObj->data.color.z = subset.diffuse.color.z * color.z;
			mCbObj->data.color.w = color.w;
			//GPU側にデータを送る
			mCbObj->Activate(context, 1, true, true, true);
			obj->SetShaderResouceView(context, subset);
			//描画
			context->DrawIndexed(subset.indexCount, subset.indexStart, 0);
			//GPU側に送ったデータを元に戻す
			mCbObj->DeActivate(context);
		}
	}
	//GPU側に送ったデータを元に戻す
	ID3D11ShaderResourceView* srv = nullptr;
	for (int i = 0; i < 3; i++)
	{
		context->PSSetShaderResources(i, 1, &srv);
	}
	mShader[static_cast<int>(shaderType)]->Deactivate(context);

}
/***************************描画(シェーダーを取得)**************************/

void MeshRender::Render(ID3D11DeviceContext* context, DrowShader* shader, StaticMesh* obj, const FLOAT4X4& world, const VECTOR4F color)
{
	shader->Activate(context);
	for (StaticMesh::Mesh& mesh : obj->mMeshes)
	{
		//GPU側にデータを送る
		u_int stride = sizeof(StaticMesh::Vertex);
		u_int offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//定数バッファのデータを更新
		DirectX::XMStoreFloat4x4(&mCbObj->data.world, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&world));

		for (StaticMesh::Subset& subset : mesh.subsets)
		{
			mCbObj->data.color.x = subset.diffuse.color.x * color.x;
			mCbObj->data.color.y = subset.diffuse.color.y * color.y;
			mCbObj->data.color.z = subset.diffuse.color.z * color.z;
			mCbObj->data.color.w = color.w;
			//GPU側にデータを送る
			mCbObj->Activate(context, 1, true, true, true);
			obj->SetShaderResouceView(context, subset);
			//描画
			context->DrawIndexed(subset.indexCount, subset.indexStart, 0);
			//GPU側に送ったデータを元に戻す
			mCbObj->DeActivate(context);
		}
	}
	//GPU側に送ったデータを元に戻す
	ID3D11ShaderResourceView* srv = nullptr;
	for (int i = 0; i < 3; i++)
	{
		context->PSSetShaderResources(i, 1, &srv);
	}
	shader->Deactivate(context);

}
/***************************描画終了**************************/

void MeshRender::End(ID3D11DeviceContext* context)
{
	//GPU側に送ったデータを元に戻す
	mCbScene->DeActivate(context);
}
/*****************************************************/
//　　　　　　　　　　速度マップの描画関数
/*****************************************************/
/***************************描画開始**************************/
void MeshRender::VelocityBegin(ID3D11DeviceContext* context, const bool w)
{
	//GPU側にデータを送る
	mShader[2]->Activate(context);
}
/***************************描画**************************/

void MeshRender::VelocityRender(ID3D11DeviceContext* context, StaticMesh* obj, const FLOAT4X4& world, const FLOAT4X4& beforeWorld, const VECTOR4F color)
{
	for (StaticMesh::Mesh& mesh : obj->mMeshes)
	{
		//GPU側にデータを送る
		u_int stride = sizeof(StaticMesh::Vertex);
		u_int offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//定数バッファのデータを更新
		DirectX::XMStoreFloat4x4(&mCbObj->data.world, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&world));

		DirectX::XMStoreFloat4x4(&mCbBeforeObj->data, DirectX::XMLoadFloat4x4(&mesh.globalTransform) * DirectX::XMLoadFloat4x4(&beforeWorld));
		//GPU側にデータを送る
		mCbBeforeObj->Activate(context, 2, true, true, true);
		for (StaticMesh::Subset& subset : mesh.subsets)
		{
			//定数バッファのデータを更新
			mCbObj->data.color.x = subset.diffuse.color.x * color.x;
			mCbObj->data.color.y = subset.diffuse.color.y * color.y;
			mCbObj->data.color.z = subset.diffuse.color.z * color.z;
			mCbObj->data.color.w = color.w;
			//GPU側にデータを送る
			mCbObj->Activate(context, 1, true, true, true);
			//描画
			context->DrawIndexed(subset.indexCount, subset.indexStart, 0);
			//GPU側に送ったデータを元に戻す
			mCbObj->DeActivate(context);
		}
		mCbBeforeObj->DeActivate(context);
	}

}
/***************************描画終了**************************/

void MeshRender::VelocityEnd(ID3D11DeviceContext* context)
{
	//GPU側に送ったデータを元に戻す
	mShader[2]->Deactivate(context);

}
