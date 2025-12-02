#include "pch.h"
#include "Effect_Editor.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "TrailObject.h"
#include "EffectPool.h"
#include "Layer.h"
#include "Player.h"
#include "Wand.h"

#include <sstream>

CEffect_Editor::CEffect_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect_Container{ pDevice, pContext }
{
}

CEffect_Editor::CEffect_Editor(const CEffect_Editor& rhs)
	: CEffect_Container(rhs)
	, m_MatFiles(rhs.m_MatFiles)
{

}

HRESULT CEffect_Editor::Initialize_Prototype()
{

	ReadMaterials("../Bin/Resources/VFX/Particles/Magic/Levioso");


	for (auto iter : m_MatFiles)
	{

		for (auto vec_iter : iter.second)
		{
			if (FAILED(m_pGameInstance->Add_Asset_Prototype(ENUM_CLASS(LEVEL::EFFECT), vec_iter.first,
				CTexture::Create(m_pDevice, m_pContext, TEXTURE_LOAD_TYPE::SINGLE, vec_iter.second.c_str(), 0, vec_iter.first)))) {
				continue;
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


	m_pEffectPool = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_EffectPool"))->Get_Object<CEffectPool>();
	SAFE_ADDREF(m_pEffectPool);

	return S_OK;
}

void CEffect_Editor::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CEffect_Editor::Update(_float fTimeDelta)
{
	Describe_Entity();
	__super::Update(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_UP))
		m_pTransformCom->Go_Straight(fTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_DOWN))
		m_pTransformCom->Go_Backward(fTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
		m_pTransformCom->Go_Right(fTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
		m_pTransformCom->Go_Left(fTimeDelta);

	if (m_pGameInstance->Key_Pressing(DIK_Z))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);

	/* 이펙트 컨테이너 업데이트 */
	//Update_Event(fTimeDelta);

	/* 트레일 업데이트 */

	if (m_isWandPos == true)
	{
		if (m_pTrailObject != nullptr)
		{
			_matrix vWandPos = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>()->Get_PartObject<CWand>()->Get_WorldMatrix();

			m_pTrailObject->Trail_Update(vWandPos, fTimeDelta);
		}


	}
	else
	{
		if (m_pTrailObject != nullptr)
			m_pTrailObject->Trail_Update(m_pTransformCom->Get_XMWorldMatrix(), fTimeDelta);
	}



}

void CEffect_Editor::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CEffect_Editor::Ready_Components(void* pArg)
{
	CTransform::TRANSFORM_DESC TransformDesc = {};

	TransformDesc.fRadius = 20.f;
	TransformDesc.fRotationPerSec = 10.f;
	TransformDesc.fSpeedPerSec = 10.f;

	if (FAILED(__super::Ready_Components(&TransformDesc))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Editor::Ready_Child()
{


	//CPartObject::PARTOBJECT_DESC PartsDesc{};

	//PartsDesc.pParentTransform = m_pTransformCom;

	//if (FAILED(Add_PartObject<CEditEffect>("EffectObject" + to_string(m_iNumPart++), ENUM_CLASS(LEVEL::EFFECT), nullptr, &PartsDesc)))
	//	return E_FAIL;

	//m_strCurrentEffectName = "EffectObject0" ;




	return S_OK;
}

HRESULT CEffect_Editor::Packaging(const _char* pDirectoryPath)
{
	_uint iIndex = {};

	_string strPerfectFilePath = "../Bin/Resources/Data/Effect/Package/";
	_string strDirectoryPath = pDirectoryPath;

	size_t pos = strDirectoryPath.rfind('/');   // 뒤에서부터 '/' 검색

	if (pos != std::string::npos)
		strPerfectFilePath += strDirectoryPath.substr(pos + 1);

	strPerfectFilePath += ".bin";


	HANDLE	hFile = CreateFile(CMyTools::ToWstring(strPerfectFilePath).c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL
	);


	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"패키징 실패", L"System Message", MB_OK);
		return E_FAIL;
	}
	DWORD dwByte;
	_int iFileCount = {};

	for (auto& file : filesystem::directory_iterator(pDirectoryPath)) {
			++iFileCount;
	}

	if (!WriteFile(hFile, &iFileCount, sizeof(_int), &dwByte, nullptr)) {
		CloseHandle(hFile);
		return E_FAIL;
	}

	for (const auto& file : filesystem::directory_iterator(pDirectoryPath))
	{
		if (file.is_directory())
			continue;

		string ext = file.path().extension().string();

		if (!(strcmp(ext.c_str(), ".bin") ^ strcmp(ext.c_str(), ".trail")))
			continue;

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		/////////////////////////////

		_string strPath = szFilePath;

	

		size_t iComponentLength = strPath.length();
		const _char* pFileComponentPath = strPath.c_str();

		if (!WriteFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			if (!WriteFile(hFile, pFileComponentPath, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}
		}

	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEffect_Editor::ReSaveFile(const _char* pDirectoryPath)
{
	_uint iIndex = {};

	for (const auto& file : filesystem::directory_iterator(pDirectoryPath))
	{
		if (file.is_directory())
			continue;

		string ext = file.path().extension().string();

		if (strcmp(ext.c_str(),".bin"))
			continue;

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());


		/////////////////////////////

		CEditEffect* pEditEffect = nullptr;

		CPartObject::PARTOBJECT_DESC PartsDesc{};

		PartsDesc.pParentTransform = m_pTransformCom;

		

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEditEffect>(ENUM_CLASS(LEVEL::EFFECT), CURRENT_LEVEL, TEXT("Layer_Effect"), &PartsDesc, this , &pEditEffect))) {
			assert(false);
			return E_FAIL;
		}
		
		if (FAILED(pEditEffect->LoadPre(szFilePath, LEVEL::EFFECT)))
		{
			Safe_Release(pEditEffect);
			continue;
		}

		_string strPath = szFilePath;


		size_t iPos;

		while ((iPos = strPath.find(ext)) != std::string::npos) {
			strPath.erase(iPos, ext.length());
		};

		pEditEffect->Save_Effect(strPath.c_str());

		pEditEffect->Set_Dead();
	}

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

					if (isExist == false)
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

				if (isExist == false)
					iter->second.push_back(pair(wstrTextureName, wstrResult));
			}

		}
	}

	wifs.close();
}

_string CEffect_Editor::Reference_Mat_For_EditEffect(CComponent** pTexture, CGameObject* pObject)
{

	_string strName = {};
	for (auto iter : m_MatFiles)
	{
		if (GUI::TreeNode(CMyTools::ToString(iter.first).c_str()))
		{

			for (auto wstrTextureInfo : iter.second)
			{
				strName = m_pGameInstance->Asset_Description<CTexture>(
					ENUM_CLASS(LEVEL::EFFECT),
					CMyTools::ToString(iter.first).c_str()
					, pTexture
					, nullptr
					, pObject
					, wstrTextureInfo.first // 이름
				);
				
				if (strName != "")
				{
					GUI::TreePop();
					return strName;
				}

			};


			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}


	return "";
}

HRESULT CEffect_Editor::Load_Edit(const _char* pPath)
{

	if (m_pEditEffect == nullptr)
		return E_FAIL;
	

	if (FAILED(m_pEditEffect->Load(pPath, LEVEL::EFFECT)))
	{
		Safe_Release(m_pEditEffect);
		return E_FAIL;
	}
	else
		MessageBox(NULL, L"이펙트 로드 성공", L"System Message", MB_OK);

	Safe_Release(m_pEditEffect);

	return S_OK;
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

	SAFE_RELEASE(m_pEditEffect);
	SAFE_RELEASE(m_pTrailObject);
	SAFE_RELEASE(m_pEffectPool);
}

void CEffect_Editor::Describe_Entity()
{
	ImGui::Begin("Effect Editor");

	GUI::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), m_strCurrentEffectName.c_str());

	if (m_pEditEffect != nullptr)
	{
		m_pEditEffect->Describe_Entity();
	}

	if (m_pTrailObject != nullptr)
	{
		m_pTrailObject->Describe_Entity();
		GUI::Checkbox("WAND POS", &m_isWandPos);
	}

	ImGui::End();

	ImGui::Begin("Reference Material");

	if (m_pEditEffect != nullptr)
		m_pEditEffect->Reference_Mat_For_EditEffect();

	ImGui::End();

	ImGui::Begin("Effect Hierarchy");

#pragma region PopUp
	
	if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		ImGui::OpenPopup("Effect Hierarchy");


	if (ImGui::BeginPopup("Effect Hierarchy"))
	{
		if (ImGui::MenuItem("Create Effect"))
		{

			CPartObject::PARTOBJECT_DESC PartsDesc{};

			PartsDesc.pParentTransform = m_pTransformCom;

			if (FAILED(Add_PartObject<CEditEffect>("EffectObject" + to_string(m_iNumPart++), ENUM_CLASS(LEVEL::EFFECT), nullptr, &PartsDesc)))
			{
				ImGui::EndPopup();
				return;
			}
		}

		if (ImGui::MenuItem("Create Trail"))
		{

			if (m_pTrailObject == nullptr)
			{

				CPartObject::PARTOBJECT_DESC PartsDesc{};

				PartsDesc.pParentTransform = m_pTransformCom;

				if (FAILED(Add_PartObject<CTrailObject>("TrailObject", ENUM_CLASS(LEVEL::EFFECT), &m_pTrailObject, &PartsDesc)))
				{
					ImGui::EndPopup();
				}

				CTrailObject* pTrailObject = m_pTrailObject;

				SAFE_RELEASE(pTrailObject);

			}
		}

		ImGui::EndPopup();
	}
#pragma endregion


	_int iIndex = {};

	for (auto& pPartObject : m_PartObjects)
	{
		_string strName = {};

		if( nullptr != dynamic_cast<CTrailObject*>(pPartObject.second))
			strName = "TrailObject" + to_string(iIndex);
		else
			strName = pPartObject.first;

		if (GUI::TreeNode(strName.c_str()))
		{

#pragma region PopUp

			if (GUI::IsItemClicked(ImGuiMouseButton_Right))
				GUI::OpenPopup(strName.c_str());


			if (GUI::BeginPopup(strName.c_str()))
			{
	
				if (ImGui::MenuItem("Delete Effect"))
				{
					auto iter = m_PartObjects.begin();
					advance(iter, iIndex); // 이터레이터를 증가시킨다

					SAFE_RELEASE(pPartObject.second);
					m_PartObjects.erase(iter);

					GUI::EndPopup();
					GUI::TreePop();
					break;
				}


				GUI::EndPopup();
			}

#pragma endregion
		
			if (GUI::IsItemClicked()) // 선택창에 띄우기
			{
				CEditEffect* pSelectedEffectObject = dynamic_cast<CEditEffect*>(pPartObject.second);

				if (pSelectedEffectObject == nullptr)
				{
					GUI::TreePop();
					continue;
		
				}
				

				if (m_pEditEffect == pSelectedEffectObject)
				{
					GUI::TreePop();
					continue;
		
				}
			


				SAFE_RELEASE(m_pEditEffect);
				
				m_pEditEffect = pSelectedEffectObject;

				SAFE_ADDREF(pSelectedEffectObject);

				m_strCurrentEffectName = strName;

			}

			GUI::TreePop();
		}

		iIndex++;
	}



	ImGui::End();

	ImGui::Begin("Effect Save");

	GUI::InputTextMultiline("FILE PATH", m_szBuffer, sizeof(m_szBuffer), ImVec2(250, 25));

	m_strSavePath = m_szBuffer;

	if (GUI::Button("SAVE"))
	{
		m_pEditEffect->Save_Effect(m_strSavePath.c_str());
	}

	GUI::SameLine();

	if (GUI::Button("LOAD"))
	{
		Load_Edit(m_strSavePath.c_str());
	}

	GUI::SameLine();

	if (GUI::Button("RESAVE"))
	{
		ReSaveFile(m_strSavePath.c_str());
	}

	GUI::InputTextMultiline("DIRECTORY PATH", m_szPackageBuffer, sizeof(m_szPackageBuffer), ImVec2(250, 25));

	m_strPackageSavePath = m_szPackageBuffer;

	if (GUI::Button("LOAD DIRECTORY"))
	{
		if (SUCCEEDED(Load_Directory(m_strPackageSavePath.c_str())))
		{
			MessageBox(NULL, L"파일 로드 성공", L"System Message", MB_OK);
		}
	}

	GUI::SameLine();

	if (GUI::Button("PACKAGE DIRECTORY"))
	{
		if (SUCCEEDED(Packaging(m_strPackageSavePath.c_str())))
		{
			MessageBox(NULL, L"패키징 성공", L"System Message", MB_OK);
		}
		else
		{
			MessageBox(NULL, L"패키징 실패", L"System Message", MB_OK);
		}
	}

	GUI::SameLine();

	if (GUI::Button("LOAD PACKAHE"))
	{
		if(SUCCEEDED(Load_Package(m_strPackageSavePath.c_str())))
		{
			MessageBox(NULL, L"로드 성공", L"System Message", MB_OK);
		}
		else
		{
			MessageBox(NULL, L"로드 실패", L"System Message", MB_OK);
		}

	}

	GUI::InputTextMultiline("TRAIL FILE PATH", m_szTrailBuffer, sizeof(m_szTrailBuffer), ImVec2(250, 25));

	m_strTrailSavePath = m_szTrailBuffer;

	if (GUI::Button("TRAIL SAVE"))
	{
		if (m_pTrailObject != nullptr)
		{
			m_pTrailObject->Save_Trail(m_strTrailSavePath.c_str());
		}
	}

	GUI::SameLine();

	if (GUI::Button("TRAIL LOAD"))
	{
		if (m_pTrailObject != nullptr)
		{
			m_pTrailObject->Load_Trail(m_strTrailSavePath.c_str(), LEVEL::EFFECT);
		}
	}




	m_pEffectPool->Describe_Entity();

	ImGui::End();

}

HRESULT CEffect_Editor::Bind_ShaderResources()
{
	return S_OK;
}
