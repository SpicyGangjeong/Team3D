#include "pch.h"
#include "MapElement_Door.h"

#include "GameInstance.h"
#include "RigidBody_Dynamic.h"
#include "Terrain.h"
#include "Layer.h"
#include "VIBuffer_Terrain.h"

CMapElement_Door::CMapElement_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Door::CMapElement_Door(const CMapElement_Door& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Door::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Door::Initialize(void* pArg)
{
	ELEMENT_DOOR_DESC* pDesc = static_cast<ELEMENT_DOOR_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
		//m_ModelPathIndices.push_back((*pDesc->pModelPathIndices)[i]);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	XMStoreFloat4((_float4*)&m_pxStartQuat, m_pTransformCom->Get_QuarternionVector());
	XMStoreFloat4x4(&m_InitialMatrix, m_pTransformCom->Get_XMWorldMatrix());

	m_vRadianYAngle.y = pDesc->vRotation.z;
	m_vRadianYAngle.x = m_vRadianYAngle.y + XMConvertToRadians(-70.f);
	m_vRadianYAngle.z = m_vRadianYAngle.y + XMConvertToRadians(70.f);

	{ // RIGID_BODY
		CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
		Desc.iSubKind = ENUM_CLASS(PXOBJECT::DOOR);
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_DOOR"), (CComponent**)&m_pRigidBody, &Desc))) {
			return E_FAIL;
		}
	}
	m_pActor = (PSX::PxRigidDynamic*)m_pRigidBody->Get_Actor();

	return S_OK;
}

void CMapElement_Door::Priority_Update(_float fTimeDelta)
{
	if (m_pActor->getRigidBodyFlags() & PSX::PxRigidBodyFlag::eKINEMATIC) {
		m_vKinematicTimer.x += fTimeDelta;
		if (m_vKinematicTimer.x < m_vKinematicTimer.z) {
			// 행렬없이 럴프 버전
			_float fTime = max(0.f, ((m_vKinematicTimer.x - m_vKinematicTimer.y) / (m_vKinematicTimer.z - m_vKinematicTimer.y)));
			Lerp_Matrix(fTime);
			//Lerp_NonMatrix(fTime);
		}
		else {
			m_pActor->setRigidBodyFlag(PSX::PxRigidBodyFlag::eKINEMATIC, false);
		}
	}
}

void CMapElement_Door::Update(_float fTimeDelta)
{
}

void CMapElement_Door::Late_Update(_float fTimeDelta)
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
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pModelComs[0]->Get_Radius())) {
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

HRESULT CMapElement_Door::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}
		
		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	//if (FAILED(m_pRigidBody->Render())) {
	//	return E_FAIL;
	//}
#endif // _DEBUG

	return S_OK;
}

HRESULT CMapElement_Door::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	for (_uint i = 0; i < m_iMaxLodLevel + 1; ++i)
	{
		CModel* pModel = { nullptr };

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(NEXT_LEVEL, m_ModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pModelComs.push_back(pModel);
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;


	ELEMENT_DOOR_DESC* pPhysXDummyDesc = static_cast<ELEMENT_DOOR_DESC*>(pArg);

	return S_OK;
}

HRESULT CMapElement_Door::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
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

void CMapElement_Door::Lerp_Matrix(_float fTime)
{
	{
		_float4x4 OutMatrix = {};
		_float4x4 CurrentMatrix = {};
		XMStoreFloat4x4(&CurrentMatrix, m_pTransformCom->Get_XMWorldMatrix());
		CMyTools::MatrixLerp(&CurrentMatrix, &m_InitialMatrix, OutMatrix, fTime);
		m_pTransformCom->Set_WorldMatrix(OutMatrix);
	}
}

void CMapElement_Door::Lerp_NonMatrix(_float fTime)
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

_float CMapElement_Door::ClampRadian(_float fNewRadian)
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

CMapElement_Door* CMapElement_Door::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Door* pInstance = new CMapElement_Door(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Door");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Door::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Door* pInstance = new CMapElement_Door(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Door");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Door::Free()
{
	if (nullptr != m_pRigidBody) {
		m_pGameInstance->Release_Actor(*m_pRigidBody->Get_Actor());
	}
	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

#ifdef _DEBUG
void CMapElement_Door::Describe_Entity()
{

}
#endif // _DEBUG


