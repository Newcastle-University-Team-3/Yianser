
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "AssetManager.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/MeshGeometry.h"
#include "../../Common/TextureLoader.h"
#include <experimental/filesystem>
#include "../../Common/Assets.h"
#include "../../Common/MeshMaterial.h"
#include "../../Common/Matrix4.h"
#include "AssimpHelper.h"

namespace NCL
{
	AssetManager* AssetManager::m_Instance = nullptr;

	AssetManager::AssetManager()
	{		
		AssimpHelper::GetInstance().Init();
		LoadMeshes();
		LoadTextures();
		LoadMaterials();
	}
	void AssetManager::LoadMeshes()
	{
		auto loadFunc = [](const std::string& name) {
			NCL::Rendering::OGLMesh* into = new NCL::Rendering::OGLMesh(name);
			(into)->SetPrimitiveType(NCL::GeometryPrimitive::Triangles);
			(into)->UploadToGPU();
			return into;
		};
		std::string filename;			

		for (const auto& entry : std::experimental::filesystem::directory_iterator(Assets::MESHDIR))
		{
			if (entry.path().extension().generic_string().compare(".msh") == 0)			
			{
				filename = entry.path().filename().generic_string();
				NCL::Rendering::OGLMesh* mesh = loadFunc(filename.c_str());				
				m_Meshes.insert({filename, mesh});
			}
			if (entry.path().extension().generic_string().compare(".fbx") == 0)
			{
				filename = entry.path().filename().generic_string();
				NCL::Rendering::OGLMesh* into = AssimpHelper::GetInstance().ProcessFBX(entry.path().generic_string().c_str());				
				(into)->SetPrimitiveType(NCL::GeometryPrimitive::Triangles);
				(into)->UploadToGPU();
				m_Meshes.insert({ filename, into });
			}
		}

	}
	void AssetManager::LoadTextures()
	{
		NCL::Rendering::OGLTexture *basicTex = (NCL::Rendering::OGLTexture*)
												TextureLoader::LoadAPITexture("checkerboard.png");
		m_Textures.insert({"checkerboard", basicTex});
	}
	void AssetManager::LoadMaterials()
	{
		std::string filename;
		for (const auto& entry : std::experimental::filesystem::directory_iterator(Assets::MESHDIR))
		{
			if (entry.path().extension().generic_string().compare(".mat") == 0)
			{
				filename = entry.path().filename().generic_string();
				NCL::MeshMaterial* material = new NCL::MeshMaterial(filename.c_str());
				material->LoadTextures();
				m_Materials.insert({filename, material});
			}
		}
	}
	AssetManager* AssetManager::GetInstance()
	{
		return m_Instance;
	}

	void AssetManager::Init()
	{
		m_Instance = new AssetManager();
	}

	void AssetManager::Finish()
	{
		delete m_Instance;
	}

	AssetManager::~AssetManager()
	{
		for (auto& i : m_Meshes)
		{
			delete i.second;
		}

		for (auto& i : m_Textures)
		{
			delete i.second;
		}

		for (auto& i : m_Materials)
		{
			delete i.second;
		}
	}

	NCL::Rendering::OGLMesh* AssetManager::GetMesh(const char* name)
	{
		return m_Meshes.at(name);
	}

	NCL::Rendering::OGLTexture* AssetManager::GetTexture(const char* name)
	{
		return m_Textures.at(name);
	}

	NCL::MeshMaterial* AssetManager::GetMaterial(const char*name)
	{
		return m_Materials.at(name);
	}
};