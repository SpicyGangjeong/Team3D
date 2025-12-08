#include "pch.h"
#include "Material.h"
#include "GameInstance.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_pGameInstance{ CGameInstance::GetInstance() }
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

				getline(file, strText);

				switch (iDataIndex)
				{
					/* Scala */
				case 0:
					// value
					Value = strText.substr(iBeginIndex + 2);

					getline(file, strText);
					// name
					iBeginIndex = (_uint)strText.find_first_of('=');

					Type = strText.substr(iBeginIndex + 2);
					break;

					/* Texture */
				case 1:
					// value
					iBeginIndex = (_uint)strText.find("/");
					iEndIndex = (_uint)strText.find('.');

					if (256 < iBeginIndex || 256 < iEndIndex)
					{
						Value = "none";
						return S_OK;
					}
					else
						Value = strText.substr(iBeginIndex, iEndIndex - iBeginIndex);
					
					// name
					iBeginIndex = (_uint)strText.find_last_of('_');
					iEndIndex = (_uint)strText.rfind('\'');

					Type = strText.substr(iBeginIndex + 1, iEndIndex - iBeginIndex - 1);


					getline(file, strText);

					// Add Texture
					if (FAILED(Add_Texture(("C:/MeshTable" + Value).c_str(), Type))) {
						return E_FAIL;
					}

					break;

				case 2:

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
HRESULT CMaterial::Add_Texture(const _char* pTextureFolderPath, string& FileType)
{
	ID3D11ShaderResourceView* pSRV = {};

	aiTextureType eTexture = {};
	
	/* Find type */
	if (!strcmp(FileType.c_str(), "D")){
		eTexture = aiTextureType::aiTextureType_DIFFUSE;
	}
	else if (!strcmp(FileType.c_str(), "Diff")){
		eTexture = aiTextureType::aiTextureType_DIFFUSE;
	}
	else if (!strcmp(FileType.c_str(), "d")){
		eTexture = aiTextureType::aiTextureType_DIFFUSE;
	}
	else if (!strcmp(FileType.c_str(), "basecolor")) {
		eTexture = aiTextureType::aiTextureType_DIFFUSE;
	}
	else if (!strcmp(FileType.c_str(), "N")){
		eTexture = aiTextureType::aiTextureType_NORMALS;
	}
	else if (!strcmp(FileType.c_str(), "normal")){
		eTexture = aiTextureType::aiTextureType_NORMALS;
	}
	else if (!strcmp(FileType.c_str(), "Norm")){
		eTexture = aiTextureType::aiTextureType_NORMALS;
	}
	else if (!strcmp(FileType.c_str(), "n")){
		eTexture = aiTextureType::aiTextureType_NORMALS;
	}
	else if (!strcmp(FileType.c_str(), "MRO")){
		eTexture = aiTextureType::aiTextureType_METALNESS;
	}
	else if (!strcmp(FileType.c_str(), "MROH")){
		eTexture = aiTextureType::aiTextureType_METALNESS;
	}
	else if (!strcmp(FileType.c_str(), "MRAB")){
		eTexture = aiTextureType::aiTextureType_METALNESS;
	}
	else if (!strcmp(FileType.c_str(), "MROA")){
		eTexture = aiTextureType::aiTextureType_METALNESS;
	}
	else if (!strcmp(FileType.c_str(), "SRO")){
		eTexture = aiTextureType::aiTextureType_SPECULAR;
	}
	else if (!strcmp(FileType.c_str(), "HRO")){
		eTexture = aiTextureType::aiTextureType_SPECULAR;
	}
	else if (!strcmp(FileType.c_str(), "SROH")) {
		eTexture = aiTextureType::aiTextureType_SPECULAR;
	}
	else if (!strcmp(FileType.c_str(), "SROA")){
		eTexture = aiTextureType::aiTextureType_SPECULAR;
	}
	else if (!strcmp(FileType.c_str(), "3Broom")) {
		eTexture = aiTextureType::aiTextureType_UNKNOWN;
	}
	else if (!strcmp(FileType.c_str(), "HDR"))
		return S_OK;
	else if (!strcmp(FileType.c_str(), "MSK")){
		eTexture = aiTextureType::aiTextureType_AMBIENT_OCCLUSION;
	}
	else if (!strcmp(FileType.c_str(), "E")){
		eTexture = aiTextureType::aiTextureType_EMISSIVE;
	}
	else if (!strcmp(FileType.c_str(), "SRXO")) {
		eTexture = aiTextureType::aiTextureType_SPECULAR;
	}
	else if (!strcmp(FileType.c_str(), "A")) {
		eTexture = aiTextureType::aiTextureType_AMBIENT;
	}
	else
	{
#ifndef 기무리
		MSG_BOX("Failed to Path Material Texture Type");
#endif // !기무리
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

HRESULT CMaterial::Bind_SRV(CShader* pShader)
{
	//if (m_SRVs[iType].empty()) {
	//	//assert(false);
	//	return S_OK;
	//}

	_float fZero = 0.f;
	if (FAILED(pShader->Bind_RawValue("g_fUsingSurfaceParams", &fZero, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_SRV("g_SurfaceParamsTexture", nullptr))) {
		return E_FAIL;
	}
	_bool bBinded[AI_TEXTURE_TYPE_MAX] = {};
	for (_uint iTextureType = 0; iTextureType < AI_TEXTURE_TYPE_MAX; ++iTextureType) {
		const _char* pConstantName = "";
		bBinded[iTextureType] = true;
		if (m_SRVs[iTextureType].empty()) {
			bBinded[iTextureType] = false;
		}
		else {
			switch (aiTextureType(iTextureType))
			{
			case aiTextureType_NONE:
				break;
			case aiTextureType_DIFFUSE:
				pConstantName = "g_DiffuseTexture";
				break;
			case aiTextureType_SPECULAR:
			{
				_float fUsingSurfaceParams = ((_float)iTextureType / (_float)AI_TEXTURE_TYPE_MAX);
				pConstantName = "g_SurfaceParamsTexture";
				if (!m_SRVs[iTextureType].empty()) {
					pShader->Bind_RawValue("g_fUsingSurfaceParams", &fUsingSurfaceParams, sizeof(_float));
				}
			} break;
			case aiTextureType_AMBIENT:
				pConstantName = "g_AmbientTexture";
				break;
			case aiTextureType_EMISSIVE:
				pConstantName = "g_EmissiveTexture";
				break;
			case aiTextureType_HEIGHT:
				break;
			case aiTextureType_NORMALS:
				pConstantName = "g_NormalTexture";
				break;
			case aiTextureType_SHININESS:
				break;
			case aiTextureType_OPACITY:
				break;
			case aiTextureType_DISPLACEMENT:
				break;
			case aiTextureType_LIGHTMAP:
				break;
			case aiTextureType_REFLECTION:
				break;
			case aiTextureType_BASE_COLOR:
				break;
			case aiTextureType_NORMAL_CAMERA:
				break;
			case aiTextureType_EMISSION_COLOR:
				break;
			case aiTextureType_METALNESS:
			{
				_float fUsingSurfaceParams = ((_float)iTextureType / (_float)AI_TEXTURE_TYPE_MAX);
				pConstantName = "g_SurfaceParamsTexture";
				if (!m_SRVs[iTextureType].empty()) {
					pShader->Bind_RawValue("g_fUsingSurfaceParams", &fUsingSurfaceParams, sizeof(_float));
				}
			} break;
			case aiTextureType_DIFFUSE_ROUGHNESS:
				break;
			case aiTextureType_AMBIENT_OCCLUSION:
				pConstantName = "g_AmbientOcclusionTexture";
				break;
			case aiTextureType_UNKNOWN:
				pConstantName = "g_UnknownTexture";
				break;
			case aiTextureType_SHEEN:
				break;
			case aiTextureType_CLEARCOAT:
				break;
			case aiTextureType_TRANSMISSION:
				pConstantName = "g_TransmissionTexture";
				break;
			case aiTextureType_MAYA_BASE:
				break;
			case aiTextureType_MAYA_SPECULAR:
				break;
			case aiTextureType_MAYA_SPECULAR_COLOR:
				break;
			case aiTextureType_MAYA_SPECULAR_ROUGHNESS:
				break;
			case aiTextureType_ANISOTROPY:
			{
				_float fUsingSurfaceParams = ((_float)iTextureType / (_float)AI_TEXTURE_TYPE_MAX);
				pConstantName = "g_SurfaceParamsTexture";
				if (!m_SRVs[iTextureType].empty()) {
					pShader->Bind_RawValue("g_fUsingSurfaceParams", &fUsingSurfaceParams, sizeof(_float));
				}
			} break;
			case _aiTextureType_Force32Bit:
				break;
			default:
				break;
			}
		}
		if (true == bBinded[iTextureType]) {
			if (FAILED(pShader->Bind_SRV(pConstantName, m_SRVs[iTextureType][0]))) {
				return E_FAIL;
			}
		}
	}
	if (FAILED(pShader->Bind_RawValue("g_bBinded_Texture", &bBinded, sizeof(bBinded)))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CMaterial::Initialize(const _char* pModelFilePath, const SaveMaterial& _SaveMaterial)
{
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
				pSRV = m_pGameInstance->Add_Resource(szFullPath);
			}

			if (nullptr == pSRV) {
				return E_FAIL;
			}
			m_SRVs[type].push_back(pSRV);
		}
	}
	return S_OK;
}

CMaterial* CMaterial::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const SaveMaterial& _SaveMaterial)
{
	CMaterial* pInstance = new CMaterial(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pModelFilePath, _SaveMaterial)))
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
