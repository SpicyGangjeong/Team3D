#include "pch.h"
#include "Effect_Editor.h"

#include "GameInstance.h"
#include "EditEffect.h"


#include <sstream>

CEffect_Editor::CEffect_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CEffect_Editor::CEffect_Editor(const CEffect_Editor& rhs)
	: CEffectObject(rhs)
	, m_MatFiles(rhs.m_MatFiles)
{

}

HRESULT CEffect_Editor::Initialize_Prototype()
{


	ReadMaterials("../Bin/Resources/VFX/ParticleMaterials/Base_Mat");
	ReadMaterials("../Bin/Resources/VFX/ParticleMaterials/Mat_Instance");

	for (auto iter : m_MatFiles)
	{

		for (auto vec_iter : iter.second)
		{
			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), vec_iter.first,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, vec_iter.second.c_str(), 0, vec_iter.first)))) {
				return E_FAIL;
			}
		}

	}
	return S_OK;

}

HRESULT CEffect_Editor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;



	return S_OK;
}

void CEffect_Editor::Priority_Update(_float fTimeDelta)
{

}

void CEffect_Editor::Update(_float fTimeDelta)
{
	Describe_Entity();
}

void CEffect_Editor::Late_Update(_float fTimeDelta)
{

}

HRESULT CEffect_Editor::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Editor::Ready_Child()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEditEffect>(ENUM_CLASS(LEVEL::EFFECT), NEXT_LEVEL, LAYER_EFFECT , nullptr ,this , reinterpret_cast<CEditEffect**>(&m_pEditEffect))))
		return E_FAIL;

	return S_OK;
}

void CEffect_Editor::ReadMaterials(const char* pDirectoryName)
{
	for (const auto& file : filesystem::directory_iterator(pDirectoryName))
	{
		if (file.is_directory())
			continue;

		_string ext = file.path().extension().string();

		if (strcmp(ext.c_str(), ".txt")) // 텍스트 파일 일때만 빌드함
			continue; 

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		_wstring wstrFileName = file.path().stem().wstring(); // 파일명


		ReadMaterial(wstrFileName, szFilePath);


	}
	
}

void CEffect_Editor::ReadMaterial(_wstring wstrFileName, const char* pFilePath)
{
	m_MatFiles.emplace(wstrFileName, vector<pair<_wstring, _wstring>>());

	wifstream  wifs(pFilePath);

	_wstring wstrFirstLine = {};

	getline(wifs, wstrFirstLine); // 가장 위의 텍스트

	//.기준으로 뒤에 이름을 지워야함
	size_t iPos = wstrFirstLine.find(L" ");

	_wstring wstrParent = {};

	wstrParent = wstrFirstLine.substr(0, iPos); // " " 이전만 남김

	if (wstrParent == L"Parent") // 부모 머테리얼이 있는 객체였다면
	{
		_wstring wstrMatPath = wstrFirstLine;

		_wstring wstrKeyword = L"VFX";

		size_t iPos = wstrMatPath.find(wstrKeyword); // VFX를 찾고 없을 시에 로드하지 않는다

		if (iPos != _wstring::npos) {

			_wstring wstrResult = wstrMatPath.substr(iPos + wstrKeyword.length()); // "VTX이후만 남김"
			_wstring wstrMatName = {};


			//.기준으로 뒤에 이름을 지워야함
			size_t iPos = wstrResult.find(L".");

			wstrResult = wstrResult.substr(0, iPos); // ".이전만 남김"

			//파일이름 받아오기
			size_t iNamePos = wstrResult.rfind(L"/");
			wstrMatName = wstrResult.substr(iNamePos + 1);

			wstrResult = L"../Bin/Resources/VFX" + wstrResult + L".props.txt";

			ReadMaterial(wstrMatName, CMyTools::ToString(wstrResult).c_str());


			// 상위 머테리얼 파일에서 넣었던 파일을 다시 내 파일로 담는다
			auto Superior_iter = m_MatFiles.find(wstrMatName);
			auto Recent_iter = m_MatFiles.find(wstrFileName);


			if (m_MatFiles.end() != Superior_iter) // 경로가 존재한다면
			{
				for (auto& wstrPath : Superior_iter->second) // 상위 머테리얼 벡터를 순회하여 내 벡터에 넣자
				{
					_bool isExist = { false };

					for (auto TextureInfo : Recent_iter->second) //이미 존재한 경로였다면 추가하지 않는다
					{
						
						if (TextureInfo.first == wstrPath.first)
						{
							isExist = true;
							break;
						}
							
					}

					if(isExist == false)
						Recent_iter->second.push_back(wstrPath);
				}


				m_MatFiles.erase(Superior_iter);
			}

			//벡터에 모두 넣었다면 상위 머테리얼을 지운다

		}

	}

	// 텍스쳐를 불러오자 
	_wstring wstrTextureLine = {};

	while (getline(wifs, wstrTextureLine))
	{

		_wstring wstrKeyword = L"/VFX";

		size_t iPos = wstrTextureLine.find(wstrKeyword); // VFX를 찾고 없을 시에 로드하지 않는다

		if (iPos != _wstring::npos) {

			_wstring wstrResult = wstrTextureLine.substr(iPos); // "VTX이후만 남김"
			_wstring wstrTextureName = {};

			//.기준으로 뒤에 이름을 지워야함
			size_t iPos = wstrResult.find(L".");

			wstrResult = wstrResult.substr(0, iPos); // ".이전만 남김"

			// 이름 만들기
			size_t iNamePos = wstrResult.rfind(L"/");
			wstrTextureName = wstrResult.substr(iNamePos + 1);

			wstrResult = L"../Bin/Resources" + wstrResult + L".png";

			//경로를 담는다
			auto iter = m_MatFiles.find(wstrFileName);

			if (m_MatFiles.end() != iter) // 경로가 존재한다면
			{
				_bool isExist = { false };

				for (auto TextureInfo : iter->second)
				{
					if (TextureInfo.first == wstrTextureName) // 이미 존재했었다면
					{
						isExist = true;
						break;
					}	
				}

				if(isExist == false)
					iter->second.push_back(pair(wstrTextureName, wstrResult));
			}

		}
	}

	wifs.close();
}

void CEffect_Editor::Reference_Mat_For_EditEffect(CComponent** pTexture, CGameObject* pObject)
{

	for (auto iter : m_MatFiles)
	{
		if (GUI::TreeNode(CMyTools::ToString(iter.first).c_str()))
		{

			for (auto wstrTextureInfo : iter.second)
			{
				m_pGameInstance->Asset_Description<CTexture>(
					ENUM_CLASS(LEVEL::EFFECT),
					CMyTools::ToString(iter.first).c_str()
					, pTexture
					, nullptr
					, pObject
					, wstrTextureInfo.first // 이름
				);
			};


			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}



}


CEffect_Editor* CEffect_Editor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Editor* pInstance = new CEffect_Editor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Editor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Editor::Clone(void* pArg, CGameObject* pOwner)
{
	CEffect_Editor* pInstance = new CEffect_Editor(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Editor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEffect_Editor::Free()
{
	__super::Free();
}

void CEffect_Editor::Describe_Entity()
{
	ImGui::Begin("Effect Editor");


	if (m_pEditEffect != nullptr)
		m_pEditEffect->Describe_Entity();

	ImGui::End();


}
