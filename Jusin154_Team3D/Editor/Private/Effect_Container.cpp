#include "pch.h"
#include "Effect_Container.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "TrailObject.h"


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

#ifdef _DEBUG

HRESULT CEffect_Container::Save_Package(const _char* pPath)
{
	_string strPerfectFilePath = pPath;
	strPerfectFilePath += ".bin";

	HANDLE	hFile = CreateFile(CMyTools::ToWstring(strPerfectFilePath).c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL
	);


	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"패키징 저장 실패", L"System Message", MB_OK);
		return E_FAIL;
	}

	;

	DWORD	dwByte(0);

	_int iObjectCount = (_int)m_PartObjects.size();

	if (!WriteFile(hFile, &iObjectCount, sizeof(_int), &dwByte, nullptr)) {
		return E_FAIL;
	}

	// 자식 개수 저장

	for (auto& pPartObejct : m_PartObjects)
	{
		CEditEffect* pEffectObject = dynamic_cast<CEditEffect*>(pPartObejct.second);
		CTrailObject* pTrailObject = {};

		if (pEffectObject == nullptr) // 이펙트 오브젝트가 아닐 경우
		{
			pTrailObject = dynamic_cast<CTrailObject*>(pPartObejct.second);

			pTrailObject->Save_Path(hFile);
		}
		else
		{
			pEffectObject->Save_Path(hFile);
		}
	}


	CloseHandle(hFile);


	MessageBox(NULL, L"패키징 성공", L"System Message", MB_OK);

	return S_OK;
}

#endif

HRESULT CEffect_Container::Load_Package(const _char* pPath)
{
	_string strPerfectFilePath = pPath;

	strPerfectFilePath += ".bin";

	DWORD	dwByte(0);

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
		MessageBox(NULL, L"패키징 읽기 실패", L"System Message", MB_OK);
		return E_FAIL;
	}

	_int iObjectCount = {};

	if (!ReadFile(hFile, &iObjectCount, sizeof(_int), &dwByte, nullptr)) {
		return E_FAIL;
	}

	for (size_t i = 0; i < iObjectCount; i++)
	{
		EFFECT_TYPE eEffectType = {};

		if (!ReadFile(hFile, &eEffectType, sizeof(EFFECT_TYPE), &dwByte, nullptr)) {
			return E_FAIL;
		}

		size_t iFilePathLength = {};

		if (!ReadFile(hFile, &iFilePathLength, sizeof(size_t), &dwByte, nullptr)) {
			return E_FAIL;
		}

		_char szFilePath[MAX_PATH] = {};

		if (iFilePathLength != 0)
		{
			if (!ReadFile(hFile, szFilePath, sizeof(_char) * ((DWORD)iFilePathLength + 1), &dwByte, nullptr)) {
				return E_FAIL;
			}
		}

		_string strPath = szFilePath;
		size_t iPos;

		while ((iPos = strPath.find(".bin")) != std::string::npos) {
			strPath.erase(iPos, 4);
		};


		CPartObject::PARTOBJECT_DESC PartsDesc{};
		CEditEffect*	pEditEffect = {};
		CTrailObject*	pTrailEffect = {};
		switch (eEffectType)
		{
		case EFFECT_TYPE::EFFECT:


			PartsDesc.pParentTransform = m_pTransformCom;


			if (FAILED(Add_PartObject<CEditEffect>("EffectObject" + i, ENUM_CLASS(LEVEL::EFFECT), &pEditEffect, &PartsDesc)))
			{
				return E_FAIL;
			}

			pEditEffect->Load(strPath.c_str(), LEVEL::EFFECT);

			SAFE_RELEASE(pEditEffect);

			break;

		case EFFECT_TYPE::TRAIL:

			PartsDesc.pParentTransform = m_pTransformCom;


			if (FAILED(Add_PartObject<CTrailObject>("TrailObject" + i, ENUM_CLASS(LEVEL::EFFECT), &pTrailEffect, &PartsDesc)))
			{
				return E_FAIL;
			}

			pTrailEffect->Load_Trail(strPath.c_str(), LEVEL::EFFECT);

			SAFE_RELEASE(pTrailEffect);

			break;
		case EFFECT_TYPE::END:
			break;
		default:
			break;
		}
	
	}

	CloseHandle(hFile);

	MessageBox(NULL, L"패키징 로드 성공", L"System Message", MB_OK);

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

	if (m_fAccTime >= m_fDelay && m_isDelayed == false) // 아직 딜레이되지 않았고 딜레이를 넘어섰다면
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
	}


	for (auto& pPair : m_Events) // 이벤트들을 비교하면서 실행함
	{
		if (pPair.first >= m_fPreAccTime && pPair.first <= m_fAccTime)
		{
			pPair.second();
		}
	}
	

}
