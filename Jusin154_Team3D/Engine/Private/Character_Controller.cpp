#include "pch.h"
#include "Character_Controller.h"
#include "GameInstance.h"

CCharacter_Controller::CCharacter_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{

}

CCharacter_Controller::CCharacter_Controller(const CCharacter_Controller& rhs)
	: CComponent(rhs)
{

}
#ifdef _DEBUG

HRESULT CCharacter_Controller::Render()
{
	_vector  vPos = Get_Position();
	_float3  vVolume = Get_Volume(); 
	_matrix  WorldMatrix = XMMatrixTranslationFromVector(vPos);
	
	_matrix ViewMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW);
	_matrix ProjMatrix = m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ);
	_vector vColor = DirectX::Colors::GhostWhite;

	switch (m_eBodyType)
	{
	case ACTOR::BOX:
	{
		// vVolume = half-extents(Hx,Hy,Hz) 라고 가정
		const _matrix ScaleMatrix =
			XMMatrixScaling(vVolume.x / 0.5f,
				vVolume.y / 0.5f,
				vVolume.z / 0.5f);
		m_pMainShape->Draw(ScaleMatrix * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
	} break;
	case ACTOR::CAPSULE:
	{
		_float fHalf = vVolume.y;

		_matrix ScaleMatrix = XMMatrixScaling(vVolume.x / 0.5f,
												fHalf / (2.f * 0.5f),
												vVolume.x / 0.5f);
		m_pMainShape->Draw(ScaleMatrix * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);

		_matrix ScaleSphereMatrix = XMMatrixScaling(vVolume.x / 0.5f, vVolume.x / 0.5f, vVolume.x / 0.5f);

		_matrix WorldUp = XMMatrixTranslation(0.f, +fHalf * 0.5f, 0.f);
		_matrix WorldDown = XMMatrixTranslation(0.f, -fHalf * 0.5f, 0.f);

		m_pSubShape->Draw(ScaleSphereMatrix * WorldUp * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
		m_pSubShape->Draw(ScaleSphereMatrix * WorldDown * WorldMatrix, ViewMatrix, ProjMatrix, vColor, nullptr, true);
	} break;
	default:
		break;
	}
	return S_OK;
}
#endif // _DEBUG
PSX::PxRigidDynamic* CCharacter_Controller::Get_Actor()
{
	return m_pController->getActor();
}

PSX::PxController* CCharacter_Controller::Get_Controller()
{
	return m_pController;
}

PSX::PxControllerCollisionFlags CCharacter_Controller::Get_CollisionFlags()
{
	return m_eBeforeCollisionFlags;
}

void CCharacter_Controller::Resize_Volume(_float fHeight)
{
	m_pController->resize(fHeight);
}

void CCharacter_Controller::Modify_Volume(_float3 fVolume)
{
	switch (m_eBodyType)
	{
	case Engine::ACTOR::BOX:
	{
		PSX::PxBoxController* pBox = static_cast<PSX::PxBoxController*>(m_pController);
		if (0 != fVolume.x) {
			pBox->setHalfSideExtent(fVolume.x);
		}
		if (0 != fVolume.y) {
			pBox->setHalfHeight(fVolume.y);
		}
		if (0 != fVolume.z) {
			pBox->setHalfForwardExtent(fVolume.z);
		}
	} break;
	case Engine::ACTOR::CAPSULE:
	{
		PSX::PxCapsuleController* pCapsule = static_cast<PSX::PxCapsuleController*>(m_pController);
		if (0 != fVolume.x) {
			pCapsule->setRadius(fVolume.x);
		}
		if (0 != fVolume.y) {
			pCapsule->setHeight(fVolume.y);
		}
	} break;
	default:
		break;
	}
	// Overlap();
}

_float3 CCharacter_Controller::Get_Volume()
{
	switch (m_eBodyType)
	{
	case Engine::ACTOR::BOX:
	{
		PSX::PxBoxController* pBox = static_cast<PSX::PxBoxController*>(m_pController);
		return {
			pBox->getHalfSideExtent(),
			pBox->getHalfHeight(),
			pBox->getHalfForwardExtent()
		};
	} break;
	case Engine::ACTOR::CAPSULE:
	{
		PSX::PxCapsuleController* pCapsule = static_cast<PSX::PxCapsuleController*>(m_pController);
		return {
			pCapsule->getRadius(),
			pCapsule->getHeight(),
			0.f
		};
	} break;
	default:
		return { -1.f, -1.f, -1.f };
		break;
	}
}

HRESULT CCharacter_Controller::ConvertToDO(CRigidBody_Dynamic& BodyOriginal)
{	
	PSX::PxRigidDynamic* pDOActor = BodyOriginal.Get_Actor();
	PSX::PxRigidDynamic* pCCTActor = m_pController->getActor();

	m_bActive = false;
	BodyOriginal.SetActive(true);

	m_pGameInstance->Attach_Actor(*pDOActor); // DO 액터 활성화

	PSX::PxTransform pxTransform = pCCTActor->getGlobalPose(); 
	pDOActor->setGlobalPose(pxTransform);

	return S_OK;
}

void CCharacter_Controller::Set_OnGroundFlag(_bool bOnGround)
{
	m_iIsOnGround = iCoyote_Counter;
}

void CCharacter_Controller::Set_CurrentSlope(_float fSlope)
{
	m_fCurrentSlopeDegree = fSlope;
}

void CCharacter_Controller::Rewind_Grounded()
{
	if (m_eBeforeCollisionFlags.isSet(PSX::PxControllerCollisionFlag::eCOLLISION_DOWN)) {
		m_iIsOnGround = iCoyote_Counter;
		return;
	}
	m_iIsOnGround -= 1;
	if (0 > m_iIsOnGround) {
		m_iIsOnGround = 0;
	}
}

_bool CCharacter_Controller::UpdateGroundByCast(_float fTimeDelta)
{
	_vector vFoot = Get_FootPosition();
	_float3 fFoot;
	XMStoreFloat3(&fFoot, vFoot);
	fFoot.y += 0.0625f;

	PSX::PxSweepBuffer pxBuffer = {};

	m_pController->getFootPosition();
	_bool bHit = { false };

	bHit = m_pGameInstance->SphereCast(0.0625f, fFoot, { 0.f, -1.f, 0.f }, 0.0625f, PSX::PxHitFlag::ePOSITION | PSX::PxHitFlag::eNORMAL, PSX::PxQueryFlag::eSTATIC, pxBuffer);

	if (true == bHit) { // 쿼리해서 스태틱 중에 터레인 액터들이 맞으면 다시 갱신
		const PSX::PxSweepHit& hit = pxBuffer.block;
		
		PSX::PxRigidActor* pActor = hit.actor;
		PSX::PxShape* pShape = hit.shape;

		if (nullptr != pActor && nullptr != pActor->userData)
		{
			PhsXUserData* pUserData = static_cast<PhsXUserData*>(pActor->userData);

			switch (pUserData->eKind)
			{
			case PHYSX_KIND::BODY_STATIC:
			{
				switch (PXOBJECT(pUserData->iSubKind))
				{
				case PXOBJECT::TERRAIN:
				case PXOBJECT::FLOOR:
				case PXOBJECT::ROAD:
				{ // 임계값 이상이면 지면에 서서히 붙임
					_float fYHitPos = hit.position.y;
					_float fYFoot = (_float)m_pController->getFootPosition().y;
					_float fDiff = (fYHitPos - fYFoot);
					if (fDiff > m_vAccHeight.w) {
						_vector vMomentum = m_pTransform->Get_CurrentMomentum();
						_float fYCurrentMomentum = XMVectorGetY(vMomentum);
						fYCurrentMomentum = fDiff * 0.3f;
						m_pTransform->Set_CurrentMomentum(XMVectorSetY(vMomentum, fYCurrentMomentum));
					}
					else {
						m_iIsOnGround = iCoyote_Counter;
					}
				}
					break;
				default:
					break;
				}
			} break;
			case PHYSX_KIND::BODY_DYNAMIC:
				break;
			case PHYSX_KIND::CCTActor:
			case PHYSX_KIND::OBSTACLEActor:
				break;
			default:
				break;
			}
		}
	}

	return bHit;
}

void CCharacter_Controller::Move(_float fTimeDelta)
{
	PSX::PxVec3						pxVecMomentum = {};					// 순간 이동량
	_float							fMinimumDistant = FLT_EPSILON3;		// 이동량 오차 허용치, ( 크면 클수록 이동이 더 일찍 끝난다, 순간 이동량보다 같거나 더 크면 안움직일듯? )
	PSX::PxControllerFilters		pxFilter = {};						// 충돌 대상 필터
	const PSX::PxObstacleContext*	pPxObstacles = { nullptr };			// 캐릭터가 충돌해야할 추가적인 장애물 객체?, 닿은 장애물은 캐시된다?
	if (true == m_bGravity) {
		if (0 >= m_iIsOnGround) {      // 공중: 무조건 중력
			m_pTransform->AccumulateMomentum(XMVectorSet(0.f, -GRAVITY * m_fGravity * fTimeDelta, 0.f, 0.f));
		}
		else if ((iCoyote_Counter) >= m_iIsOnGround) {
			UpdateGroundByCast(fTimeDelta); // 바닥에 플레이어 붙이게 함
		}
		else if (m_fCurrentSlopeDegree > m_fWalkableSlopeDegree) {        // 너무 가파른 경사: 중력
			//m_pTransform->AccumulateMomentum(XMVectorSet(0.f, -GRAVITY * fTimeDelta, 0.f, 0.f));
		}
		else {
			// 아니면 안함
		}
	}
	XMStoreFloat3((_float3*)&pxVecMomentum, m_pTransform->Get_CurrentMomentum());
	
	m_eBeforeCollisionFlags = m_pController->move(pxVecMomentum, fMinimumDistant, fTimeDelta, pxFilter, pPxObstacles);
}

void CCharacter_Controller::Set_Position(_fvector vNewPos)
{
	PSX::PxExtendedVec3 pxLVec3NewPos = {};
	pxLVec3NewPos.x = vNewPos.m128_f32[0];
	pxLVec3NewPos.y = vNewPos.m128_f32[1];
	pxLVec3NewPos.z = vNewPos.m128_f32[2];
	m_pController->setPosition(pxLVec3NewPos);
}

_vector CCharacter_Controller::Get_Position()
{
	PSX::PxExtendedVec3 pxVPos = m_pController->getPosition();
	return XMVectorSet((_float)pxVPos.x, (_float)pxVPos.y, (_float)pxVPos.z, 1.f);
}

_vector CCharacter_Controller::Get_FootPosition()
{
	PSX::PxExtendedVec3  pxLVecfootPos = m_pController->getFootPosition();
	return XMVectorSet((_float)pxLVecfootPos.x, (_float)pxLVecfootPos.y, (_float)pxLVecfootPos.z, 1.f );
}

HRESULT CCharacter_Controller::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCharacter_Controller::Initialize(void* pArg)
{
	Character_Controller_DESC* pDesc = static_cast<Character_Controller_DESC*>(pArg);
	{ // Character_Controller_DESC
		m_eBodyType = pDesc->eBodyType;
		m_bAutoStepping = pDesc->bAutoStepping;
		m_pTransform = pDesc->pTransform;
		m_fWalkableSlopeDegree = pDesc->fWalkableSlope;
	}
	{ // PhsXUserData
		m_tagData.eKind = PHYSX_KIND::CCTActor;
		m_tagData.pOwner = m_pOwner;
		XMStoreFloat4x4(&m_tagData.BeforeMatrix, m_pTransform->Get_XMWorldMatrix());
		m_tagData.pCharacter = this;
		m_tagData.iSubKind = pDesc->iSubKind;
	}

	switch (m_eBodyType)
	{
	case ACTOR::BOX:
	{
		PSX::PxBoxControllerDesc Desc{};
		Desc.halfHeight			= pDesc->vBoxSize.y;
		Desc.halfSideExtent		= pDesc->vBoxSize.x;
		Desc.halfForwardExtent	= pDesc->vBoxSize.z;
		Desc.contactOffset		= pDesc->fContactOffset;
		Desc.reportCallback		= pDesc->pCallback_HitReport;
		Desc.behaviorCallback	= pDesc->pCallback_Behavior;
		Desc.slopeLimit			= cosf(XMConvertToRadians(m_fWalkableSlopeDegree));
		Desc.nonWalkableMode	= PSX::PxControllerNonWalkableMode::ePREVENT_CLIMBING;
		Desc.material			= m_pGameInstance->Create_Material(&pDesc->fMaterial);
		m_pController			= m_pGameInstance->Add_BoxController(Desc);
		m_pController->setUserData(&m_tagData);
		m_pController->getActor()->userData = &m_tagData;
	} break;
	case ACTOR::CAPSULE:
	{
		PSX::PxCapsuleControllerDesc Desc{};
		Desc.radius				= pDesc->fRadius;
		Desc.height				= pDesc->fHeight;
		Desc.climbingMode		= pDesc->eClimbingMode; // 기본 eEASY
		Desc.contactOffset		= pDesc->fContactOffset;
		Desc.reportCallback		= pDesc->pCallback_HitReport;
		Desc.behaviorCallback	= pDesc->pCallback_Behavior;
		Desc.slopeLimit			= cosf(XMConvertToRadians(m_fWalkableSlopeDegree));
		Desc.nonWalkableMode	= PSX::PxControllerNonWalkableMode::ePREVENT_CLIMBING;
		Desc.material			= m_pGameInstance->Create_Material(&pDesc->fMaterial);
		m_pController			= m_pGameInstance->Add_CapsuleController(Desc);
		m_pController->setUserData(&m_tagData);
		m_pController->getActor()->userData = &m_tagData;
	} break;
	default:
		assert(false); // PhysX에서 불가능
		return E_FAIL;
		break;
	}
	
	if (nullptr == m_pController) {
		assert(false);
		return E_FAIL;
	}

#ifdef _DEBUG
	if (FAILED(Debug_Initialize())) {
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

#ifdef _DEBUG
HRESULT CCharacter_Controller::Debug_Initialize()
{
	_float3 vVolume = Get_Volume();
	switch (m_eBodyType)
	{
	case ACTOR::BOX:
		m_pMainShape = (GeometricPrimitive::CreateBox(m_pContext, vVolume, false, false));
		break;
	case ACTOR::CAPSULE:
		m_pMainShape = (GeometricPrimitive::CreateCylinder(m_pContext, vVolume.y, vVolume.x, 10, false));
		m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, vVolume.x, 10, false, false));
		break;
	default:
		break;
	}
	return S_OK;
}
#endif // _DEBUG


_bool CCharacter_Controller::Check_Overlap()
{
	return _bool();
}

CCharacter_Controller* CCharacter_Controller::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCharacter_Controller* pInstance = new CCharacter_Controller(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCharacter_Controller");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CComponent* CCharacter_Controller::Clone(void* pArg, class CGameObject* pOwner)
{
	CCharacter_Controller* pInstance = new CCharacter_Controller(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCharacter_Controller");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCharacter_Controller::Free()
{
	if (nullptr != m_pController) {
		m_pController->release();
		m_pController = nullptr;
	}
	__super::Free();
}
#ifdef _DEBUG
void CCharacter_Controller::Describe_Entity()
{
	GUI::Begin("Character_Controller");
	GUI::Text("Gravity : %d ", m_bGravity);
	m_pTransform->Describe_Entity();
	GUI::TextLinkOpenURL("More details here", "https://dev-treadmill.tistory.com/158");
	_float fContact = m_pController->getContactOffset();
	PSX::PxVec3 vUpDirection = m_pController->getUpDirection();
	PSX::PxExtendedVec3 vCenterPosition = m_pController->getPosition();
	PSX::PxExtendedVec3 vFootPosition = m_pController->getFootPosition();
	_float3 vVolume = Get_Volume();
	_float fStepOffset = m_pController->getStepOffset();

	GUI::Text("eBodyType $d", (_uint)m_eBodyType); GUIHelpMarker("Body: only box and capsule available");

	if (GUI::SliderFloat3("UpDirection", (_float*)&vUpDirection, -1.f, 1.f)) {
		m_pController->setUpDirection(vUpDirection.getNormalized());
	} GUIHelpMarker("Up vector in PhysX");
	GUI::Text("vCenterPosition %lf, %lf, %lf", vCenterPosition.x, vCenterPosition.y, vCenterPosition.z); GUIHelpMarker("Exact center of the rigid body owned by the controller");
	GUI::Text("vFootPosition %lf, %lf, %lf", vFootPosition.x, vFootPosition.y, vFootPosition.z); GUIHelpMarker("Foot position including step offset from the body center to the ground");

	if (GUI::SliderFloat("ContactOffset", &fContact, 0.01f, 10.f, "%.2f")) {
		m_pController->setContactOffset(fContact);
	} GUIHelpMarker("Actual contact offset in PhysX; increasing it may make the controller appear to hover");
	if (GUI::SliderFloat("StepOffset", &fStepOffset, 0.f, 3.f, "%.2f")) {
		m_pController->setStepOffset(fStepOffset);
	} GUIHelpMarker("In stepped regions, the tolerance height the controller can climb.\n Capsule behaves slightly differently.");
	
	if (ACTOR::CAPSULE == m_eBodyType) {
		PSX::PxCapsuleController* pController = (PSX::PxCapsuleController*)m_pController;

		if (0.f != fStepOffset) {
			switch (pController->getClimbingMode())
			{
			case PSX::PxCapsuleClimbingMode::eEASY:
				if (GUI::Button("ClimbingMode : eEasy")) {
					pController->setClimbingMode(PSX::PxCapsuleClimbingMode::eCONSTRAINED);
				}
				break;
			case PSX::PxCapsuleClimbingMode::eCONSTRAINED:
				if (GUI::Button("ClimbingMode : eConstrained")) {
					pController->setClimbingMode(PSX::PxCapsuleClimbingMode::eEASY);
				}
				break;
			default:
				break;
			}
			GUIHelpMarker("A capsule is affected by its lower sphere and tends to generate an up vector on steps.\nIn eEASY, the up vector is combined with the step offset, allowing easier climbing; in eCONSTRAINED, the up vector is removed during step detection and only the step offset is used.");
		}
	}
	GUI::Text("%.1f", m_fCurrentSlopeDegree);
	GUI::Text("%.1f", m_fWalkableSlopeDegree); GUI::SameLine(); if (GUI::SliderFloat("m_fWalkableSlopeDegree", &m_fWalkableSlopeDegree, 0.0f, 90.f)) {
		m_pController->setSlopeLimit(cosf(XMConvertToRadians(m_fWalkableSlopeDegree)));
	}
	GUI::DragFloat("m_vAccHeight.w", &m_vAccHeight.w, 0.01f, 0.f, 1.f);
	GUI::SliderFloat("m_fGravity_Multiplier", &m_fGravity, 0.0f, 3.f);
	if (GUI::SliderFloat3("Volume", (_float*)&vVolume, 0.1f, 10.f, "%.2f")) {
		Modify_Volume(vVolume);
	}GUIHelpMarker("Box uses vSize; Capsule uses x->fRadius, y->fHeight.\nThe capsule's fHeight is likely the distance from the lower sphere center to the upper sphere center.");

	GUI::End();
}

#endif // _DEBUG
