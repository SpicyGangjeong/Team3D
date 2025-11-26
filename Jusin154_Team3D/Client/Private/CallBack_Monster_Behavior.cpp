#include "pch.h"
#include "CallBack_Monster_Behavior.h"
#include "GameInstance.h"


CCallBack_Monster_Behavior::CCallBack_Monster_Behavior()
{
}

CCallBack_Monster_Behavior::~CCallBack_Monster_Behavior()
{
}

PSX::PxControllerBehaviorFlags CCallBack_Monster_Behavior::getBehaviorFlags(const PSX::PxShape& shape, const PSX::PxActor& actor)
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

PSX::PxControllerBehaviorFlags CCallBack_Monster_Behavior::getBehaviorFlags(const PSX::PxController& controller)
{
	// CCT끼리 부딪힐 때를 굳이 안 쓰면 0 리턴
	return PSX::PxControllerBehaviorFlags(0);
}

PSX::PxControllerBehaviorFlags CCallBack_Monster_Behavior::getBehaviorFlags(const PSX::PxObstacle& obstacle)
{
	// Obstacle이 움직이는 발판이다? 
	// 여기서 ride/slide 세팅
	//return PSX::PxControllerBehaviorFlag::eCCT_CAN_RIDE_ON_OBJECT;

	return PSX::PxControllerBehaviorFlags(0);
}

HRESULT CCallBack_Monster_Behavior::Initialize(CCharacter_Controller* pController, CRigidBody_Dynamic* pPartDynamicObject)
{
	m_pController = pController;
	m_pPartDynamicBody = pPartDynamicObject;
	m_pGameInstance = CGameInstance::GetInstance();
	SAFE_ADDREF(m_pController);
	SAFE_ADDREF(m_pPartDynamicBody);
	SAFE_ADDREF(m_pGameInstance);
	return S_OK;
}

HRESULT CCallBack_Monster_Behavior::Finalize()
{
	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pPartDynamicBody);
	SAFE_RELEASE(m_pController);
	return S_OK;
}

CCallBack_Monster_Behavior* CCallBack_Monster_Behavior::Create()
{
	return new CCallBack_Monster_Behavior();;
}
