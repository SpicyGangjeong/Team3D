#include "pch.h"
#include "Effect_Container.h"

#include "GameInstance.h"
#include "EffectParts.h"
#include "TrailObject.h"
#include "GameObject.h"

#include <sstream>

CEffect_Container::CEffect_Container(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainerObject{ pDevice, pContext }
{
}

CEffect_Container::CEffect_Container(const CEffect_Container& rhs)
	: CContainerObject(rhs),
	m_EffectsInfo(rhs.m_EffectsInfo),
	m_TrailsInfo(rhs.m_TrailsInfo)
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

	m_bVisible = false;

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

void CEffect_Container::Reset_Light()
{
	for (auto& iter : m_PartObjects)
	{
		CLight* pLight = iter.second->Get_Component<CLight>();

		if (pLight == nullptr)
			continue;

		pLight->Reset_IntensityRatio();
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

		if (iPos != string::npos)
		{
			strExt += strEffectPath.substr(iPos + 1);
			strEffectName = strEffectPath.substr(0, iPos);
		}

		size_t pos = strEffectName.rfind('\\');   // 뒤에서부터 '/' 검색

		if (pos != string::npos)
			strEffectName = strEffectName.substr(pos + 1);

		if (!strcmp(strExt.c_str(), "trail"))
		{
		
			Load_Trail_Data(strEffectPath.c_str());

			m_TrailsInfo.back().wstrTrailName = CMyTools::ToWstring(strEffectName);
			continue;
		}

		/////////////////////////////
		
		_string strFilePath = strEffectPath;
		Load_Data(strFilePath.c_str());
		
		m_EffectsInfo.back().wstrEffectName = CMyTools::ToWstring(strEffectName);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEffect_Container::Load_Data(const _char* pPath)
{
	_string strPerfectFilePath = pPath;

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

	EFFECT_SAVE_INFO EffectSaveInfo = {};

	DWORD	dwByte(0);

	if (!ReadFile(hFile, &EffectSaveInfo.EffectInfo, sizeof(EFFECT_INFO), &dwByte, nullptr)) {
		CloseHandle(hFile);
		return E_FAIL;
	}

	if (EffectSaveInfo.EffectInfo.isDiffuse)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			EffectSaveInfo.wstrDiffuseName = CMyTools::ToWstring(szName);
		}


	}

	if (EffectSaveInfo.EffectInfo.isNoise)
	{
		size_t iComponentLength = {};


		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			EffectSaveInfo.wstrNoiseName = CMyTools::ToWstring(szName);
		}


	}

	if (EffectSaveInfo.EffectInfo.isMasking)
	{
		size_t iComponentLength = {};


		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{

			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}


			EffectSaveInfo.wstrMaskingName = CMyTools::ToWstring(szName);
		}


	}

	if (EffectSaveInfo.EffectInfo.isDissolve)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			EffectSaveInfo.wstrDissolveName = CMyTools::ToWstring(szName);
		}


	}

	if (EffectSaveInfo.EffectInfo.isEmissive)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			EffectSaveInfo.wstrEmissiveName = CMyTools::ToWstring(szName);
		}


	}

	if (EffectSaveInfo.EffectInfo.isDistortion)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			EffectSaveInfo.wstrDistortionName = CMyTools::ToWstring(szName);
		}


	}

	if (EffectSaveInfo.EffectInfo.isNomalMap)
	{
		size_t iComponentLength = {};

		if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			_char szName[MAX_PATH] = {};

			if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				CloseHandle(hFile);
				return E_FAIL;
			}

			EffectSaveInfo.wstrNomalName = CMyTools::ToWstring(szName);
		}


	}

	size_t iComponentLength = {};


	if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		CloseHandle(hFile);
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)(DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			CloseHandle(hFile);
			return E_FAIL;
		}


		EffectSaveInfo.wstrModelName = CMyTools::ToWstring(szName);
	}

	if (!ReadFile(hFile, &EffectSaveInfo.InstanceModelInfo, sizeof(CInstance_Model::INSTANCE_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}


	CloseHandle(hFile);

	m_EffectsInfo.push_back(EffectSaveInfo);

	return S_OK;
}

HRESULT CEffect_Container::Create_Effect()
{
	CEffectParts* pEffectParts = nullptr;
	CTrailObject* pTrail = nullptr;

	CPartObject::PARTOBJECT_DESC PartsDesc{};

	PartsDesc.pParentTransform = m_pTransformCom;

	/*이펙트 껍데기 생성*/



	for (auto& EffectInfo : m_EffectsInfo)
	{

		if (FAILED(Add_PartObject<CEffectParts>(CMyTools::ToString(EffectInfo.wstrEffectName), g_iStaticLevel, &pEffectParts, &PartsDesc))) {
			return E_FAIL;
		}

		pEffectParts->Load(EffectInfo, static_cast<LEVEL>(g_iStaticLevel));

		Safe_Release(pEffectParts);
	}

	for (auto& TrailInfo : m_TrailsInfo)
	{
		if (FAILED(Add_PartObject<CTrailObject>(CMyTools::ToString(TrailInfo.wstrTrailName), g_iStaticLevel, &pTrail, &PartsDesc))) {
			return E_FAIL;
		}

		pTrail->Load_Trail(TrailInfo, static_cast<LEVEL>(g_iStaticLevel));

		Safe_Release(pTrail);
	}

	
	return S_OK;
}

HRESULT CEffect_Container::Load_Trail_Data(const _char* pPath)
{
	_string strPerfectFilePath = pPath;

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
		MessageBox(NULL, L"오브젝트 읽기 실패", L"System Message", MB_OK);
		return E_FAIL;
	}

	TRAIL_SAVE_INFO Trail_Save_Info = {};


	size_t iComponentLength = {};

	if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}

		Trail_Save_Info.wstrDiffuseName = CMyTools::ToWstring(szName);
	}

	if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}

		Trail_Save_Info.wstrNoiseName = CMyTools::ToWstring(szName);

	}

	if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}

		Trail_Save_Info.wstrMaskingName = CMyTools::ToWstring(szName);
	}


	if (!ReadFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		_char szName[MAX_PATH] = {};

		if (!ReadFile(hFile, &szName, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}

		Trail_Save_Info.wstrDistortionName = CMyTools::ToWstring(szName);

	}

	if (!ReadFile(hFile, &Trail_Save_Info.TrailComponentDesc, sizeof(CTrail::TRAIL_DESC), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (!ReadFile(hFile, &Trail_Save_Info.TrailDesc, sizeof(TRAIL_INFO), &dwByte, nullptr)) {
		return E_FAIL;
	}


	m_TrailsInfo.push_back(Trail_Save_Info);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEffect_Container::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (pObject == nullptr)
		return E_FAIL;

	m_pOwner = pObject;


	Reset_EffectParts();
	Reset_Light();

	m_fAccTime = 0.f;
	m_fPreAccTime = 0.f;

	m_bVisible = true;
	m_isCollisionEnter = false;

	m_bHit = false;

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

HRESULT CEffect_Container::Reset_EffectParts()
{
	for (auto& iter : m_PartObjects)
	{
		CInstance_Model* pInstanceModel = iter.second->Get_Component<CInstance_Model>();

		if (pInstanceModel == nullptr)
			continue;

		pInstanceModel->Instane_Buffer_ReStruct();
	}

	return S_OK;
}
_int CEffect_Container::CollisionCheck()
{
	_bool bIsCollide = { false };

	// 건드린 친구들 순서대로 다 가져옴 ( 정렬은 안되어있음 )
	for (PSX::PxU32 i = 0; i < m_Hitbuffer.nbTouches; ++i)
	{
		const PSX::PxSweepHit& Hit = m_Hitbuffer.touches[i];
		/* 기타 로직 */

		PSX::PxRigidActor* pActor = Hit.actor;
		PSX::PxShape* pShape = Hit.shape;

		if (nullptr != pActor && nullptr != pActor->userData) {

			PHYSX_USERDATA* pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);

			if (pUserData->iSubKind >= UINT_MAX - 1) {
				continue;
			}

			switch (PXOBJECT(pUserData->iSubKind))
			{
			case PXOBJECT::PLAYER:
				continue;
			case PXOBJECT::MONSTER:
			case PXOBJECT::GOBLIN_WARRIOR:
			case PXOBJECT::GOBLIN_MAGICIAN:
			case PXOBJECT::TROLL:
			case PXOBJECT::RANROK:
			case PXOBJECT::WALL:
			{
				return i;
			}
			break;
			default:
				break;
			}

			//switch (pUserData->eKind)
			//{

			//case PHYSX_KIND::BODY_STATIC:
			//case PHYSX_KIND::BODY_DYNAMIC:
			//{
			//	return i;
			//}
			//break;
			//case PHYSX_KIND::CCTActor:
			//	break;
			//default:
			//	break;
			//}
		}
	}

	return -1;

}
ON_COLLISION_INFO CEffect_Container::SweepTarget(_vector StartPos, _vector EndPos, _float fRadius , _bool isTerrainCollision )
{
	_vector vStartPos = StartPos;
	_vector vEndPos = EndPos;
	_vector vDir = { vEndPos - vStartPos };

	_float fDistance = XMVectorGetX(XMVector3Length(vEndPos - vStartPos));

	PSX::PxSweepBuffer pxBuffer = {};

	_bool bHit = m_pGameInstance->SphereCast(fRadius, vStartPos, vDir, fDistance, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eDYNAMIC, pxBuffer);

	const PSX::PxSweepHit& hit = pxBuffer.block;
	PSX::PxRigidActor* pActor = hit.actor;
	PSX::PxShape* pShape = hit.shape;
    ON_COLLISION_INFO tagCollInfo = {};

	tagCollInfo.vWorldPos.w = 1.f;

	if (bHit) {

		memcpy_s(&tagCollInfo.vWorldPos, sizeof(tagCollInfo.vWorldPos), &hit.position, sizeof(hit.position));

		memcpy_s(&tagCollInfo.vWorldNomal, sizeof(tagCollInfo.vWorldNomal), &hit.normal, sizeof(hit.normal));
		XMStoreFloat4(&tagCollInfo.vHitDir, vDir);
		tagCollInfo.fLength = fDistance;


		if (nullptr != pActor && nullptr != pActor->userData)
		{
			PHYSX_USERDATA* pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);
			tagCollInfo.pObject = pUserData->pOwner;

			switch (pUserData->eKind)
			{
			case PHYSX_KIND::BODY_DYNAMIC:
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::GOBLIN_PROTEGO:
				{
					pUserData->pOwner->OnCollision(this, &tagCollInfo);
					m_bHit = true;
				}
				break;
				}
				break;
			case PHYSX_KIND::CCTActor:
			{
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::MONSTER:
					break;
				case PXOBJECT::GOBLIN_WARRIOR:
				{
					pUserData->pOwner->OnCollision(this, &tagCollInfo);
					m_bHit = true;
				}
				break;
				case PXOBJECT::GOBLIN_MAGICIAN:
				{
					pUserData->pOwner->OnCollision(this, &tagCollInfo);
					m_bHit = true;
				}
				break;
				case PXOBJECT::TROLL:
				{
					pUserData->pOwner->OnCollision(this, &tagCollInfo);
					m_bHit = true;
				}
				break;
				case PXOBJECT::RANROK:
				{
					pUserData->pOwner->OnCollision(this, &tagCollInfo);
					m_bHit = true;
				}
				}
			}
			}
		}

		
	}

	if (isTerrainCollision == true && bHit == false)
	{
		memcpy_s(&tagCollInfo.vWorldPos, sizeof(tagCollInfo.vWorldPos), &hit.position, sizeof(hit.position));
		memcpy_s(&tagCollInfo.vWorldNomal, sizeof(tagCollInfo.vWorldNomal), &hit.normal, sizeof(hit.normal));
		XMStoreFloat4(&tagCollInfo.vHitDir, vDir);
		tagCollInfo.fLength = fDistance;
		tagCollInfo.pObject = m_pOwner->Get_Owner();

		bHit = m_pGameInstance->SphereCast(fRadius, vStartPos, vDir, fDistance, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eSTATIC, pxBuffer);

		const PSX::PxSweepHit& hit = pxBuffer.block;
		PSX::PxRigidActor* pActor = hit.actor;
		PSX::PxShape* pShape = hit.shape;

		tagCollInfo.vWorldPos.w = 1.f;

		if (bHit)
		{
			memcpy_s(&tagCollInfo.vWorldPos, sizeof(tagCollInfo.vWorldPos), &hit.position, sizeof(hit.position));
			memcpy_s(&tagCollInfo.vWorldNomal, sizeof(tagCollInfo.vWorldNomal), &hit.normal, sizeof(hit.normal));
			XMStoreFloat4(&tagCollInfo.vHitDir, vDir);
			tagCollInfo.fLength = fDistance;

			if (nullptr != pActor && nullptr != pActor->userData)
			{
				PHYSX_USERDATA* pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);
				tagCollInfo.pObject = pUserData->pOwner;

				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::TERRAIN:
				{

					m_bHit = true;
					break;
				}
				}

			}
		}
		

	}

	return tagCollInfo;
}

ON_COLLISION_INFO CEffect_Container::MonsterSweepTarget(_vector StartPos, _vector EndPos, _float fRadius, _bool isTerrainCollision)
{
	_vector vStartPos = StartPos;
	_vector vEndPos = EndPos;
	_vector vDir = { vEndPos - vStartPos };

	_float fDistance = XMVectorGetX(XMVector3Length(vEndPos - vStartPos));

	PSX::PxSweepBuffer pxBuffer = {};

	_bool bHit = m_pGameInstance->SphereCast(fRadius, vStartPos, vDir, fDistance, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eDYNAMIC, pxBuffer);

	const PSX::PxSweepHit& hit = pxBuffer.block;
	PSX::PxRigidActor* pActor = hit.actor;
	PSX::PxShape* pShape = hit.shape;
	ON_COLLISION_INFO tagCollInfo = {};

	tagCollInfo.vWorldPos.w = 1.f;

	if (bHit) {

		memcpy_s(&tagCollInfo.vWorldPos, sizeof(tagCollInfo.vWorldPos), &hit.position, sizeof(hit.position));

		memcpy_s(&tagCollInfo.vWorldNomal, sizeof(tagCollInfo.vWorldNomal), &hit.normal, sizeof(hit.normal));
		XMStoreFloat4(&tagCollInfo.vHitDir, vDir);
		tagCollInfo.fLength = fDistance;


		if (nullptr != pActor && nullptr != pActor->userData)
		{
			PHYSX_USERDATA* pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);
			tagCollInfo.pObject = pUserData->pOwner;

			switch (pUserData->eKind)
			{
			case PHYSX_KIND::BODY_DYNAMIC:
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::SKILL_PROTEGO:
				{
					pUserData->pOwner->OnCollision(this, &tagCollInfo);
					m_bHit = true;
				}
				break;
				}
				break;
			case PHYSX_KIND::CCTActor:
			{
				switch (PXOBJECT(pUserData->iSubKind))
				{

				case PXOBJECT::PLAYER:
				{
					pUserData->pOwner->OnCollision(this, &tagCollInfo);
					m_bHit = true;
				}
				break;

				}
			}
			}
		}


	}

	if (isTerrainCollision == true && bHit == false)
	{
		memcpy_s(&tagCollInfo.vWorldPos, sizeof(tagCollInfo.vWorldPos), &hit.position, sizeof(hit.position));
		memcpy_s(&tagCollInfo.vWorldNomal, sizeof(tagCollInfo.vWorldNomal), &hit.normal, sizeof(hit.normal));
		XMStoreFloat4(&tagCollInfo.vHitDir, vDir);
		tagCollInfo.fLength = fDistance;
		tagCollInfo.pObject = m_pOwner->Get_Owner();

		bHit = m_pGameInstance->SphereCast(fRadius, vStartPos, vDir, fDistance, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eSTATIC, pxBuffer);

		const PSX::PxSweepHit& hit = pxBuffer.block;
		PSX::PxRigidActor* pActor = hit.actor;
		PSX::PxShape* pShape = hit.shape;

		tagCollInfo.vWorldPos.w = 1.f;

		if (bHit)
		{
			memcpy_s(&tagCollInfo.vWorldPos, sizeof(tagCollInfo.vWorldPos), &hit.position, sizeof(hit.position));
			memcpy_s(&tagCollInfo.vWorldNomal, sizeof(tagCollInfo.vWorldNomal), &hit.normal, sizeof(hit.normal));
			XMStoreFloat4(&tagCollInfo.vHitDir, vDir);
			tagCollInfo.fLength = fDistance;

			if (nullptr != pActor && nullptr != pActor->userData)
			{
				PHYSX_USERDATA* pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);
				tagCollInfo.pObject = pUserData->pOwner;

				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::TERRAIN:
				{

					m_bHit = true;
					break;
				}
				}

			}
		}


	}

	return tagCollInfo;
}

ON_COLLISION_INFO CEffect_Container::MonsterRayCast(_vector StartPos, _vector _vDir, _float _fLength, _uint iMaxHitCapacity)
{
	_vector vStartPos = StartPos;
	_vector vDir = _vDir;

	_float fLength = _fLength;

	vector<PSX::PxRaycastHit> Raycasts = {};
	Raycasts.resize(iMaxHitCapacity);

	_bool bHit = m_pGameInstance->RayCast(vStartPos, vDir, fLength, Raycasts.data(), iMaxHitCapacity, m_iHitCount);

	ON_COLLISION_INFO tagCollInfo = {};

	for (_uint i = 0; i < iMaxHitCapacity; i++)
	{
		PSX::PxRigidActor* pActor = Raycasts[i].actor;
		PSX::PxShape* pShape = Raycasts[i].shape;


		tagCollInfo.vWorldPos.w = 1.f;

		if (bHit) {

			memcpy_s(&tagCollInfo.vWorldPos, sizeof(tagCollInfo.vWorldPos), &Raycasts[i].position, sizeof(Raycasts[i].position));

			memcpy_s(&tagCollInfo.vWorldNomal, sizeof(tagCollInfo.vWorldNomal), &Raycasts[i].normal, sizeof(Raycasts[i].normal));
			XMStoreFloat4(&tagCollInfo.vHitDir, vDir);
			tagCollInfo.fLength = fLength;

			if (nullptr != pActor && nullptr != pActor->userData)
			{
				PHYSX_USERDATA* pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);
				tagCollInfo.pObject = pUserData->pOwner;

				switch (pUserData->eKind)
				{
				case PHYSX_KIND::CCTActor:
				{
					switch (PXOBJECT(pUserData->iSubKind))
					{

					case PXOBJECT::PLAYER:
					{
						pUserData->pOwner->OnCollision(this, &tagCollInfo);
						m_bHit = true;
					}
					break;
					case PXOBJECT::SKILL_PROTEGO:
					{
						pUserData->pOwner->OnCollision(this, &tagCollInfo);
						m_bHit = true;
					}
					}
				}
				}


				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::TERRAIN:
				{

					m_bHit = true;
					break;
				}
				}
			}

		}

		return tagCollInfo;
	}

	return tagCollInfo;
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
