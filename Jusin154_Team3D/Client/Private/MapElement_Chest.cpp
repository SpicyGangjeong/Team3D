#include "pch.h"
#include "MapElement_Chest.h"

#include "GameInstance.h"
#include "Terrain.h"
#include "Layer.h"
#include "MapElement_Chest_Lid.h"
#include "Player.h"

CMapElement_Chest::CMapElement_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Chest::CMapElement_Chest(const CMapElement_Chest& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Chest::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Chest::Initialize(void* pArg)
{
	ELEMENT_CHEST_DESC* pDesc = static_cast<ELEMENT_CHEST_DESC*>(pArg);

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

	m_eCurState = m_ePreState = CHEST_STATE::IDLE;
	m_fRimLightPower = pDesc->fRimLightPow;
	m_fRimLightStrength = pDesc->fRimLightStrength;
	m_vRimLightColor = pDesc->vRimLightColor;

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));

	/* 뚜껑 */
	if (FAILED(Ready_Lid(pDesc)))
		return E_FAIL;

	m_pActor = static_cast<PSX::PxRigidDynamic*>(m_pRigidBody->Get_Actor());

	static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Set_HalfGeometryInfo(pDesc->vBoxSize);
	static_cast<CRigidBody_Dynamic*>(m_pRigidBody)->Move_LocalPos(_float4(0.f, 0.f, 0.f, 0.f), pDesc->vBoxLocalPosition);

	return S_OK;
}

void CMapElement_Chest::Priority_Update(_float fTimeDelta)
{
	m_pLid->Priority_Update(fTimeDelta);
}

void CMapElement_Chest::Update(_float fTimeDelta)
{
#ifdef _DEBUG
	/*if(CHEST_STATE::OPENED != m_eCurState)
	{
		_vector vPlayerPos = m_pGameInstance->Get_Layer(g_iStaticLevel, LAYER_PLAYER)->Get_Object<CPlayer>()->Get_WorldPostion();

		_float fDistance = XMVectorGetX(XMVector3Length(vPlayerPos - m_pTransformCom->Get_State(STATE::POSITION)));

		if (CHEST_STATE::IDLE == m_eCurState && fDistance < 10.f)
			m_eCurState = CHEST_STATE::FOUND;
		else if (fDistance < 1.5f)
			m_eCurState = CHEST_STATE::OPENED;
	}*/
#endif // _DEBUG

	switch (m_eCurState)
	{
	case CMapElement_Chest::CHEST_STATE::IDLE:
		break;

	case CMapElement_Chest::CHEST_STATE::FOUND:
		m_fTimeAcc += fTimeDelta * 0.03f;

		if (0.f <= m_fTimeAcc && 0.06f > m_fTimeAcc)
		{
			m_fCurRimLightStrength = m_fRimLightStrength * m_fTimeAcc / 0.06f;
		}
		else if (0.3f <= m_fTimeAcc)
		{
			m_fTimeAcc = 0.f;
			m_fCurRimLightStrength = 0.f;
			m_eCurState = CHEST_STATE::IDLE;
		}
		else
		{
			m_fCurRimLightStrength = m_fRimLightStrength;
		}
		break;

	case CMapElement_Chest::CHEST_STATE::OPENED:
		break;

	default:
		break;
	}

	Chage_State();

	m_pLid->Update(fTimeDelta);
}

void CMapElement_Chest::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_WorldFrustum(Get_WorldPostion(), m_pModelComs[0]->Get_Radius())) {

		if (CMapElement_Chest::CHEST_STATE::FOUND == m_eCurState)
		{
			m_pGameInstance->Add_RenderGroup(RENDER::NONLIGHT, this);
			m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
		}
		else
			m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);

		m_pLid->Late_Update(fTimeDelta);
	}
}

HRESULT CMapElement_Chest::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelComs[0]->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iShaderPass_Index))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelComs[m_iLodIndex]->Render(i))) {
			return E_FAIL;
		}
	}

#ifdef _DEBUG
	if (FAILED(m_pRigidBody->Render())) {
		return E_FAIL;
	}
#endif // _DEBUG

	if (FAILED(m_pLid->Render(m_iShaderPass_Index)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapElement_Chest::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	for (_uint i = 0; i < m_iMaxLodLevel + 1; ++i)
	{
		CModel* pModel = { nullptr };

		/* Com_Model */
		if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, m_ModelPrototypeTags[i],
			reinterpret_cast<CComponent**>(&pModel))))
			return E_FAIL;

		m_pModelComs.push_back(pModel);
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Levioso_Noise"),
		reinterpret_cast<CComponent**>(&m_pNoiseTextureCom))))
		return E_FAIL;

	ELEMENT_CHEST_DESC* pPhysXDummyDesc = static_cast<ELEMENT_CHEST_DESC*>(pArg);

	// RIGID_BODY
	CRigidBody_Dynamic::RIGIDBODY_DYNAMIC_DESC Desc{};
	Desc.iSubKind = ENUM_CLASS(PXOBJECT::BOX);
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("PHYSX_DYNAMIC_BOX_KIN"), (CComponent**)&m_pRigidBody, &Desc))) {
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CMapElement_Chest::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTimeAcc, sizeof(_float)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimPower", &m_fRimLightPower, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimStrength", &m_fCurRimLightStrength, sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vRimColor", &m_vRimLightColor, sizeof(_float4)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_SRV("g_NoiseTexture", m_pNoiseTextureCom->Get_SRV(0)))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapElement_Chest::Ready_Lid(ELEMENT_CHEST_DESC* pArg)
{
	m_pLid = m_pGameInstance->Clone_Prototype<CMapElement_Chest_Lid>(g_iStaticLevel, pArg, this);

	if (nullptr == m_pLid)
		return E_FAIL;

	return S_OK;
}

void CMapElement_Chest::Chage_State()
{
	if (m_ePreState != m_eCurState)
	{
		switch (m_eCurState)
		{
		case CMapElement_Chest::CHEST_STATE::IDLE:
			m_fCurRimLightStrength = 0.f;
			m_fTimeAcc = 0.f;
			m_iShaderPass_Index = ENUM_CLASS(SHADER_PASS_MESH::DEFAULT);
			break;

		case CMapElement_Chest::CHEST_STATE::FOUND:
			m_iShaderPass_Index = 21; //ENUM_CLASS(SHADER_PASS_MESH::DEFAULT);
			break;

		case CMapElement_Chest::CHEST_STATE::OPENED:
			m_iShaderPass_Index = ENUM_CLASS(SHADER_PASS_MESH::DEFAULT);
			m_pLid->Open();
			break;

		default:
			break;
		}
		m_ePreState = m_eCurState;
	}
}

CMapElement_Chest* CMapElement_Chest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Chest* pInstance = new CMapElement_Chest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Chest");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Chest::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Chest* pInstance = new CMapElement_Chest(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Chest");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Chest::Free()
{
	if (nullptr != m_pRigidBody) {
		m_pGameInstance->Release_Actor(*m_pRigidBody->Get_Actor());
	}
	__super::Free();

	if (m_bCloned)
		SAFE_RELEASE(m_pLid);

	SAFE_RELEASE(m_pRigidBody);
	SAFE_RELEASE(m_pNoiseTextureCom);
	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

#ifdef _DEBUG
void CMapElement_Chest::Describe_Entity()
{
}
#endif // _DEBUG


