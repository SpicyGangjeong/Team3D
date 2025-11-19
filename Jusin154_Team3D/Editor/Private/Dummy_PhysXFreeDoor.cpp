#include "pch.h"
#include "Dummy_PhysXFreeDoor.h"

#include "GameInstance.h"


CDummy_PhysXFreeDoor::CDummy_PhysXFreeDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CDummy_PhysXFreeDoor::CDummy_PhysXFreeDoor(const CDummy_PhysXFreeDoor& rhs)
	: CGameObject(rhs)
{
}

HRESULT CDummy_PhysXFreeDoor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummy_PhysXFreeDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

void CDummy_PhysXFreeDoor::Priority_Update(_float fTimeDelta)
{
	//GUI::Text("%f %f %f", m_vKinematicTimer.x, m_vKinematicTimer.y, m_vKinematicTimer.z);
	//GUI::Text("%f", ((m_vKinematicTimer.x - m_vKinematicTimer.y) / (m_vKinematicTimer.z - m_vKinematicTimer.y)));

	if (m_pActor->getRigidBodyFlags() & PSX::PxRigidBodyFlag::eKINEMATIC) {
		m_vKinematicTimer.x += fTimeDelta;
		if (m_vKinematicTimer.x < m_vKinematicTimer.z) {
			// 행렬없이 럴프 버전
			_float fTime = max(0.f, ((m_vKinematicTimer.x - m_vKinematicTimer.y) / (m_vKinematicTimer.z - m_vKinematicTimer.y)));
			Lerp_Matrix(fTime);
			//Lerp_NonMatrix(fTime);
			GUI::Text("2");
		}
		else {
			m_pActor->setRigidBodyFlag(PSX::PxRigidBodyFlag::eKINEMATIC, false);
			GUI::Text("3");
		}
	}
}

void CDummy_PhysXFreeDoor::Lerp_Matrix(Engine::_float fTime)
{
	{
		_float4x4 OutMatrix = {};
		_float4x4 CurrentMatrix = {};
		XMStoreFloat4x4(&CurrentMatrix, m_pTransformCom->Get_XMWorldMatrix());
		CMyTools::MatrixLerp(&CurrentMatrix, &m_InitialMatrix, OutMatrix, fTime);
		m_pTransformCom->Set_WorldMatrix(OutMatrix);
	}
}

void CDummy_PhysXFreeDoor::Lerp_NonMatrix(Engine::_float fTime)
{
	{
		_vector vCurrQ = m_pTransformCom->Get_QuarternionVector();
		_vector vInitialQ = XMLoadFloat4((_float4*)&m_pxStartQuat);
		_vector vLerpQuat = XMVector4Normalize(XMVectorLerp(vCurrQ, vInitialQ, fTime));

		PSX::PxTransform actorPose = m_pActor->getGlobalPose();
		PSX::PxTransform massLocalPose = m_pActor->getCMassLocalPose();

		PSX::PxVec3 vpxMassLocal = massLocalPose.p;
		PSX::PxVec3 vpxMassWorld = actorPose.transform(vpxMassLocal);

		_vector vMassLocal = XMVectorSet(vpxMassLocal.x, vpxMassLocal.y, vpxMassLocal.z, 0.f);
		_vector vMassWorld = XMVectorSet(vpxMassWorld.x, vpxMassWorld.y, vpxMassWorld.z, 1.f);


		_float3 vScale = m_pTransformCom->Get_Scale();
		_matrix ScaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&vScale));
		_matrix RotationMatrix = XMMatrixRotationQuaternion(vLerpQuat);

		_matrix TranslationLocal = XMMatrixTranslationFromVector(-vMassLocal);
		_matrix TranslationWorld = XMMatrixTranslationFromVector(vMassWorld);

		// 최종 월드 행렬
		_matrix world = TranslationLocal // 피벗중심으로 좌표계를 이동시킴
			* (ScaleMatrix * RotationMatrix * TranslationWorld); // 그 후 SRT 적용

		m_pTransformCom->Set_WorldMatrix(world);
	}
}

void CDummy_PhysXFreeDoor::Update(_float fTimeDelta)
{
}

void CDummy_PhysXFreeDoor::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	}
}

HRESULT CDummy_PhysXFreeDoor::Render()
{
	_vector vRPY = m_pTransformCom->Get_RollPitchYawVector();
	if (!(m_pActor->getRigidBodyFlags() & PSX::PxRigidBodyFlag::eKINEMATIC)) {
		_float fPitch = XMVectorGetX(vRPY);
		_float fYaw = XMVectorGetY(vRPY);
		_float fRoll = XMVectorGetZ(vRPY);

		_float fClampedYaw = ClampRadian(fYaw);
		if (fClampedYaw != fYaw) {
			_vector vNewQuat = XMQuaternionRotationRollPitchYaw(fPitch, fClampedYaw, fRoll);

			PSX::PxTransform actorPose = m_pActor->getGlobalPose();
			PSX::PxTransform massLocalPose = m_pActor->getCMassLocalPose();

			PSX::PxVec3 vpxMassLocal = massLocalPose.p;
			PSX::PxVec3 vpxMassWorld = actorPose.transform(vpxMassLocal);

			_vector vMassLocal = XMVectorSet(vpxMassLocal.x, vpxMassLocal.y, vpxMassLocal.z, 0.f);
			_vector vMassWorld = XMVectorSet(vpxMassWorld.x, vpxMassWorld.y, vpxMassWorld.z, 1.f);

			_float3 vScale = m_pTransformCom->Get_Scale();
			_matrix ScaleMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&vScale));
			_matrix RotationMatrix = XMMatrixRotationQuaternion(vNewQuat);

			_matrix TranslationLocal = XMMatrixTranslationFromVector(-vMassLocal);
			_matrix TranslationWorld = XMMatrixTranslationFromVector(vMassWorld);

			// 최종 월드 행렬
			_matrix world = TranslationLocal // 피벗중심으로 좌표계를 이동시킴
				* (ScaleMatrix * RotationMatrix * TranslationWorld); // 그 후 SRT 적용

			m_pTransformCom->Set_WorldMatrix(world);
			m_pActor->setGlobalPose(XMWorldToPx_NoScale(world));

			m_pActor->setAngularVelocity(PSX::PxVec3(0));
			m_pActor->setLinearVelocity(PSX::PxVec3(0));
			m_pActor->setRigidBodyFlag(PSX::PxRigidBodyFlag::eKINEMATIC, true);
			m_vKinematicTimer.x = 0.f;
		}
	}

	vRPY = m_pTransformCom->Get_RollPitchYawVector(); // 갱신된 값으로 다시 읽기
	GUI::Begin("Door");
	m_pTransformCom->Describe_Entity();
	GUI::Text("Current Roll : %f", XMConvertToDegrees(XMVectorGetZ(vRPY)));
	GUI::Text("Current Pitch : %f", XMConvertToDegrees(XMVectorGetX(vRPY)));
	GUI::Text("Current Yaw : %f", XMConvertToDegrees(XMVectorGetY(vRPY)));
	GUI::Text("Current Limit Left : %f", XMConvertToDegrees(m_vRadianYAngle.x));
	GUI::Text("Current Limit Right : %f", XMConvertToDegrees(m_vRadianYAngle.z));
	GUI::End();
#ifdef _DEBUG
	if (FAILED(m_pRigidBody->Render())) {
		return E_FAIL;
	}
#endif // _DEBUG


	return S_OK;
}

HRESULT CDummy_PhysXFreeDoor::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}
	PHYSXDUMMY_DESC* pPhysXDummyDesc = static_cast<PHYSXDUMMY_DESC*>(pArg);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pPhysXDummyDesc->vPos), 1.f));
	m_pTransformCom->Rotation(pPhysXDummyDesc->vRotRPY.x, pPhysXDummyDesc->vRotRPY.y, pPhysXDummyDesc->vRotRPY.z);

	XMStoreFloat4((_float4*)&m_pxStartQuat, m_pTransformCom->Get_QuarternionVector());
	XMStoreFloat4x4(&m_InitialMatrix, m_pTransformCom->Get_XMWorldMatrix());

	m_vRadianYAngle.y = pPhysXDummyDesc->vRotRPY.z;
	m_vRadianYAngle.x = m_vRadianYAngle.y + XMConvertToRadians(-pPhysXDummyDesc->fAngleLimit);
	m_vRadianYAngle.z = m_vRadianYAngle.y + XMConvertToRadians(pPhysXDummyDesc->fAngleLimit);

	{ // RIGID_BODY
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = pPhysXDummyDesc->iSubKind;
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_DOOR"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
	}
	m_pActor = m_pRigidBody->Get_Actor();
	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}

	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Box"), (CComponent**)&m_pModelCom))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDummy_PhysXFreeDoor::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	return S_OK;
}

_float CDummy_PhysXFreeDoor::ClampRadian(_float fNewRadian)
{
	// [x, z] 사이로 잘라주는 단순한 클램프
	_float minYaw = m_vRadianYAngle.x;
	_float maxYaw = m_vRadianYAngle.z;

	// 라디안 노멀라이즈 ([-pi, pi] 같은 범위로)
	fNewRadian = CMyTools::NormalizeRadian(fNewRadian);

	if (fNewRadian < minYaw) {
		fNewRadian = minYaw;
	}
	else if (fNewRadian > maxYaw) {
		fNewRadian = maxYaw;
	}
	return fNewRadian;
}

CDummy_PhysXFreeDoor* CDummy_PhysXFreeDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummy_PhysXFreeDoor* pInstance = new CDummy_PhysXFreeDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummy_PhysXFreeDoor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummy_PhysXFreeDoor::Clone(void* pArg, CGameObject* pOwner)
{
	CDummy_PhysXFreeDoor* pInstance = new CDummy_PhysXFreeDoor(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummy_PhysXFreeDoor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummy_PhysXFreeDoor::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);
}

void CDummy_PhysXFreeDoor::Describe_Entity()
{
}
