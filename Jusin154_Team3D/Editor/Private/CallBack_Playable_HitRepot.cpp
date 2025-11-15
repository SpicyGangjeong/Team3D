#include "pch.h"
#include "CallBack_Playable_HitRepot.h"
#include "GameInstance.h"
#include "GameObject.h"

CCallBack_Playable_HitRepot::CCallBack_Playable_HitRepot():
	m_pGameInstance(CGameInstance::GetInstance())
{
	SAFE_ADDREF(m_pGameInstance);
}

CCallBack_Playable_HitRepot::~CCallBack_Playable_HitRepot()
{
	SAFE_RELEASE(m_pGameInstance);
}

void CCallBack_Playable_HitRepot::onShapeHit(const PSX::PxControllerShapeHit& hit)
{
	PSX::PxController*		pController = hit.controller;
	PSX::PxExtendedVec3		vWorldPos = hit.worldPos;		// 접촉지점
	PSX::PxVec3				vWorldNormal = hit.worldNormal;	// 접촉노말
	PSX::PxVec3				vDir = hit.dir;					// 시도한 move 방향
	_float					fLength = hit.length;			// 시도한 move 길이

	PSX::PxShape* pShape = hit.shape;
	PSX::PxRigidActor* pActor = hit.actor;

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
			case 23: // 무거운데 올라갈 수 있는 벽
				if (m_pGameInstance->Key_Pressing(DIK_E)) {
					pOwnerActorData->pOwner->Get_Component<CTransform>()->BookMomentum(fLength * XMVectorSet(0.f, 1.f, 0.f, 0.f));
				}																			   					  
				else if (m_pGameInstance->Key_Pressing(DIK_R)) {							   					  
					pOwnerActorData->pOwner->Get_Component<CTransform>()->BookMomentum(fLength * XMVectorSet(0.f, -1.f, 0.f, 0.f));
				}
				break;
			default:
				break;
			}
			PSX::PxRigidDynamic* pDynamic = static_cast<PSX::PxRigidDynamic*>(pActor);
			pDynamic->addForce(vDir * fLength * 100000.f, PSX::PxForceMode::eIMPULSE);
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

void CCallBack_Playable_HitRepot::onControllerHit(const PSX::PxControllersHit& hit)
{
	PSX::PxController*		pController = hit.controller;
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

void CCallBack_Playable_HitRepot::onObstacleHit(const PSX::PxControllerObstacleHit& hit)
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
