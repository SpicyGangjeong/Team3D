        #include "pch.h"
#include "Material.h"
#include "GameInstance.h"
#include "Shader.h"
#ifdef EDITOR_PROJECT
static EFileTypeId GetFileTypeId(const std::string& FileType)
{
	// 필요하면 thread-safe static 초기화 (C++11 이상에서 보장)[web:20]
	static const std::unordered_map<std::string, EFileTypeId> table = {
		{ "Diffuse Map",                EFileTypeId::Diffuse_Map },
		{ "Diffuse Texture",            EFileTypeId::Diffuse_Texture },
		{ "Diffuse A Map",              EFileTypeId::Diffuse_A_Map },
		{ "Difuse_1",                   EFileTypeId::Difuse_1 },
		{ "Base color",                 EFileTypeId::Base_color },
		{ "DiffuseTexture",             EFileTypeId::DiffuseTexture },
		{ "Distort texture",            EFileTypeId::Distort_texture },
		{ "Diffuse",                    EFileTypeId::Diffuse },
		{ "Lamp",                       EFileTypeId::Lamp },
		{ "WindowDiffuse",              EFileTypeId::WindowDiffuse },
		{ "Color Glass",                EFileTypeId::Color_Glass },
		{ "Window_Surface_Diffuse",     EFileTypeId::Window_Surface_Diffuse },
		{ "InteriorDiffuse",            EFileTypeId::InteriorDiffuse },
		{ "Diffuse Moss",               EFileTypeId::Diffuse_Moss },
		{ "Moss Diffuse",               EFileTypeId::Moss_Diffuse },
		{ "Diffuse C",                  EFileTypeId::Diffuse_C },
		{ "Diffuse D",                  EFileTypeId::Diffuse_D },
		{ "Cavity D",                   EFileTypeId::Cavity_D },
		{ "Diffuse A",                  EFileTypeId::Diffuse_A },
		{ "Object Diffuse",             EFileTypeId::Object_Diffuse },
		{ "Diffuse B",                  EFileTypeId::Diffuse_B },
		{ "Lichen D",                   EFileTypeId::Lichen_D },
		{ "Diffuse B Map",              EFileTypeId::Diffuse_B_Map },
		{ "TopBlend_Albedo",            EFileTypeId::TopBlend_Albedo },
		{ "Base colour",				EFileTypeId::Base_colour },

		{ "Normal Map",                 EFileTypeId::Normal_Map },
		{ "HRO Map",                    EFileTypeId::HRO_Map },
		{ "Normal_1",                   EFileTypeId::Normal_1 },
		{ "Object Normal",              EFileTypeId::Object_Normal },
		{ "Normal A Map",               EFileTypeId::Normal_A_Map },
		{ "Base Normal",                EFileTypeId::Base_Normal },
		{ "Object_Normal",              EFileTypeId::Object_Normal_Underbar },
		{ "NormalTexture",              EFileTypeId::NormalTexture },
		{ "Normal",                     EFileTypeId::Normal },
		{ "NormalMap",                  EFileTypeId::NormalMap },
		{ "Window_Surface_Normal",      EFileTypeId::Window_Surface_Normal },   // Normal 용
		{ "Cloth Detail Normal",        EFileTypeId::Cloth_Detail_Normal },
		{ "Detail Normal",              EFileTypeId::Detail_Normal },
		{ "TopBlend_Normal",            EFileTypeId::TopBlend_Normal },
		{ "Moss Normal",                EFileTypeId::Moss_Normal },
		{ "Cavity Detail N",            EFileTypeId::Cavity_Detail_N },
		{ "Moss Normal Map",            EFileTypeId::Moss_Normal_Map },

		{ "Normal Map A",               EFileTypeId::Normal_Map_A },
		{ "Normal Map B",               EFileTypeId::Normal_Map_B },
		{ "Lichen Detail N",            EFileTypeId::Lichen_Detail_N },
		{ "Normal B Map",               EFileTypeId::Normal_B_Map },

		{ "MRO",                        EFileTypeId::MRO },
		{ "MRO Map",                    EFileTypeId::MRO_Map },
		{ "WIndow_Surface_MRO",         EFileTypeId::WIndow_Surface_MRO },
		{ "PackedTexture",              EFileTypeId::PackedTexture },
		{ "MRO Map A",                  EFileTypeId::MRO_Map_A },
		{ "MRO Map B",                  EFileTypeId::MRO_Map_B },
		{ "MROH",                       EFileTypeId::MROH },
		{ "MROA Mask",                  EFileTypeId::MROA_Mask },
		{ "MROA Map",                   EFileTypeId::MROA_Map },
		{ "MROA",                       EFileTypeId::MROA },
		{ "MRS",                        EFileTypeId::MRS },

		{ "SRO Map",                    EFileTypeId::SRO_Map },
		{ "SRO",                        EFileTypeId::SRO },
		{ "Moss SRO",                   EFileTypeId::Moss_SRO },
		{ "SRO Map A",                  EFileTypeId::SRO_Map_A },
		{ "SRO Map D",                  EFileTypeId::SRO_Map_D },
		{ "SRO Map B",                  EFileTypeId::SRO_Map_B },
		{ "TopBlend_SRO",               EFileTypeId::TopBlend_SRO },
		{ "Detail SRO",                 EFileTypeId::Detail_SRO },
		{ "SRO B Map",                  EFileTypeId::SRO_B_Map },
		{ "SROH",                       EFileTypeId::SROH },
		{ "SROA",                       EFileTypeId::SROA },
		{ "SROA Map",                   EFileTypeId::SROA_Map },
		{ "SROH A Map",                 EFileTypeId::SROH_A_Map },

		{ "MROH/SROH Map A",            EFileTypeId::MROH_SROH_Map_A },
		{ "Object MRO/SRO",             EFileTypeId::Object_MRO_SRO },
		{ "MROH/SROH Map B",            EFileTypeId::MROH_SROH_Map_B },
		{ "MRO/SRO Map A",              EFileTypeId::MRO_SRO_Map_A },
		{ "MROH/SROH A",                EFileTypeId::MROH_SROH_A },
		{ "MRO/SRO Map B",              EFileTypeId::MRO_SRO_Map_B },
		{ "MRO/SRO B",                  EFileTypeId::MRO_SRO_B },
		{ "MROH/SROH B",                EFileTypeId::MROH_SROH_B },
		{ "Moss MRO/SRO Map",           EFileTypeId::Moss_MRO_SRO_Map },
		{ "Roughness",					EFileTypeId::Roughness },

		{ "RGBA_Mask",                  EFileTypeId::RGBA_Mask },
		{ "exterior_Cube_Map",          EFileTypeId::exterior_Cube_Map },
		{ "Interior_Cube_Map",          EFileTypeId::Interior_Cube_Map },

		{ "InteriorEmissive",           EFileTypeId::InteriorEmissive },
		{ "Emissive Map",               EFileTypeId::Emissive_Map },
		{ "Lamp Emissive",              EFileTypeId::Lamp_Emissive },
		{ "FakeExtDiffuse",             EFileTypeId::FakeExtDiffuse },
		{ "Diffuse with alpha",         EFileTypeId::Diffuse_with_alpha },
		{ "Emissive Texture",			EFileTypeId::Emissive_Texture },

		{ "Wind Position Map",          EFileTypeId::Wind_Position_Map },
		{ "Detail Diffuse",             EFileTypeId::Detail_Diffuse },
		{ "Subsurface",                 EFileTypeId::Subsurface },
		{ "Color Mask",                 EFileTypeId::Color_Mask },
		{ "Mask Texture (packed)",      EFileTypeId::Mask_Texture_packed },

		{ "Worn MRO",                   EFileTypeId::Worn_MRO },
		{ "Worn Normal",                EFileTypeId::Worn_Normal },
		{ "Detail MRO",                 EFileTypeId::Detail_MRO },
		{ "SRA",                        EFileTypeId::SRA },

		{ "Detail Diffuse A",           EFileTypeId::Detail_Diffuse_A },
		{ "Cavity-Lichen D Mask",       EFileTypeId::Cavity_Lichen_D_Mask },
		{ "Detail Diffuse B",           EFileTypeId::Detail_Diffuse_B },
		{ "Detail Albedo",              EFileTypeId::Detail_Albedo },
		{ "Top Layer Base Color",       EFileTypeId::Top_Layer_Base_Color },
		{ "Detail Normal A",            EFileTypeId::Detail_Normal_A },
		{ "Detail Normal B",            EFileTypeId::Detail_Normal_B },
		{ "Vertex Blend Texture ",      EFileTypeId::Vertex_Blend_Texture },
		{ "Water Color tex",            EFileTypeId::Water_Color_tex },
		{ "emissiveMask",               EFileTypeId::emissiveMask },
		{ "EmissiveAlphaMask",          EFileTypeId::EmissiveAlphaMask },
		{ "Distortion Map",             EFileTypeId::Distortion_Map },
		{ "Distortion Bottom",          EFileTypeId::Distortion_Bottom },
		{ "Distortion Top",             EFileTypeId::Distortion_Top },
		{ "Albedo",                     EFileTypeId::Albedo },
		{ "Displacement",               EFileTypeId::Displacement },
		{ "Material",                   EFileTypeId::Material },
		{ "Distortion Pattern",         EFileTypeId::Distortion_Pattern },
		{ "Ground Diffuse",             EFileTypeId::Ground_Diffuse },
		{ "Ground MRO/SRO",             EFileTypeId::Ground_MRO_SRO },
		{ "Corruption Mask",            EFileTypeId::Corruption_Mask },

		{ "Rock_Texture",               EFileTypeId::Rock_Texture },
		{ "Rock MRO",                   EFileTypeId::Rock_MRO },
		{ "Base MRO/SRO",               EFileTypeId::Base_MRO_SRO },
		{ "MRO/SRO",                    EFileTypeId::MRO_SRO },
		{ "Rock Details",               EFileTypeId::Rock_Details },
		{ "Rock baked Normals",         EFileTypeId::Rock_baked_Normals },
	};

	auto it = table.find(FileType);
	if (it != table.end())
		return it->second;
	return EFileTypeId::Unknown;
}
#endif // EDITOR_PROJECT

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
	, m_vSRV_Flag{ 0.f, 0.f}
	, m_vPBR_Flag{ 0.f, 0.f, 0.f}
{
	SAFE_ADDREF(m_pDevice);
	SAFE_ADDREF(m_pContext);
	SAFE_ADDREF(m_pGameInstance);
}
#ifdef EDITOR_PROJECT
HRESULT CMaterial::Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		_uint iNumSRVs = pAIMaterial->GetTextureCount(static_cast<aiTextureType>(i));

		m_SRVs[i].reserve(iNumSRVs);

		ID3D11ShaderResourceView* pSRV = { nullptr };

		HRESULT			hr = {};

		for (size_t j = 0; j < iNumSRVs; j++)
		{
			aiString	strTexturePath;

			if (FAILED(pAIMaterial->GetTexture(static_cast<aiTextureType>(i), (_uint)j, &strTexturePath))) {
				continue;
			}
			_char szTextureDir[MAX_PATH] = {};
			_char szSaveDir[MAX_PATH] = {};
			_char szDrive[MAX_PATH] = {};
			_char szDir[MAX_PATH] = {};
			_char szName[MAX_PATH] = {};
			_char szExt[MAX_PATH] = {};
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);
			_splitpath_s(strTexturePath.C_Str(), nullptr, 0, nullptr, 0, szName, MAX_PATH, szExt, MAX_PATH);
			strcpy_s(szTextureDir, sizeof(_char) * MAX_PATH, szDrive);
			strcat_s(szTextureDir, sizeof(_char) * MAX_PATH, szDir);
			strcat_s(szTextureDir, sizeof(_char) * MAX_PATH, szName);
			strcat_s(szTextureDir, sizeof(_char) * MAX_PATH, szExt);

			{
				_char		szTextureFileName[MAX_PATH] = {};
				_char		szTextureFilePath[MAX_PATH] = {};
				_char		szDrive[MAX_PATH] = {};
				_char		szDir[MAX_PATH] = {};
				_char		szFileName[MAX_PATH] = {};
				_char		szEXT[MAX_PATH] = {};
				_splitpath_s(szTextureDir, szDrive, MAX_PATH, szDir, MAX_PATH, szFileName, MAX_PATH, szEXT, MAX_PATH);
				strcpy_s(szTextureFileName, szDrive);
				strcat_s(szTextureFileName, szDir);
				strcat_s(szTextureFileName, szFileName);

				strcat_s(szTextureFilePath, szTextureFileName);
				strcat_s(szTextureFilePath, ".dds");

				hr = CreateDDSTextureFromFile(m_pDevice, CMyTools::ToWstring(szTextureFilePath).c_str(), nullptr, &pSRV); // 일단 dds로 시도하고
				if (FAILED(hr)) {
					memset(szTextureFilePath, 0, sizeof(_char) * MAX_PATH); // 실패하면 원래 확장자로 다시 시도
					strcat_s(szTextureFilePath, szTextureFileName);
					strcat_s(szTextureFilePath, szEXT);

					if (false == strcmp(".tga", szEXT)) {
						assert(false); // trying to Load TGA
						hr = S_OK;
					}
					else {
						hr = CreateWICTextureFromFile(m_pDevice, CMyTools::ToWstring(szTextureFilePath).c_str(), nullptr, &pSRV);
					}
				}
				m_SaveMaterial.Path[i].push_back(szTextureFilePath);
			}

			strcpy_s(szSaveDir, sizeof(_char) * MAX_PATH, szName);
			strcat_s(szSaveDir, sizeof(_char) * MAX_PATH, szExt);

			if (FAILED(hr)) {
				return E_FAIL;
			}
			//m_strPath[i].push_back(szSaveDir);
			m_SRVs[i].push_back(pSRV);
		}
	}

	return S_OK;
}
HRESULT CMaterial::Initialize(const _char* pModelFilePath, MODEL eType, const aiMaterial* pAIMaterial)
{
	ID3D11ShaderResourceView* pSRV = { nullptr };

	filesystem::path xmlFilePath = pModelFilePath;
	xmlFilePath.replace_extension(".xml");

	tinyxml2::XMLDocument xmldoc;
	if ((tinyxml2::XML_SUCCESS != xmldoc.LoadFile(xmlFilePath.string().c_str()))) {
		return E_FAIL;
	}

	tinyxml2::XMLElement* pRoot = xmldoc.FirstChildElement("Material");
	if (pRoot == nullptr){
		return E_FAIL;
	}

	aiString aistrName = pAIMaterial->GetName();
	string xmlMaterialName = aistrName.C_Str();

	tinyxml2::XMLElement* pElement = pRoot->FirstChildElement(xmlMaterialName.c_str());
	
	if (pElement == nullptr){
		return E_FAIL;
	}

	filesystem::path pathFolder = xmlFilePath.parent_path();

	for (tinyxml2::XMLElement* typeElement = pElement->FirstChildElement();
		typeElement != nullptr;
		typeElement = typeElement->NextSiblingElement())
	{
		_int typeIndex = typeElement->IntAttribute("Index", -1);
		if (typeIndex < 0){
			continue;
		}

		const _char* pFileNameAtt = typeElement->Attribute("Filename");
		const _char* pExtensionAtt = typeElement->Attribute("Extension");

		if (pFileNameAtt == nullptr || pFileNameAtt[0] == '\0'
		 || pExtensionAtt == nullptr || pExtensionAtt[0] == '\0') {
			continue;
		}

		filesystem::path pathResourceFile = pathFolder / pFileNameAtt;
		pathResourceFile.replace_extension(pExtensionAtt);

		pSRV = m_pGameInstance->Add_Resource(pathResourceFile.string().c_str());
		if (nullptr == pSRV){
			return E_FAIL;
		}

		m_SRVs[typeIndex].push_back(pSRV);
		m_SaveMaterial.Path[typeIndex].push_back(pathResourceFile.string());
	}

	return S_OK;
}
HRESULT CMaterial::Initialize(const _char* pMaterialFilePath, const _char* pTextureFilePath)
{
	_char szTextureFilePath[MAX_PATH] = {};
	_char szMaterialFilePath[MAX_PATH] = {};

	strcpy_s(szTextureFilePath, pTextureFilePath);

	strcpy_s(szMaterialFilePath, pMaterialFilePath);
	strcat_s(szMaterialFilePath, ".props.txt");

	if (FAILED(Read_MaterialFile(szMaterialFilePath, szTextureFilePath)))
		return E_FAIL;

	return S_OK;
}
HRESULT CMaterial::Read_MaterialFile(const _char* pMaterialFilePath, const _char* pTextureFolderPath)
{
	/* Read MaterialFile to Read TextureFilePath */

	ifstream file(pMaterialFilePath);

	if (!file.is_open())
	{
		//MSG_BOX("Failed to Open Materail File");
		//return E_FAIL;
		return S_OK;
	}

	_char szTextureFilePath[MAX_PATH] = {};

	string strText = {};

	string Value = {};
	string Type = {};
	string Use = {};

	_uint iDataIndex = {};
	_uint iNumParameter = {};

	_uint iBeginIndex = {};
	_uint iEndIndex = {};

	_bool bParent = { false };

	getline(file, strText);

	if (!strcmp(strText.c_str(), "none"))
		return S_OK;

	while (file)
	{
		if (2 < iDataIndex)
			break;

		if (false == bParent)
		{
			getline(file, strText);
			auto index = strText.find_first_of("[");
			auto iBackindex = strText.find_first_of("]");

			if (index == string::npos || iBackindex == string::npos)
			{
				return S_OK;
			}

			if (index + 1 != iBackindex - 1)
				iNumParameter = (strText[iBackindex - 1] - '0') + 10;
			else
				iNumParameter = strText[index + 1] - '0';

			getline(file, strText);
			bParent = true;
		}
		else
		{
			if (0 == iNumParameter)
			{
				++iDataIndex;
				auto index = strText.find_first_of("[");

				iNumParameter = strText[index + 1] - '0';
				getline(file, strText);
			}
			for (_uint i = 0; i < iNumParameter; ++i)
			{
				for (_uint j = 0; j < 3; ++j)
					getline(file, strText);

				switch (iDataIndex)
				{
					/* Scala */
				case 0:
					// value
					getline(file, strText);
					Value = strText.substr(iBeginIndex + 2);

					getline(file, strText);
					// name
					iBeginIndex = (_uint)strText.find_first_of('=');

					Type = strText.substr(iBeginIndex + 2);
					break;

					/* Texture */
				case 1:
					// name
					iBeginIndex = (_uint)strText.find_last_of('=');
					iEndIndex = (_uint)strText.rfind('}');

					Type = strText.substr(iBeginIndex + 1, iEndIndex - iBeginIndex - 2);

					// value
					getline(file, strText);
					iBeginIndex = (_uint)strText.find("/");
					iEndIndex = (_uint)strText.find('.');

					if (256 < iBeginIndex || 256 < iEndIndex)
					{
						Value = "none";
						return S_OK;
					}
					else
					{
						Value = strText.substr(iBeginIndex, iEndIndex - iBeginIndex);
						iBeginIndex = (_uint)Value.find_last_of("_");
						iEndIndex = (_uint)Value.size() - 1;
						Use = Value.substr(iBeginIndex + 1, iEndIndex - iBeginIndex);
					}
				
					getline(file, strText);

					// Add Texture
					if (FAILED(Add_Texture(("C:/MeshTable" + Value).c_str(), Type, Use))) {
						return E_FAIL;
					}

					break;

				case 2:
					getline(file, strText);
					break;

				default:
					break;
				}
				getline(file, strText);
			}
			++iDataIndex;
			bParent = false;
			getline(file, strText);
		}
	}

	file.close();

	return S_OK;
}

HRESULT CMaterial::Add_Texture(const _char* pTextureFolderPath, string& FileType, string& Use)
{
	ID3D11ShaderResourceView* pSRV = {};

	aiTextureType eTexture = {};
	
	EFileTypeId ID = GetFileTypeId(FileType);

	const EFileTypeId id = GetFileTypeId(FileType);

	switch (id)
	{
		// ---- Diffuse 기본 ----
	case EFileTypeId::Diffuse_Map:
	case EFileTypeId::Diffuse_Texture:
	case EFileTypeId::Diffuse_A_Map:
	case EFileTypeId::Difuse_1:
	case EFileTypeId::Base_color:
	case EFileTypeId::DiffuseTexture:
	case EFileTypeId::Distort_texture:
	case EFileTypeId::Diffuse:
	case EFileTypeId::Lamp:
	case EFileTypeId::WindowDiffuse:
	case EFileTypeId::Color_Glass:
	case EFileTypeId::Window_Surface_Diffuse:
	case EFileTypeId::InteriorDiffuse:
	case EFileTypeId::Diffuse_A:
	case EFileTypeId::Object_Diffuse:
	case EFileTypeId::Rock_Texture:
	case EFileTypeId::Base_colour:
		eTexture = aiTextureType::aiTextureType_DIFFUSE;
		break;

		// ---- Height / Cavity 계열 (SRV_Flag.x += 2) ----
	case EFileTypeId::Diffuse_Moss:
	case EFileTypeId::Moss_Diffuse:
	case EFileTypeId::Diffuse_C:
	case EFileTypeId::Cavity_D:
		eTexture = aiTextureType::aiTextureType_HEIGHT;
		m_vSRV_Flag.x += 2.f;
		break;

		// ---- Clearcoat (SRV_Flag.x += 4) ----
	case EFileTypeId::Diffuse_D:
		eTexture = aiTextureType::aiTextureType_CLEARCOAT;
		m_vSRV_Flag.x += 4.f;
		break;

		// ---- BaseColor (SRV_Flag.x += 1) ----
	case EFileTypeId::Diffuse_B:
	case EFileTypeId::Lichen_D:
	case EFileTypeId::Diffuse_B_Map:
	case EFileTypeId::TopBlend_Albedo:
	case EFileTypeId::Detail_Diffuse:
		eTexture = aiTextureType::aiTextureType_BASE_COLOR;
		m_vSRV_Flag.x += 1.f;
		break;

		// ---- Normal 기본 ----
	case EFileTypeId::Normal_Map:
	case EFileTypeId::Normal_1:
	case EFileTypeId::Object_Normal:
	case EFileTypeId::Normal_A_Map:
	case EFileTypeId::Base_Normal:
	case EFileTypeId::Object_Normal_Underbar:
	case EFileTypeId::NormalTexture:
	case EFileTypeId::Normal:
	case EFileTypeId::NormalMap:
	case EFileTypeId::Window_Surface_Normal:
	case EFileTypeId::Normal_Map_A:
	case EFileTypeId::Worn_Normal:
	case EFileTypeId::Rock_baked_Normals:
		eTexture = aiTextureType::aiTextureType_NORMALS;
		break;

		// ---- Detail / Opacity (SRV_Flag.y += 1) ----
	case EFileTypeId::Cloth_Detail_Normal:
	case EFileTypeId::Detail_Normal:
	case EFileTypeId::TopBlend_Normal:
	case EFileTypeId::Normal_Map_B:
	case EFileTypeId::Lichen_Detail_N:
	case EFileTypeId::Normal_B_Map:
	case EFileTypeId::Rock_Details:
		eTexture = aiTextureType::aiTextureType_OPACITY;
		m_vSRV_Flag.y += 1.f;
		break;

		// ---- SHININESS (SRV_Flag.y += 2) ----
	case EFileTypeId::Moss_Normal:
	case EFileTypeId::Cavity_Detail_N:
	case EFileTypeId::Moss_Normal_Map:
		eTexture = aiTextureType::aiTextureType_SHININESS;
		m_vSRV_Flag.y += 2.f;
		break;

		// ---- HRO ----
	case EFileTypeId::HRO_Map:
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.y = PBR_HRO;
		break;

		// ---- MRO 계열 (PBR_MRO / LIGHTMAP case 포함) ----
	case EFileTypeId::MRO:
	case EFileTypeId::MRO_Map:
	case EFileTypeId::WIndow_Surface_MRO:
	case EFileTypeId::PackedTexture:
	case EFileTypeId::MRO_Map_A:
	case EFileTypeId::Worn_MRO:
	case EFileTypeId::Roughness:
		eTexture = aiTextureType::aiTextureType_METALNESS;
		m_vPBR_Flag.x = PBR_MRO;
		break;

	case EFileTypeId::MRO_Map_B:
	case EFileTypeId::Detail_MRO:
		eTexture = aiTextureType::aiTextureType_LIGHTMAP;
		m_vPBR_Flag.y = PBR_MRO;
		break;

	case EFileTypeId::MROH:
		eTexture = aiTextureType::aiTextureType_METALNESS;
		m_vPBR_Flag.x = PBR_MROH;
		break;

	case EFileTypeId::MROA_Mask:
	case EFileTypeId::MROA_Map:
	case EFileTypeId::MROA:
	case EFileTypeId::Rock_MRO:
		eTexture = aiTextureType::aiTextureType_METALNESS;
		m_vPBR_Flag.x = PBR_MROA;
		break;

	case EFileTypeId::MRS:
		eTexture = aiTextureType::aiTextureType_METALNESS;
		m_vPBR_Flag.x = PBR_MRS;
		break;

		// ---- SRO 계열 ----
	case EFileTypeId::SRO_Map:
	case EFileTypeId::SRO:
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.x = PBR_SRO;
		break;

	case EFileTypeId::Moss_SRO:
		eTexture = aiTextureType::aiTextureType_NORMAL_CAMERA;
		m_vPBR_Flag.x = PBR_SRO;
		break;

	case EFileTypeId::SRO_Map_A:
		eTexture = aiTextureType::aiTextureType_METALNESS;
		m_vPBR_Flag.x = PBR_SRO;
		break;

	case EFileTypeId::SRO_Map_D:
		eTexture = aiTextureType::aiTextureType_SHEEN;
		m_vPBR_Flag.x = PBR_SRO;
		break;

	case EFileTypeId::SROH:
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.x = PBR_SROH;
		break;

	case EFileTypeId::SROA:
	case EFileTypeId::SROA_Map:
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.x = PBR_SROA;
		break;

	case EFileTypeId::SROH_A_Map:
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.x = PBR_SROH;
		break;

	case EFileTypeId::SRO_Map_B:
	case EFileTypeId::TopBlend_SRO:
	case EFileTypeId::Detail_SRO:
	case EFileTypeId::SRO_B_Map:
		eTexture = Switch_PBR_B(Use);
		m_vPBR_Flag.y = PBR_SRO;
		break;

		// ---- MRO/SRO 혼합 ----
	case EFileTypeId::MROH_SROH_Map_A:
	case EFileTypeId::Object_MRO_SRO:
	case EFileTypeId::MRO_SRO_Map_A:
	case EFileTypeId::MROH_SROH_A:
	case EFileTypeId::Base_MRO_SRO:
	case EFileTypeId::MRO_SRO:
		eTexture = Switch_PBR(Use);
		break;

	case EFileTypeId::MROH_SROH_Map_B:
	case EFileTypeId::MRO_SRO_Map_B:
	case EFileTypeId::MRO_SRO_B:
	case EFileTypeId::MROH_SROH_B:
		eTexture = Switch_PBR_B(Use);
		break;

	case EFileTypeId::Moss_MRO_SRO_Map:
		eTexture = Switch_PBR_MOSS(Use);
		break;

		// ---- RGBA / Mask / 기타 ----
	case EFileTypeId::RGBA_Mask:
		eTexture = aiTextureType::aiTextureType_MAYA_BASE;
		m_vSRV_Flag.x = 5.f;
		m_vSRV_Flag.y = 5.f;
		break;

	case EFileTypeId::Wind_Position_Map:
	case EFileTypeId::Color_Mask:
		eTexture = aiTextureType::aiTextureType_MAYA_BASE;
		break;

		// ---- Emissive ----
	case EFileTypeId::InteriorEmissive:
	case EFileTypeId::Emissive_Map:
	case EFileTypeId::Lamp_Emissive:
	case EFileTypeId::FakeExtDiffuse:
	case EFileTypeId::Diffuse_with_alpha:
	case EFileTypeId::Emissive_Texture:
		eTexture = aiTextureType::aiTextureType_EMISSIVE;
		break;

		// ---- 바로 무시(S_OK 반환) 되던 타입들 ----
	case EFileTypeId::exterior_Cube_Map:
	case EFileTypeId::Interior_Cube_Map:
	case EFileTypeId::Subsurface:
	case EFileTypeId::Mask_Texture_packed:
	case EFileTypeId::Detail_Diffuse_A:
	case EFileTypeId::Cavity_Lichen_D_Mask:
	case EFileTypeId::Detail_Diffuse_B:
	case EFileTypeId::Detail_Albedo:
	case EFileTypeId::Top_Layer_Base_Color:
	case EFileTypeId::Detail_Normal_A:
	case EFileTypeId::Detail_Normal_B:
	case EFileTypeId::Vertex_Blend_Texture:
	case EFileTypeId::Water_Color_tex:
	case EFileTypeId::emissiveMask:
	case EFileTypeId::EmissiveAlphaMask:
	case EFileTypeId::Distortion_Map:
	case EFileTypeId::Distortion_Bottom:
	case EFileTypeId::Distortion_Top:
	case EFileTypeId::Albedo:
	case EFileTypeId::Displacement:
	case EFileTypeId::Material:
	case EFileTypeId::Distortion_Pattern:
	case EFileTypeId::Ground_Diffuse:
	case EFileTypeId::Ground_MRO_SRO:
	case EFileTypeId::Corruption_Mask:
		return S_OK;

	case EFileTypeId::SRA:
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.y = PBR_SRA;
		break;

		// ---- Unknown ----
	case EFileTypeId::Unknown:
	default:
#ifndef 기무리
		MSG_BOX("Failed to Path Material Texture Type");
#endif
#ifndef gimch
		MSG_BOX("Failed to Path Material Texture Type");
#endif
		return S_OK;
	}

	_char TexturePath[MAX_PATH] = {};

	strcpy_s(TexturePath, pTextureFolderPath);
	strcat_s(TexturePath, ".dds");

	_tchar szTextureFilePath[MAX_PATH] = {};

	string strTexturePath;

	MultiByteToWideChar(CP_ACP, 0, TexturePath, (_int)strlen(TexturePath), szTextureFilePath, MAX_PATH);

	if (FAILED(CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV)))
	{
		_char TexturePath_Png[MAX_PATH] = {};

		strcpy_s(TexturePath_Png, pTextureFolderPath);
		strcat_s(TexturePath_Png, ".png");

		_tchar szTextureFilePath_Png[MAX_PATH] = {};

		MultiByteToWideChar(CP_ACP, 0, TexturePath_Png, (_int)strlen(TexturePath_Png), szTextureFilePath_Png, MAX_PATH);

		if (FAILED(CreateWICTextureFromFile(m_pDevice, szTextureFilePath_Png, nullptr, &pSRV)))
		{
#ifndef 기무리
			MSG_BOX("Failed to Load TextureFile");
#endif // !기무리
			return S_OK;
		}
		strTexturePath = CMyTools::ToString(szTextureFilePath_Png);
	}
	else
	{
		strTexturePath = CMyTools::ToString(szTextureFilePath);
	}

	m_SaveMaterial.Path[ENUM_CLASS(eTexture)].push_back(strTexturePath);
	m_SRVs[ENUM_CLASS(eTexture)].push_back(pSRV);

	return S_OK;
}
aiTextureType CMaterial::Switch_PBR(string FileType)
{
	aiTextureType eTexture = {};

	if (!strcmp(FileType.c_str(), "MRO")) {
		eTexture = aiTextureType::aiTextureType_METALNESS;
		m_vPBR_Flag.x = PBR_MRO;
	}
	else if (!strcmp(FileType.c_str(), "MROH")) {
		eTexture = aiTextureType::aiTextureType_METALNESS;
		m_vPBR_Flag.x = PBR_MROH;
	}
	else if (!strcmp(FileType.c_str(), "MROA")) {
		eTexture = aiTextureType::aiTextureType_METALNESS;
		m_vPBR_Flag.x = PBR_MROA;
	}
	else if (!strcmp(FileType.c_str(), "MRS")) {
		eTexture = aiTextureType::aiTextureType_METALNESS;
		m_vPBR_Flag.x = PBR_MRS;
	}
	else if (!strcmp(FileType.c_str(), "SROH")) {
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.x = PBR_SROH;
	}
	else if (!strcmp(FileType.c_str(), "SROA")) {
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.x = PBR_SROA;
	}
	else if (!strcmp(FileType.c_str(), "HRO")) {
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.y = PBR_HRO;
	}
	else if (!strcmp(FileType.c_str(), "HRO Map")) {
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.y = PBR_HRO;
	}
	else if (!strcmp(FileType.c_str(), "SRO")) {
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.y = PBR_SRO;
	}
	else if (!strcmp(FileType.c_str(), "MSK")) {
		eTexture = aiTextureType::aiTextureType_SPECULAR;
		m_vPBR_Flag.y = PBR_SRO;
	}
	else
		return aiTextureType::aiTextureType_NONE;

	return eTexture;
}

aiTextureType CMaterial::Switch_PBR_B(string FileType)
{
	aiTextureType eTexture = {};

	if (!strcmp(FileType.c_str(), "MRO")) {
		eTexture = aiTextureType::aiTextureType_LIGHTMAP;
		m_vPBR_Flag.y = PBR_MRO;
	}
	else if (!strcmp(FileType.c_str(), "MROH")) {
		eTexture = aiTextureType::aiTextureType_LIGHTMAP;
		m_vPBR_Flag.y = PBR_MROH;
	}
	else if (!strcmp(FileType.c_str(), "MROA")) {
		eTexture = aiTextureType::aiTextureType_LIGHTMAP;
		m_vPBR_Flag.z = PBR_MROA;
	}
	else if (!strcmp(FileType.c_str(), "MRS")) {
		eTexture = aiTextureType::aiTextureType_LIGHTMAP;
		m_vPBR_Flag.y = PBR_MRS;
	}
	else if (!strcmp(FileType.c_str(), "SROH")) {
		eTexture = aiTextureType::aiTextureType_DISPLACEMENT;
		m_vPBR_Flag.y = PBR_SROH;
	}
	else if (!strcmp(FileType.c_str(), "SROA")) {
		eTexture = aiTextureType::aiTextureType_DISPLACEMENT;
		m_vPBR_Flag.y = PBR_SROA;
	}
	else if (!strcmp(FileType.c_str(), "SRO")) {
		eTexture = aiTextureType::aiTextureType_DISPLACEMENT;
		m_vPBR_Flag.y = PBR_SRO;
	}
	else if (!strcmp(FileType.c_str(), "MSK")) {
		eTexture = aiTextureType::aiTextureType_NORMAL_CAMERA;
		m_vPBR_Flag.y = PBR_SRO;
	}
	else
		return aiTextureType::aiTextureType_NONE;

	return eTexture;
}

aiTextureType CMaterial::Switch_PBR_MOSS(string FileType)
{
	aiTextureType eTexture = {};

	if (!strcmp(FileType.c_str(), "MRO")) {
		eTexture = aiTextureType::aiTextureType_EMISSION_COLOR;
		m_vPBR_Flag.z = PBR_MRO;
	}
	else if (!strcmp(FileType.c_str(), "MROA")) {
		eTexture = aiTextureType::aiTextureType_EMISSION_COLOR;
		m_vPBR_Flag.z = PBR_MROA;
	}
	else if (!strcmp(FileType.c_str(), "MROH")) {
		eTexture = aiTextureType::aiTextureType_EMISSION_COLOR;
		m_vPBR_Flag.z = PBR_MROH;
	}
	else if (!strcmp(FileType.c_str(), "MRS")) {
		eTexture = aiTextureType::aiTextureType_EMISSION_COLOR;
		m_vPBR_Flag.z = PBR_MRS;
	}
	else if (!strcmp(FileType.c_str(), "SROH")) {
		eTexture = aiTextureType::aiTextureType_NORMAL_CAMERA;
		m_vPBR_Flag.z = PBR_SROH;
	}
	else if (!strcmp(FileType.c_str(), "HRO")) {
		eTexture = aiTextureType::aiTextureType_NORMAL_CAMERA;
		m_vPBR_Flag.z = PBR_HRO;
	}
	else if (!strcmp(FileType.c_str(), "SROA")) {
		eTexture = aiTextureType::aiTextureType_NORMAL_CAMERA;
		m_vPBR_Flag.y = PBR_SROA;
	}
	else if (!strcmp(FileType.c_str(), "SRO")) {
		eTexture = aiTextureType::aiTextureType_NORMAL_CAMERA;
		m_vPBR_Flag.y = PBR_SRO;
	}
	else if (!strcmp(FileType.c_str(), "MSK")) {
		eTexture = aiTextureType::aiTextureType_NORMAL_CAMERA;
		m_vPBR_Flag.y = PBR_SRO;
	}
	else
		return aiTextureType::aiTextureType_NONE;

	return eTexture;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, pAIMaterial)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODEL eType, const _char* pModelFilePath, const aiMaterial* pAIMaterial)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, eType, pAIMaterial)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pMaterialFilePath, const _char* pTextureFilePath)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pMaterialFilePath, pTextureFilePath)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}
#endif

HRESULT CMaterial::UnbindAllMaterialTextures(CShader* shader)
{
	//if (shader == nullptr) {
	//	return E_FAIL;
	//}

	//if (FAILED(shader->Bind_SRV("g_DiffuseTexture", nullptr))) { return E_FAIL; }
	//if (FAILED(shader->Bind_SRV("g_NormalTexture", nullptr))) { return E_FAIL; }
	//if (FAILED(shader->Bind_SRV("g_AmbientTexture", nullptr))) { return E_FAIL; }
	//if (FAILED(shader->Bind_SRV("g_EmissiveTexture", nullptr))) { return E_FAIL; }
	//if (FAILED(shader->Bind_SRV("g_AmbientOcclusionTexture", nullptr))) { return E_FAIL; }
	//if (FAILED(shader->Bind_SRV("g_TransmissionTexture", nullptr))) { return E_FAIL; }
	//if (FAILED(shader->Bind_SRV("g_SurfaceParamsTexture", nullptr))) { return E_FAIL; }
	//if (FAILED(shader->Bind_SRV("g_UnknownTexture", nullptr))) { return E_FAIL; }

	return S_OK;
}

HRESULT CMaterial::Bind_SRV(CShader* pShader, MODEL eType)
{
	if (FAILED(UnbindAllMaterialTextures(pShader))) {
		return E_FAIL;
	}
	_float fZero = 0.f;


	if (FAILED(pShader->Bind_RawValue("g_fUsingSurfaceParams", &fZero, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_SRV("g_SurfaceParamsTexture", nullptr))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vSRVFlag", &m_vSRV_Flag, sizeof(_float2)))) {
	}
	if (FAILED(pShader->Bind_RawValue("g_vPBR_Flag", &m_vPBR_Flag,  sizeof(_float3)))) {
	}
	_int iBinded[AI_TEXTURE_TYPE_MAX] = {};
	memset(iBinded, 0, sizeof(iBinded));

	for (_uint iTextureType = 0; iTextureType < AI_TEXTURE_TYPE_MAX; ++iTextureType) {
		const _char* pConstantName = nullptr;
		if (m_SRVs[iTextureType].empty()) {
			continue;
		}
		iBinded[iTextureType] = 1;

		switch (aiTextureType(iTextureType))
		{
		case aiTextureType_NONE:
			break;
		case aiTextureType_DIFFUSE:
			pConstantName = "g_DiffuseTexture";
			break;
		case aiTextureType_SPECULAR:
		{
			if (MODEL::PBR_ANIM == eType || MODEL::PBR_NONANIM == eType) {
				pConstantName = "g_SpecularTexture";
			}
			else {
				pConstantName = "g_SurfaceParamsTexture";
				if (!m_SRVs[iTextureType].empty()) {
					_float fUsingSurfaceParams = ((_float)iTextureType / (_float)AI_TEXTURE_TYPE_MAX);
					if (FAILED(pShader->Bind_RawValue("g_fUsingSurfaceParams", &fUsingSurfaceParams, sizeof(_float)))) {
						return E_FAIL;
					}
				}
			}
		} break;
		case aiTextureType_AMBIENT:
			pConstantName = "g_AmbientTexture";
			break;
		case aiTextureType_EMISSIVE:
			pConstantName = "g_EmissiveTexture";
			break;
		case aiTextureType_HEIGHT:
			pConstantName = "g_MossDiffuseTexture";
			break;
		case aiTextureType_NORMALS:
			pConstantName = "g_NormalTexture";
			break;
		case aiTextureType_SHININESS:
			pConstantName = "g_MossNormalTexture";
			break;
		case aiTextureType_OPACITY:
			pConstantName = "g_NormalBlendTexture";
			break;
		case aiTextureType_DISPLACEMENT:
			pConstantName = "g_SROBlendTexture";
			break;
		case aiTextureType_LIGHTMAP:
			pConstantName = "g_MROBlendTexture";
			break;
		case aiTextureType_REFLECTION:
			pConstantName = "g_ReflectionTexture";
			break;
		case aiTextureType_BASE_COLOR:
			pConstantName = "g_DiffuseBlend";
			break;
		case aiTextureType_NORMAL_CAMERA:
			pConstantName = "g_MossSROTexture";
			break;
		case aiTextureType_EMISSION_COLOR:
			pConstantName = "g_MossMROTexture";
			break;
		case aiTextureType_METALNESS:
		{
			if (MODEL::PBR_ANIM == eType || MODEL::PBR_NONANIM == eType) {
				pConstantName = "g_MetalnessTexture";
			}
			else {
				pConstantName = "g_SurfaceParamsTexture";
				if (!m_SRVs[iTextureType].empty()) {
					_float fUsingSurfaceParams = ((_float)iTextureType / (_float)AI_TEXTURE_TYPE_MAX);
					if (FAILED(pShader->Bind_RawValue("g_fUsingSurfaceParams", &fUsingSurfaceParams, sizeof(_float)))) {
						return E_FAIL;
					}
				}
			}
		} break;
		case aiTextureType_DIFFUSE_ROUGHNESS:
			pConstantName = "g_Diffuse_RoughnessTexture";
			break;
		case aiTextureType_AMBIENT_OCCLUSION:
			pConstantName = "g_AmbientOcclusionTexture";
			break;
		case aiTextureType_UNKNOWN:
			pConstantName = "g_UnknownTexture";
			break;
		case aiTextureType_SHEEN: // Rock SRO
			pConstantName = "g_SheenTexture";
			break;
		case aiTextureType_CLEARCOAT: // Rock_4 Diffuse
			pConstantName = "g_ClearcoadTexture";
			break;
		case aiTextureType_TRANSMISSION:
			pConstantName = "g_TransmissionTexture";
			break;
		case aiTextureType_MAYA_BASE: // Mask
			pConstantName = "g_Maya_BaseTexture";
			break;
		case aiTextureType_MAYA_SPECULAR:
			pConstantName = "g_Maya_SpecularTexture";
			break;
		case aiTextureType_MAYA_SPECULAR_COLOR:
			pConstantName = "g_Maya_Specular_ColorTexture";
			break;
		case aiTextureType_MAYA_SPECULAR_ROUGHNESS:
			pConstantName = "g_Maya_Specular_RoughnessTexture";
			break;
		case aiTextureType_ANISOTROPY:
		{
			if (MODEL::PBR_ANIM == eType || MODEL::PBR_NONANIM == eType) {
				pConstantName = "g_AnisotropyTexture";
			}
			else {
				pConstantName = "g_SurfaceParamsTexture";
				if (!m_SRVs[iTextureType].empty()) {
					_float fUsingSurfaceParams = ((_float)iTextureType / (_float)AI_TEXTURE_TYPE_MAX);
					if (FAILED(pShader->Bind_RawValue("g_fUsingSurfaceParams", &fUsingSurfaceParams, sizeof(_float)))) {
						return E_FAIL;
					}
				}
			}
		} break;
		default:
			break;
		}

		if (nullptr == pConstantName) { // 미사용 SRV
			iBinded[iTextureType] = 0;
			continue;
		}
		if (FAILED(pShader->Bind_SRV(pConstantName, m_SRVs[iTextureType][0]))) {
			return E_FAIL;
		}
	}
	
	if (FAILED(pShader->Bind_IntArray("g_iBinded_Texture", &iBinded[0], AI_TEXTURE_TYPE_MAX))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMaterial::Initialize(const _char* pModelFilePath, const SaveMaterial& _SaveMaterial, _uint iLevel)
{
	m_vSRV_Flag = _SaveMaterial.vSRV_Flag;
	m_vPBR_Flag = _SaveMaterial.vPBR_Flag;

	for (size_t type = 0; type < 27; type++)
	{
		if(!_SaveMaterial.Path[type].empty())
		{
			char szDrive[MAX_PATH] = {};
			char szDir[MAX_PATH] = {};
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szDir, MAX_PATH, nullptr, 0, nullptr, 0);

			char szFullPath[MAX_PATH] = {};
			//(szFullPath, szDrive);
			strcpy_s(szFullPath, _SaveMaterial.Path[type].front().c_str());

			_tchar szPerfectPath[MAX_PATH] = {};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, -1, szPerfectPath, MAX_PATH);

			ID3D11ShaderResourceView* pSRV = nullptr;
			HRESULT hr = S_OK;

			if (!m_SRVs[type].empty())
				continue;

			const char* ext = strrchr(szFullPath, '.');
			if (ext)
			{
				pSRV = m_pGameInstance->Add_Resource(szFullPath, iLevel);
			}

			if (nullptr == pSRV) {
				return E_FAIL;
			}
			m_SRVs[type].push_back(pSRV);
		}
	}
	return S_OK;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const SaveMaterial& _SaveMaterial, _uint iLevel)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, _SaveMaterial, iLevel)))
	{
		MSG_BOX("Failed to Created : CMaterial");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}


void CMaterial::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pDevice);
	SAFE_RELEASE(m_pContext);

	for (auto& SRVs : m_SRVs)
	{
		for (auto& pSRV : SRVs) {
			SAFE_RELEASE(pSRV);
		}
		SRVs.clear();
	}
}
