#include "pch.h"
#include "CallBack_Monster_HitReport.h"
#include "GameInstance.h"
#include "Character_Controller.h"
#include "RigidBody_Dynamic.h"
#include "GameObject.h"

CCallBack_Monster_HitReport::CCallBack_Monster_HitReport()	
{
}

CCallBack_Monster_HitReport::~CCallBack_Monster_HitReport()
{
}

void CCallBack_Monster_HitReport::onShapeHit(const PSX::PxControllerShapeHit& hit)
{
	PSX::PxController*		pController = hit.controller;
	PSX::PxExtendedVec3		vWorldPos = hit.worldPos;		// 접촉지점
	PSX::PxVec3				vWorldNormal = hit.worldNormal;	// 접촉노말
	PSX::PxVec3				vDir = hit.dir;					// 시도한 move 방향
	_float					fLength = hit.length;			// 시도한 move 길이

	PSX::PxShape*			pShape = hit.shape;
	PSX::PxRigidActor*		pActor = hit.actor;

	if (nullptr != pController && nullptr != pActor) {
		PhsXUserData* pTargetActorData = static_cast<PhsXUserData*>(pActor->userData);
		PhsXUserData* pOwnerActorData = static_cast<PhsXUserData*>(pController->getActor()->userData);
		if (nullptr == pTargetActorData) { // missing user data
			return;
		}

		switch (pTargetActorData->eKind)
		{
		case PHYSX_KIND::BODY_STATIC:
			// action
			break;
		case PHYSX_KIND::BODY_DYNAMIC:
		{
			switch (pTargetActorData->iSubKind)
			{
			case 0:
				m_pController->ConvertToDO(*m_pPartDynamicBody);
				m_pPartDynamicBody->Add_Force(XMVectorSet(vDir.x, vDir.y, vDir.z, 0.f) * fLength * 100.f, PSX::PxForceMode::eIMPULSE);
				break;
			default:
			{
			}
			break;
			}
		}	break;
		case PHYSX_KIND::CCTActor:
			assert(false);
			break;
		case PHYSX_KIND::OBSTACLEActor:
			break;
		default:
			break;
		}
	}
}

void CCallBack_Monster_HitReport::onControllerHit(const PSX::PxControllersHit& hit)
{
	PSX::PxController* pController = hit.controller;
	PSX::PxExtendedVec3		vWorldPos = hit.worldPos;		// 접촉지점
	PSX::PxVec3				vWorldNormal = hit.worldNormal;	// 접촉노말
	PSX::PxVec3				vDir = hit.dir;			// 시도한 move 방향
	_float					fLength = hit.length;		// 시도한 move 길이

	PSX::PxController* pOtherController = hit.other;		// 다른 컨트롤러

	if (nullptr != pController && nullptr != pOtherController) {
		PhsXUserData* pActorData = static_cast<PhsXUserData*>(pOtherController->getActor()->userData);
		assert(nullptr != pActorData); // missing user data

		switch (pActorData->eKind)
		{
		case PHYSX_KIND::BODY_STATIC:
			assert(false);
			break;
		case PHYSX_KIND::BODY_DYNAMIC:
			assert(false);
			break;
		case PHYSX_KIND::CCTActor:
			// Action
			break;
		case PHYSX_KIND::OBSTACLEActor:
			break;
		default:
			break;
		}
	}
}

void CCallBack_Monster_HitReport::onObstacleHit(const PSX::PxControllerObstacleHit& hit)
{
	PSX::PxController* pController = hit.controller;
	PSX::PxExtendedVec3		vWorldPos = hit.worldPos;		// 접촉지점
	PSX::PxVec3				vWorldNormal = hit.worldNormal;	// 접촉노말
	PSX::PxVec3				vDir = hit.dir;			// 시도한 move 방향
	_float					fLength = hit.length;		// 시도한 move 길이

	const void* pOtherController = hit.userData;		// 장애물

	//switch (pOtherController.member)
	//{
	//default:
	//	break;
	//}
}

HRESULT CCallBack_Monster_HitReport::Initialize(CCharacter_Controller* pController, CRigidBody_Dynamic* pPartDynamicObject)
{
	m_pController = pController;
	m_pPartDynamicBody = pPartDynamicObject;
	m_pGameInstance = CGameInstance::GetInstance();
	SAFE_ADDREF(m_pController);
	SAFE_ADDREF(m_pPartDynamicBody);
	SAFE_ADDREF(m_pGameInstance);
	return S_OK;
}

HRESULT CCallBack_Monster_HitReport::Finalize()
{
	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pPartDynamicBody);
	SAFE_RELEASE(m_pController);
	return S_OK;
}

CCallBack_Monster_HitReport* CCallBack_Monster_HitReport::Create()
{
	return new CCallBack_Monster_HitReport();
}
