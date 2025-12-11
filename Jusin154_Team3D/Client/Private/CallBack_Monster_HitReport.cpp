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

void CCallBack_Monster_HitReport::BeginFrame()
{
	// 매 프레임 초기화
	m_vClimbNormal = { 0.f, 1.f, 0.f };
	m_bGroundHit = { false };
	m_fBestNormal = { 0.f };
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
			switch (PXOBJECT(pTargetActorData->iSubKind))
			{
			case PXOBJECT::TERRAIN:
			case PXOBJECT::FLOOR:
			case PXOBJECT::ROAD:
				if (vWorldNormal.y > 0) {
					_float fDotValue = vWorldNormal.dot({ 0.f, 1.f, 0.f });
					if (m_fBestNormal < fDotValue) {
						m_bGroundHit = true;
						m_fBestNormal = fDotValue;
						m_vClimbNormal = { vWorldNormal.x, vWorldNormal.y, vWorldNormal.z };
					}
				}
				break;
			default:
				break;
			}
			break;
		case PHYSX_KIND::BODY_DYNAMIC:
		{
			switch (PXOBJECT(pTargetActorData->iSubKind))
			{
			case PXOBJECT::END:
				break;
			case PXOBJECT::BOX:
			{
				PSX::PxRigidDynamic* pDynamic = static_cast<PSX::PxRigidDynamic*>(pActor);
				//pDynamic->setRigidBodyFlag(PSX::PxRigidBodyFlag::eKINEMATIC, false);
				PSX::PxVec3 vCompressedDir = -vWorldNormal;
				vCompressedDir.normalize();
				pDynamic->addForce(vCompressedDir * fLength * 1000.f, PSX::PxForceMode::eFORCE);
			} break;
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
	PSX::PxActor* pActor = pOtherController->getActor();

	ON_COLLISION_INFO CollisionDesc = {};

	if (nullptr != pController && nullptr != pOtherController) {
		PhsXUserData* pTargetActorData = static_cast<PhsXUserData*>(pActor->userData);
		PhsXUserData* pOwnerActorData = static_cast<PhsXUserData*>(pController->getActor()->userData);
		assert(nullptr != pTargetActorData); // missing user data

		switch (pTargetActorData->eKind)
		{
		case PHYSX_KIND::BODY_STATIC:
			assert(false);
			break;
		case PHYSX_KIND::BODY_DYNAMIC:
			assert(false);
			break;
		case PHYSX_KIND::CCTActor:
			switch (pTargetActorData->iSubKind)
			{
			case ENUM_CLASS(PXOBJECT::SKILL_NORMALJAP):
				break;
			default:
			{
				PSX::PxRigidDynamic* pDynamic = static_cast<PSX::PxRigidDynamic*>(pActor);
				pDynamic->addForce(vDir * fLength * 100000.f, PSX::PxForceMode::eIMPULSE);
			}
			break;
			}
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
	PSX::PxExtendedVec3		vWorldPos = hit.worldPos;			// 접촉지점
	PSX::PxVec3				vWorldNormal = hit.worldNormal;		// 접촉노말
	PSX::PxVec3				vDir = hit.dir;						// 시도한 move 방향
	_float					fLength = hit.length;				// 시도한 move 길이

	const void* pOtherController = hit.userData;		// 장애물

	//switch (pOtherController.member)
	//{
	//default:
	//	break;
	//}
}

void CCallBack_Monster_HitReport::Set_CurrentSlop()
{
	m_pController->Rewind_Grounded();
	if (m_bGroundHit)
	{
		m_pController->Set_OnGroundFlag(m_bGroundHit);
		float fDot = clamp(m_fBestNormal, -1.0f, 1.0f);
		float fAngleRad = acosf(fDot);
		float fAngleDeg = XMConvertToDegrees(fAngleRad);

		m_pController->Set_CurrentSlope(fAngleDeg);
	}
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

_bool CCallBack_Monster_HitReport::IsOnGround()
{
	return m_bGroundHit;
}

_vector CCallBack_Monster_HitReport::Get_GroundVector()
{
	return XMLoadFloat3(&m_vClimbNormal);
}

CCallBack_Monster_HitReport* CCallBack_Monster_HitReport::Create()
{
	return new CCallBack_Monster_HitReport();
}
