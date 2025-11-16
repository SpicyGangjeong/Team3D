#include "pch.h"
#include "CallBack_Playable_Behavior.h"
#include "GameInstance.h"


CCallBack_Playable_Behavior::CCallBack_Playable_Behavior():
	m_pGameInstance(CGameInstance::GetInstance())
{
	SAFE_ADDREF(m_pGameInstance);
}

CCallBack_Playable_Behavior::~CCallBack_Playable_Behavior()
{
	SAFE_RELEASE(m_pGameInstance);
}

PSX::PxControllerBehaviorFlags CCallBack_Playable_Behavior::getBehaviorFlags(const PSX::PxShape& shape, const PSX::PxActor& actor)
{
	if (nullptr == actor.userData) {
		return PSX::PxControllerBehaviorFlags(0);
	}
	PSX::PxControllerBehaviorFlags eResults = PSX::PxControllerBehaviorFlags(0);
	PhsXUserData* pActorData = static_cast<PhsXUserData*>(actor.userData);
	switch (pActorData->eKind)
	{
	case PHYSX_KIND::BODY_STATIC:
		eResults = PSX::PxControllerBehaviorFlag::eCCT_SLIDE | PSX::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
		break;
	case PHYSX_KIND::BODY_DYNAMIC:
		switch (pActorData->iSubKind)
		{
		case 20:
			eResults = PSX::PxControllerBehaviorFlag::eCCT_SLIDE;
			break;
		case 21:
			eResults = PSX::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
			break;
		case 22:
			eResults = PSX::PxControllerBehaviorFlag::eCCT_SLIDE | PSX::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
			break;
		case 23:
			eResults = PSX::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;
			break;
		case 24:
			eResults = PSX::PxControllerBehaviorFlags(0);
			break;
		default:
			break;
		}
		break;
	case PHYSX_KIND::CCTActor:
		// Action
		break;
	case PHYSX_KIND::OBSTACLEActor:
		break;
	default:
		break;
	}

	return eResults;
}

PSX::PxControllerBehaviorFlags CCallBack_Playable_Behavior::getBehaviorFlags(const PSX::PxController& controller)
{
	// CCT끼리 부딪힐 때를 굳이 안 쓰면 0 리턴
	return PSX::PxControllerBehaviorFlags(0);
}

PSX::PxControllerBehaviorFlags CCallBack_Playable_Behavior::getBehaviorFlags(const PSX::PxObstacle& obstacle)
{
	// Obstacle이 움직이는 발판이다? 
	// 여기서 ride/slide 세팅
	//return PSX::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;

	return PSX::PxControllerBehaviorFlags(0);
}
