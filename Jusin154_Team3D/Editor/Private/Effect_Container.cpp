#include "pch.h"
#include "Effect_Container.h"

#include "GameInstance.h"
#include "EditEffect.h"
#include "TrailObject.h"
#include "GameObject.h"
#include "Dummy_PhysXEffectHitBox.h"

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

HRESULT CEffect_Container::Load_Directory(const _char* pPath)
{
	if (filesystem::exists(pPath) == false) {
		MessageBox(NULL, L"존재하지 않는 경로", L"System Message", MB_OK);
		return E_FAIL;
	}

	for (const auto& file : filesystem::directory_iterator(pPath))
	{
		if (file.is_directory())
			continue;

		string ext = file.path().extension().string();

		if (!(strcmp(ext.c_str(), ".bin") ^ strcmp(ext.c_str(), ".trail")))
			continue;

		_char szFilePath[MAX_PATH] = {};

		strcpy_s(szFilePath, MAX_PATH, file.path().string().c_str());

		/* Trail */

		if (!strcmp(ext.c_str(), ".trail"))
		{
			CTrailObject* pTrail = nullptr;

			CPartObject::PARTOBJECT_DESC PartsDesc{};

			PartsDesc.pParentTransform = m_pTransformCom;


			_wstring wstrFileName = file.path().stem().wstring(); //이름 넣기

			if (FAILED(Add_PartObject<CTrailObject>(CMyTools::ToString(wstrFileName), NEXT_LEVEL, &pTrail, &PartsDesc))) {
				assert(false);
				return E_FAIL;
			}


			/*이펙트 로드 로드*/
			size_t iPos = {};
			_string strFilePath = szFilePath;

			while ((iPos = strFilePath.find(".trail")) != std::string::npos) {
				strFilePath.erase(iPos, 6);
			};


			if (FAILED(pTrail->Load_Trail(strFilePath.c_str(), LEVEL::EFFECT)))
			{
				Safe_Release(pTrail);
				continue;
			}

			Safe_Release(pTrail);

			continue;
		}


		/////////////////////////////

		CEditEffect* pEditEffect = nullptr;

		CPartObject::PARTOBJECT_DESC PartsDesc{};

		PartsDesc.pParentTransform = m_pTransformCom;




		/*이펙트 껍데기 생성*/

		_wstring wstrFileName = file.path().stem().wstring(); //이름 넣기

		if (FAILED(Add_PartObject<CEditEffect>(CMyTools::ToString(wstrFileName), NEXT_LEVEL, &pEditEffect, &PartsDesc))) {
			assert(false);
			return E_FAIL;
		}
		

		/*이펙트 로드 로드*/
		size_t iPos = {};
		_string strFilePath = szFilePath;

		while ((iPos = strFilePath.find(".bin")) != std::string::npos) {
			strFilePath.erase(iPos, 4);
		};


		if (FAILED(pEditEffect->Load(strFilePath.c_str(), LEVEL::EFFECT)))
		{
			Safe_Release(pEditEffect);
			continue;
		}

		Safe_Release(pEditEffect);
	}

	return S_OK;
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

			if (FAILED(Add_PartObject<CTrailObject>(strEffectName, NEXT_LEVEL, &pTrail, &PartsDesc))) {
				assert(false);
				return E_FAIL;
			}


			/*이펙트 로드 로드*/
			size_t iPos = {};

			while ((iPos = strEffectPath.find(".trail")) != std::string::npos) {
				strEffectPath.erase(iPos, 6);
			};


			if (FAILED(pTrail->Load_Trail(strEffectPath.c_str(), LEVEL::EFFECT)))
			{
				Safe_Release(pTrail);
				continue;
			}

			Safe_Release(pTrail);

			continue;
		}


		/////////////////////////////

		CEditEffect* pEditEffect = nullptr;

		CPartObject::PARTOBJECT_DESC PartsDesc{};

		PartsDesc.pParentTransform = m_pTransformCom;

		/*이펙트 껍데기 생성*/

		if (FAILED(Add_PartObject<CEditEffect>(strEffectName, NEXT_LEVEL, &pEditEffect, &PartsDesc))) {
			assert(false);
			return E_FAIL;
		}

		/*이펙트 로드 로드*/
		size_t iPosition = {};
		_string strFilePath = strEffectPath;

		while ((iPosition = strFilePath.find(".bin")) != std::string::npos) {
			strFilePath.erase(iPosition, 4);
		};


		if (FAILED(pEditEffect->Load(strFilePath.c_str(), LEVEL::EFFECT)))
		{
			Safe_Release(pEditEffect);
			continue;
		}

		Safe_Release(pEditEffect);
	
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEffect_Container::Pre_Setting(CGameObject* pObject, void* pArg)
{
	if (pObject == nullptr)
		return E_FAIL;

	m_pOwner = pObject;


	Reset_EditEffect();
	Reset_Light();

	m_fAccTime = 0.f;
	m_fPreAccTime = 0.f;

	m_bVisible = true;
	m_isCollisionEnter = false;
	m_bHit = false;

	return S_OK;
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

HRESULT CEffect_Container::Reset_EditEffect()
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

ON_COLLISION_INFO CEffect_Container::SweepTarget(_vector StartPos, _vector EndPos, _float fRadius, _bool isTerrainCollision)
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
			PhsXUserData* pUserData = static_cast<PhsXUserData*>(pActor->userData);
			tagCollInfo.pObject = pUserData->pOwner;

			switch (pUserData->eKind)
			{
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

			PhsXUserData* pUserData = static_cast<PhsXUserData*>(pActor->userData);
			tagCollInfo.pObject = pUserData->pOwner;

			switch (pUserData->eKind)
			{
			case PHYSX_KIND::CCTActor:
			{
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::TERRAIN:
				{

					bHit = true;
					break;
				}
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
			PhsXUserData* pUserData = static_cast<PhsXUserData*>(pActor->userData);
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
				PhsXUserData* pUserData = static_cast<PhsXUserData*>(pActor->userData);
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

			PhsXUserData* pUserData = static_cast<PhsXUserData*>(pActor->userData);

			if (pUserData->iSubKind >= UINT_MAX - 1) {
				continue;
			}

			switch (PXOBJECT(pUserData->eKind))
			{
			case PXOBJECT::PLAYER:
				continue;
			case PXOBJECT::MONSTER:
			case PXOBJECT::GOBLIN_WARRIOR:
			case PXOBJECT::TROLL:
			case PXOBJECT::WALL:

			{
				return i;
			}
			break;
			default:
				break;
			}

			switch (pUserData->eKind)
			{
			case PHYSX_KIND::BODY_STATIC:
			case PHYSX_KIND::BODY_DYNAMIC:
			{
				return i;
			}
			break;
			case PHYSX_KIND::CCTActor:
				break;
			default:
				break;
			}
		}
	}

	return -1;

}
