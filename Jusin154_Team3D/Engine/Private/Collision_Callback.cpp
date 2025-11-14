#include "pch.h"
#include "Collision_Callback.h"
#include "GameInstance.h"

CCollision_Callback::CCollision_Callback()
{
}

void CCollision_Callback::onShapeHit(const PSX::PxControllerShapeHit& hit)
{
	PSX::PxController*		pController		= hit.controller;
	PSX::PxExtendedVec3		vWorldPos		= hit.worldPos;		// 접촉지점
	PSX::PxVec3				vWorldNormal	= hit.worldNormal;	// 접촉노말
	PSX::PxVec3				vDir			= hit.dir;			// move 방향
	_float					fLength			= hit.length;		// move 길이
	
	PSX::PxShape*			pShape			= hit.shape;
	PSX::PxRigidActor*		pActor			= hit.actor;


	if (nullptr != pController && nullptr != pActor) {
		PhsXUserData* pActorData = static_cast<PhsXUserData*>(pActor->userData);
		assert(nullptr != pActor); // missing user data
		switch (pActorData->eKind)
		{
			case PHYSX_KIND::BODY_STATIC:
				// action
				break;
			case PHYSX_KIND::BODY_DYNAMIC:
				// action
				break;
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

void CCollision_Callback::onControllerHit(const PSX::PxControllersHit& hit)
{
	PSX::PxController*		pController		= hit.controller;
	PSX::PxExtendedVec3		vWorldPos		= hit.worldPos;		// 접촉지점
	PSX::PxVec3				vWorldNormal	= hit.worldNormal;	// 접촉노말
	PSX::PxVec3				vDir			= hit.dir;			// move 방향
	_float					fLength			= hit.length;		// move 길이

	PSX::PxController*		pOtherController= hit.other;		// 다른 컨트롤러

	if (nullptr != pController && nullptr != pOtherController) {
		PhsXUserData* pActorData = static_cast<PhsXUserData*>(pOtherController->getActor()->userData);
		assert(nullptr != pOtherController); // missing user data

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

void CCollision_Callback::onObstacleHit(const PSX::PxControllerObstacleHit& hit)
{
	PSX::PxController*		pController		= hit.controller;
	PSX::PxExtendedVec3		vWorldPos		= hit.worldPos;		// 접촉지점
	PSX::PxVec3				vWorldNormal	= hit.worldNormal;	// 접촉노말
	PSX::PxVec3				vDir			= hit.dir;			// move 방향
	_float					fLength			= hit.length;		// move 길이

	const void*				pOtherController= hit.userData;		// 장애물

	//switch (pOtherController.member)
	//{
	//default:
	//	break;
	//}
}

PSX::PxControllerBehaviorFlags CCollision_Callback::getBehaviorFlags(const PSX::PxShape& shape, const PSX::PxActor& actor)
{
	//if (actor.userData.subKind == kMovingPlatformTag)
	//	return PSX::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT
	//			| PSX::PxControllerBehaviorFlag::eCCT_SLIDE;

	// 나머지는 아무 특별 동작 없음
	return PSX::PxControllerBehaviorFlags(0);
}

PSX::PxControllerBehaviorFlags CCollision_Callback::getBehaviorFlags(const PSX::PxController& controller)
{
	// CCT끼리 부딪힐 때를 굳이 안 쓰면 0 리턴
	return PSX::PxControllerBehaviorFlags(0);
}

PSX::PxControllerBehaviorFlags CCollision_Callback::getBehaviorFlags(const PSX::PxObstacle& obstacle)
{
	// Obstacle이 움직이는 발판이다? 
	// 여기서 ride/slide 세팅
	//return PSX::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;

	return PSX::PxControllerBehaviorFlags();
}
