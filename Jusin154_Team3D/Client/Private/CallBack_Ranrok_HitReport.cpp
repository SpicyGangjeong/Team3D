#include "pch.h"
#include "CallBack_Ranrok_HitReport.h"
#include "GameInstance.h"
#include "Character_Controller.h"
#include "RigidBody_Dynamic.h"
#include "Unit.h"
#include "GameObject.h"

CCallBack_Ranrok_HitReport::CCallBack_Ranrok_HitReport()
{
}

CCallBack_Ranrok_HitReport::~CCallBack_Ranrok_HitReport()
{
}

void CCallBack_Ranrok_HitReport::BeginFrame()
{
	// 매 프레임 초기화
	m_vClimbNormal = { 0.f, 1.f, 0.f };
	m_bGroundHit = { false };
	m_fBestNormal = { 0.f };
}

void CCallBack_Ranrok_HitReport::onShapeHit(const PSX::PxControllerShapeHit& hit)
{
	PSX::PxController* pController = hit.controller;
	PSX::PxExtendedVec3		vWorldPos = hit.worldPos;		// 접촉지점
	PSX::PxVec3				vWorldNormal = hit.worldNormal;	// 접촉노말
	PSX::PxVec3				vDir = hit.dir;					// 시도한 move 방향
	_float					fLength = hit.length;			// 시도한 move 길이

	PSX::PxShape* pShape = hit.shape;
	PSX::PxRigidActor* pActor = hit.actor;

	if (nullptr != pController && nullptr != pActor) {
		PHYSX_USERDATA* pTargetActorData = static_cast<PHYSX_USERDATA*>(pActor->userData);
		PHYSX_USERDATA* pOwnerActorData = static_cast<PHYSX_USERDATA*>(pController->getActor()->userData);
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

void CCallBack_Ranrok_HitReport::onControllerHit(const PSX::PxControllersHit& hit)
{
	PSX::PxController* pController = hit.controller;
	PSX::PxExtendedVec3		vWorldPos = hit.worldPos;		// 접촉지점
	PSX::PxVec3				vWorldNormal = hit.worldNormal;	// 접촉노말
	PSX::PxVec3				vDir = hit.dir;			// 시도한 move 방향
	_float					fLength = hit.length;		// 시도한 move 길이

	PSX::PxController* pOtherController = hit.other;		// 다른 컨트롤러
	PSX::PxActor* pActor = pOtherController->getActor();

	

	if (nullptr != pController && nullptr != pOtherController) {
		PHYSX_USERDATA* pTargetActorData = static_cast<PHYSX_USERDATA*>(pActor->userData);
		PHYSX_USERDATA* pOwnerActorData = static_cast<PHYSX_USERDATA*>(pController->getActor()->userData);

		ON_COLLISION_INFO CollisionDesc = {};

		_vector vHitDir = pTargetActorData->pCharacter->Get_Owner()->Get_WorldPostion() - m_pController->Get_Owner()->Get_WorldPostion();
		vHitDir = XMVector3Normalize(vHitDir);
		XMStoreFloat4(&CollisionDesc.vHitDir, vHitDir);
		CollisionDesc.eHitType = ENUM_CLASS(HIT_TYPE::HIT_HEAVY);
		CollisionDesc.fDamage = 30.f;
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
			case ENUM_CLASS(PXOBJECT::PLAYER):
			{
				class CUnit* pOwner = (CUnit*)m_pController->Get_Owner();
				CModel* pModel = pOwner->Get_Component<CModel>();
				_uint iAnimIndex = pModel->Get_AnimIndex();
				if (pOwner->Get_AnimInfo(STATEANIM::RUSH_LOOP).first == iAnimIndex) {
					if (false == *m_pCollisionPlayer) {
						*m_pCollisionPlayer = true;
						pTargetActorData->pCharacter->Get_Owner()->OnCollision(pOwner, &CollisionDesc);
					}
				}
			}
				break;
			default:
			{
				PSX::PxRigidDynamic* pDynamic = static_cast<PSX::PxRigidDynamic*>(pActor);
				pDynamic->addForce(vDir * fLength * 100.f, PSX::PxForceMode::eIMPULSE);
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

void CCallBack_Ranrok_HitReport::onObstacleHit(const PSX::PxControllerObstacleHit& hit)
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

void CCallBack_Ranrok_HitReport::Set_CurrentSlop()
{
	m_pController->Rewind_Grounded();
	if (m_bGroundHit)
	{
		m_pController->Set_OnGroundFlag(m_bGroundHit);
		float fDot = clamp(m_fBestNormal, -1.0f, 1.0f);
		float fAngleRad = acosf(fDot);
		float fAngleDeg = XMConvertToDegrees(fAngleRad);

		m_pController->Set_CurrentSlope(fAngleDeg, m_vClimbNormal);
	}
}

HRESULT CCallBack_Ranrok_HitReport::Initialize(CCharacter_Controller* pController, CRigidBody_Dynamic* pPartDynamicObject, _bool* pCollisionPlayer)
{
	m_pController = pController;
	m_pPartDynamicBody = pPartDynamicObject;
	m_pGameInstance = CGameInstance::GetInstance();
	m_pCollisionPlayer = pCollisionPlayer;
	SAFE_ADDREF(m_pController);
	SAFE_ADDREF(m_pPartDynamicBody);
	SAFE_ADDREF(m_pGameInstance);
	return S_OK;
}

HRESULT CCallBack_Ranrok_HitReport::Finalize()
{
	SAFE_RELEASE(m_pGameInstance);
	SAFE_RELEASE(m_pPartDynamicBody);
	SAFE_RELEASE(m_pController);
	return S_OK;
}

_bool CCallBack_Ranrok_HitReport::IsOnGround()
{
	return m_bGroundHit;
}

_vector CCallBack_Ranrok_HitReport::Get_GroundVector()
{
	return XMLoadFloat3(&m_vClimbNormal);
}

CCallBack_Ranrok_HitReport* CCallBack_Ranrok_HitReport::Create()
{
	return new CCallBack_Ranrok_HitReport();
}
