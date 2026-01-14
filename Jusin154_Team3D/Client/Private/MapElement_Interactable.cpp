#include "pch.h"
#include "MapElement_Interactable.h"

#include "InfoInstance.h"
#include "GameInstance.h"
#include "EffectPool.h"
#include "Layer.h"

CMapElement_Interactable::CMapElement_Interactable(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Interactable::CMapElement_Interactable(const CMapElement_Interactable& rhs)
	: CMapElement(rhs),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

HRESULT CMapElement_Interactable::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Interactable::Initialize(void* pArg)
{
	ELEMENT_INTERACTABLE_DESC* pDesc = static_cast<ELEMENT_INTERACTABLE_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags[i]);
		//m_ModelPathIndices.push_back((*pDesc->pModelPathIndices)[i]);
	}
	m_isPooled = pDesc->isPooled;
	m_eInteractableID = static_cast<ELEMENT_INTERACTABLE_ID>(pDesc->iInteractableID);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	ELEMENT_INTERACTABLE_DESC* pPhysXDummyDesc = static_cast<ELEMENT_INTERACTABLE_DESC*>(pArg);

	// RIGID_BODY
	CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
	Desc.iSubKind = ENUM_CLASS(PXOBJECT::BOX);
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_THROWABLE_BOX"), (CComponent**)&m_pRigidBody, &Desc))) {
		return E_FAIL;
	}

	m_pActor = static_cast<PSX::PxRigidDynamic*>(m_pRigidBody->Get_Actor());

	static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Set_HalfGeometryInfo(pDesc->vBoxSize);
	static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Move_LocalPos(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat3(&pDesc->vBoxLocalPosition));
	//static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Set_Kinematic(false);

	if (true == m_isPooled)
	{
		m_bVisible = false;
		static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Set_Kinematic(true);
	}
	else
	{
		m_bVisible = true;
		m_pInfoInstance->Regist_ActiveInteractive(this);
	}


	m_pEffectPool = m_pGameInstance->Get_Layer(NEXT_LEVEL, TEXT("Layer_EffectPool"))->Get_Object<CEffectPool>();
	SAFE_ADDREF(m_pEffectPool);

	return S_OK;
}

void CMapElement_Interactable::Priority_Update(_float fTimeDelta)
{
	if(m_bVisible)
	{
		_float fRadius = m_pModelComs[0]->Get_Radius();

		XMStoreFloat4(&m_vStartPos, Get_WorldPostion() + XMVectorSet(0.f, fRadius, 0.f, 0.f));
	}

}

void CMapElement_Interactable::Update(_float fTimeDelta)
{

}

void CMapElement_Interactable::Late_Update(_float fTimeDelta)
{
	if (m_bVisible)
	{
		if (m_bThrow == true)
		{
			_float fRadius = m_pModelComs[0]->Get_Radius();

			if (false == m_bHit) {
				_vector vStartPos = XMLoadFloat4(&m_vStartPos);
				_vector vEndPos = Get_WorldPostion() + XMVectorSet(0.f, fRadius, 0.f, 0.f);
				ON_COLLISION_INFO CollisionInfo = CollisionCheck(vStartPos, vEndPos, 1.f);

				if (m_bHit == true)
					OnCollision(this, &CollisionInfo);
			}
		}

		if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pModelComs[0]->Get_Radius())) {
			m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
		}
	}
}

HRESULT CMapElement_Interactable::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	_uint iShaderPass = ENUM_CLASS(SHADER_PASS_MESH::DEFAULT);
	if (true == m_bDrawOutLine) {
		iShaderPass = ENUM_CLASS(SHADER_PASS_MESH::OUTLINE_WRITE);
	}
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}
		
		if (FAILED(m_pShaderCom->Begin(iShaderPass))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

	if (m_bDrawOutLine) {
		Render_OutLine();
	}
#ifdef _DEBUG
	if (FAILED(m_pRigidBody->Render())) {
		return E_FAIL;
	}
#endif // _DEBUG

	return S_OK;
}

void CMapElement_Interactable::GrapToPlayer(_fvector vPos, _float fRatio)
{
	_vector vCurrentPosition = m_pTransformCom->Get_State(STATE::POSITION);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorLerp(vCurrentPosition, vPos, fRatio));

	m_isGraped = true;

}

void CMapElement_Interactable::OnCollision(CGameObject* pOther, void* pDesc)
{
	if (m_bThrow == false)
		return;

	if (m_bHit == false)
		return;

	ON_COLLISION_INFO CollisionDesc = *static_cast<ON_COLLISION_INFO*>(pDesc);

	_vector vPos = XMLoadFloat4(&CollisionDesc.vWorldPos);

	switch (m_eInteractableID)
	{
	case Client::ELEMENT_INTERACTABLE_ID::BARREL:

		m_pEffectPool->Use_Skill(SKILL_TYPE::BARRAL_SPLASH, this);
		break;
	case Client::ELEMENT_INTERACTABLE_ID::BOX:

		m_pEffectPool->Use_Skill(SKILL_TYPE::BOX_SPLESH, this);
		break;
	case Client::ELEMENT_INTERACTABLE_ID::TABLE:
		m_pEffectPool->Use_Skill(SKILL_TYPE::CHAIL_SPLESH, this);
		break;
	case Client::ELEMENT_INTERACTABLE_ID::CHAIR:
		m_pEffectPool->Use_Skill(SKILL_TYPE::CHAIL_SPLESH, this);
		break;
	case Client::ELEMENT_INTERACTABLE_ID::END:
		break;
	default:
		break;
	}

	if (m_isPooled)
	{
		m_bVisible = false;
		static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Set_Kinematic(true);
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, -999.f, 0.f, 1.f));
		m_pInfoInstance->Deregist_ActiveInteractive(this);
	}
	else{
		m_pInfoInstance->Deregist_ActiveInteractive(this);
		Set_Dead();
	}
}

HRESULT CMapElement_Interactable::Ready_Components(void* pArg)
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
		reinterpret_cast<CComponent**>(&m_pShaderCom)))){
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CMapElement_Interactable::Bind_ShaderResources()
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

_vector CMapElement_Interactable::Get_LockOnPos()
{
	return m_pRigidBody->Get_Position();
}

void CMapElement_Interactable::Set_DrawOutLine()
{
	m_bDrawOutLine = true;
}

HRESULT CMapElement_Interactable::Render_OutLine()
{
	m_bDrawOutLine = false;
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	Compute_Depth();
	_float fCamFar = *m_pGameInstance->Get_CurrentCameraFar();
	_float fRatio = CMyTools::Saturate((m_fCamDepth / (fCamFar * fCamFar)));
	m_fOutLineThickness = CMyTools::Lerp_f1D(0.8f, 1.2f, fRatio);
	if (m_fOutLineThickness > 0.023f) {
		m_fOutLineThickness = 0.023f;
	}
	else if (m_fOutLineThickness < 0.013f) {
		m_fOutLineThickness = 0.013f;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vOutLineColor", &m_vOutLineColor, sizeof(_float3)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineThickness", &m_fOutLineThickness, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLineScale", &m_fOutLineScale, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOutLinePower", &m_fOutLinePower, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
		return E_FAIL;
	}
	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::OUTLINE_READ)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

void CMapElement_Interactable::Set_KinematicFlag(_bool bFlag)
{
	m_pRigidBody->Set_Kinematic(bFlag);
}

void CMapElement_Interactable::ActivateAt(_fvector vPos)
{
	m_bVisible = true;
	m_pTransformCom->Set_State(STATE::POSITION, vPos);
	m_pInfoInstance->Regist_ActiveInteractive(this);
	m_pRigidBody->Set_Kinematic(false);
	m_pRigidBody->Set_Position(vPos, true);
}

ON_COLLISION_INFO CMapElement_Interactable::CollisionCheck(_fvector StartPos, _fvector EndPos, _float fRadius)
{

	_vector vStartPos = StartPos;
	_vector vEndPos = EndPos;
	_vector vDir = { vEndPos - vStartPos };

	_float fDistance = XMVectorGetX(XMVector3Length(vEndPos - vStartPos));

	PSX::PxSweepBufferN<12> pxBuffer = {};

	_bool bHit = m_pGameInstance->SphereCast(fRadius, vStartPos, vDir, fDistance, PSX::PxHitFlag::eDEFAULT, PSX::PxQueryFlag::eDYNAMIC | PSX::PxQueryFlag::eSTATIC | PSX::PxQueryFlag::eNO_BLOCK, pxBuffer);
	ON_COLLISION_INFO tagCollInfo = {};
	XMStoreFloat4(&tagCollInfo.vHitDir, vDir);
	tagCollInfo.vWorldPos.w = 1.f;
	if (bHit) {
		HRESULT hr = CheckHit(&pxBuffer.block, tagCollInfo);
		if (FAILED(hr)) {
			PSX::PxSweepHit* hits = pxBuffer.touches;
			_uint iHitCount = pxBuffer.getNbTouches();
			for (_uint i = 0; i < iHitCount; ++i) {
				PSX::PxSweepHit* pHit = &pxBuffer.touches[i];
				hr = CheckHit(pHit, tagCollInfo);
				if (SUCCEEDED(hr)) {
					break;
				}
			}
		}
	}
	return tagCollInfo;
}

HRESULT CMapElement_Interactable::CheckHit(PSX::PxSweepHit* pHit, ON_COLLISION_INFO& Info)
{
	HRESULT hr = E_FAIL;
	PSX::PxActor* pActor = pHit->actor;
	PSX::PxShape* pShape = pHit->shape;
	PHYSX_USERDATA* pUserData = { nullptr };
	CGameObject* pTarget = { nullptr };
	if (nullptr != pActor && nullptr != pActor->userData || nullptr != pShape && nullptr != pShape->userData) {
		pUserData = static_cast<PHYSX_USERDATA*>(pActor->userData);
		if (nullptr == pUserData) {
			pUserData = static_cast<PHYSX_USERDATA*>(pShape->userData);
		}
		switch (pUserData->eKind)
		{
		case PHYSX_KIND::BODY_STATIC:
		case PHYSX_KIND::CCTActor:
		{
			switch (PXOBJECT(pUserData->iSubKind))
			{
			case PXOBJECT::TERRAIN:
			case PXOBJECT::GOBLIN_WARRIOR:
			case PXOBJECT::GOBLIN_MAGICIAN:
			case PXOBJECT::GOBLIN_ASSASSIN:
			case PXOBJECT::TROLL:
			case PXOBJECT::RANROK_BODY:
			case PXOBJECT::RANROK:
			{
				hr = S_OK;
				m_bHit = true;
			} break;
			default:
				break;
			}
		} break;
		default:
			break;
		}
	}
	if (SUCCEEDED(hr)) {
		memcpy_s(&Info.vWorldPos, sizeof(Info.vWorldPos), &pHit->position, sizeof(pHit->position));
		memcpy_s(&Info.vWorldNomal, sizeof(Info.vWorldNomal), &pHit->normal, sizeof(pHit->normal));
		Info.fLength = pHit->distance;
		Info.pObject = pUserData->pOwner;
		Info.pObject->OnCollision(this, &Info);
	}
	return hr;
}

CMapElement_Interactable* CMapElement_Interactable::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Interactable* pInstance = new CMapElement_Interactable(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Interactable");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Interactable::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Interactable* pInstance = new CMapElement_Interactable(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Interactable");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Interactable::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pEffectPool);
	 
	if (!m_isPooled) {
		if (nullptr != m_pInfoInstance) {
			m_pInfoInstance = nullptr;
		}
	}
	
	for (auto& pModel : m_pModelComs){
		SAFE_RELEASE(pModel);
	}


}

#ifdef _DEBUG
void CMapElement_Interactable::Describe_Entity()
{

}
#endif // _DEBUG


