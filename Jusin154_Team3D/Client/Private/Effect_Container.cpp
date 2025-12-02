#include "pch.h"
#include "Effect_Container.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "TrailObject.h"

#include "PhysXEffectHitBox.h"

#include <sstream>

CEffect_Container::CEffect_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CEffect_Container::CEffect_Container(const CEffect_Container& rhs)
	: CContainerObject(rhs)
{

}

HRESULT CEffect_Container::Initialize_Prototype()
{

	return S_OK;

}

HRESULT CEffect_Container::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;

	Set_Visible(false);

	return S_OK;
}

void CEffect_Container::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CEffect_Container::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CEffect_Container::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

void CEffect_Container::OnCollision(CGameObject* pOther, void* pDesc)
{
}

HRESULT CEffect_Container::Load_Directory(const _char* pPath)
{

	for (const auto& file : filesystem::directory_iterator(pPath))
	{
		if (file.is_directory())
			continue;

		string ext = file.path().extension().string();

		if (strcmp(ext.c_str(), ".bin"))
			continue;

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());


		/////////////////////////////

		CEffectParts* pEffectParts = nullptr;

		CPartObject::PARTOBJECT_DESC PartsDesc{};

		PartsDesc.pParentTransform = m_pTransformCom;




		/*이펙트 껍데기 생성*/

		_wstring wstrFileName = file.path().stem().wstring(); //이름 넣기

		if (FAILED(Add_PartObject<CEffectParts>(CMyTools::ToString(wstrFileName), g_iStaticLevel, &pEffectParts, &PartsDesc))) {
			assert(false);
			return E_FAIL;
		}
		

		/*이펙트 로드 로드*/
		size_t iPos = {};
		_string strFilePath = szFilePath;

		while ((iPos = strFilePath.find(".bin")) != std::string::npos) {
			strFilePath.erase(iPos, 4);
		};


		if (FAILED(pEffectParts->Load(strFilePath.c_str(),static_cast<LEVEL>(NEXT_LEVEL))))
		{
			Safe_Release(pEffectParts);
			continue;
		}

		Safe_Release(pEffectParts);
	}

	return S_OK;
}

HRESULT CEffect_Container::Ready_Components(void* pArg)
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

HRESULT CEffect_Container::Pre_Setting(CGameObject* pObject)
{
	return S_OK;
}

HRESULT CEffect_Container::Reset_EffectParts()
{
	for (auto& iter : m_PartObjects)
	{
		CInstance_Model* pInstanceModel = iter.second->Get_Component<CInstance_Model>();

		if (pInstanceModel == nullptr){
			continue;
		}

		pInstanceModel->Instane_Buffer_ReStruct();
	}

	return S_OK;
}


HRESULT CEffect_Container::Ready_Child()
{

	return S_OK;
}

void CEffect_Container::Free()
{
	__super::Free();

}

HRESULT CEffect_Container::Bind_ShaderResources()
{
	return S_OK;
}

void CEffect_Container::Update_Event(_float fTimeDelta)
{


	m_fPreAccTime = m_fAccTime;
	m_fAccTime += fTimeDelta;

	if (m_fAccTime >= m_fDelay || m_isDelayed == false) // 아직 딜레이되지 않았고 딜레이를 넘어섰다면
	{
		m_isDelayed = true;
	}
	else
	{
		return;
	}


	if (m_fAccTime >= m_fDuration) // 듀레이션을 넘어섰다면
	{
		if (m_isLoop)
		{
			m_fAccTime = 0.f;
			m_fPreAccTime = 0.f;
			m_isDelayed = false;
		}
		else
		{
			m_bVisible = false;

			for (auto& pPart : m_PartObjects)
			{
				pPart.second->Set_Visible(false);
			}

			if (m_pPhysHitBox != nullptr && m_pPhysHitBox->isDead() == false)
			{
				m_pPhysHitBox->Set_Dead();
				SAFE_RELEASE(m_pPhysHitBox);
			}
		}
	}


	for (auto& pPair : m_Events) // 이벤트들을 비교하면서 실행함
	{
		if (pPair.first >= m_fPreAccTime && pPair.first <= m_fAccTime)
		{
			pPair.second();
		}
	}
	

}
HRESULT CEffect_Container::Load_Package(const _char* pPath)
{

	_string strPerfectFilePath = pPath;

	strPerfectFilePath += ".bin";

	HANDLE hFile = CreateFileW(
		CMyTools::ToWstring(strPerfectFilePath).c_str(),               // 파일 이름
		GENERIC_READ,              // 읽기 모드
		FILE_SHARE_READ,           // 다른 프로세스도 읽기 가능
		NULL,
		OPEN_EXISTING,             // 기존 파일 열기
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);


	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"오브젝트 읽기 실패", L"System Message", MB_OK);
		return E_FAIL;
	}

	// 나중에 패키징할때 패스를 저장할거임


	DWORD	dwByte(0);

	_int iEffectCount = {};

	if (!ReadFile(hFile, &iEffectCount, sizeof(_int), &dwByte, nullptr)) {
		CloseHandle(hFile);
		return E_FAIL;
	}


	for (_int i = 0; i < iEffectCount; i++)
	{
		size_t iComponentLength = {};
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			if (!ReadFile(hFile, szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}
		}

		_string strEffectPath = szName;
		_string strExt = {};
		_string strEffectName = {};

		size_t iPos = strEffectPath.rfind('.');   // 뒤에서부터 '/' 검색

		if (iPos != std::string::npos)
		{
			strExt += strEffectPath.substr(iPos + 1);
			strEffectName = strEffectPath.substr(0, iPos);
		}

		size_t pos = strEffectName.rfind('\\');   // 뒤에서부터 '/' 검색

		if (pos != std::string::npos)
			strEffectName = strEffectName.substr(pos + 1);

		if (!strcmp(strExt.c_str(), "trail"))
		{
			CTrailObject* pTrail = nullptr;

			CPartObject::PARTOBJECT_DESC PartsDesc{};

			PartsDesc.pParentTransform = m_pTransformCom;

			if (FAILED(Add_PartObject<CTrailObject>(strEffectName, g_iStaticLevel, &pTrail, &PartsDesc))) {
				assert(false);
				return E_FAIL;
			}


			/*이펙트 로드 로드*/
			size_t iPos = {};

			while ((iPos = strEffectPath.find(".trail")) != std::string::npos) {
				strEffectPath.erase(iPos, 6);
			};


			if (FAILED(pTrail->Load_Trail(strEffectPath.c_str(), static_cast<LEVEL>(NEXT_LEVEL))))
			{
				Safe_Release(pTrail);
				continue;
			}

			Safe_Release(pTrail);

			continue;
		}


		/////////////////////////////

		CEffectParts* pEffectPart = nullptr;

		CPartObject::PARTOBJECT_DESC PartsDesc{};

		PartsDesc.pParentTransform = m_pTransformCom;

		/*이펙트 껍데기 생성*/

		if (FAILED(Add_PartObject<CEffectParts>(strEffectName, g_iStaticLevel, &pEffectPart, &PartsDesc))) {
			assert(false);
			return E_FAIL;
		}

		/*이펙트 로드 로드*/
		size_t iPosition = {};
		_string strFilePath = strEffectPath;

		while ((iPosition = strFilePath.find(".bin")) != std::string::npos) {
			strFilePath.erase(iPosition, 4);
		};


		if (FAILED(pEffectPart->Load(strFilePath.c_str(), static_cast<LEVEL>(NEXT_LEVEL))))
		{
			Safe_Release(pEffectPart);
			continue;
		}

		Safe_Release(pEffectPart);

	}

	CloseHandle(hFile);

	return S_OK;
}
