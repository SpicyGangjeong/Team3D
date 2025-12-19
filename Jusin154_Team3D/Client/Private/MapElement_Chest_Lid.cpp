#include "pch.h"
#include "MapElement_Chest_Lid.h"
#include "MapElement_Chest.h"

#include "GameInstance.h"

CMapElement_Chest_Lid::CMapElement_Chest_Lid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Chest_Lid::CMapElement_Chest_Lid(const CMapElement_Chest_Lid& rhs)
	: CMapElement(rhs)
{
}

HRESULT CMapElement_Chest_Lid::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapElement_Chest_Lid::Initialize(void* pArg)
{
	CMapElement_Chest::ELEMENT_CHEST_DESC* pDesc = static_cast<CMapElement_Chest::ELEMENT_CHEST_DESC*>(pArg);

	m_iMaxLodLevel = pDesc->iMaxLodLevel_Lid;

	for (_uint i = 0; i < m_iMaxLodLevel + 1; i++)
	{
		m_ModelPrototypeTags.push_back(pDesc->ModelPrototypeTags_Lid[i]);
	}

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_fOpenDuration = 0.1f;
	m_fRotationAngle = pDesc->fRotaitionAngle;
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vLid_Offset), 1.f));

	m_pParentWorldMatrix = m_pOwner->Get_Component<CTransform>()->Get_WorldMatrixPtr();
	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_XMWorldMatrix() * XMLoadFloat4x4(m_pParentWorldMatrix));

	return S_OK;
}

void CMapElement_Chest_Lid::Priority_Update(_float fTimeDelta)
{
}

void CMapElement_Chest_Lid::Update(_float fTimeDelta)
{
	switch (m_eState)
	{
	case CMapElement_Chest_Lid::LID_STATE::IDLE:
		break;

	case CMapElement_Chest_Lid::LID_STATE::OPENING:
		if (Opening(fTimeDelta))
			m_eState = LID_STATE::OPENED;
		break;

	case CMapElement_Chest_Lid::LID_STATE::OPENED:
		break;

	default:
		break;
	}
}

void CMapElement_Chest_Lid::Late_Update(_float fTimeDelta)
{
	/* 부모에서 Render 호출  */
}

HRESULT CMapElement_Chest_Lid::Render(_uint iShaderPass)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelComs[m_iLodIndex]->Get_NumMeshes();

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

	return S_OK;
}

HRESULT CMapElement_Chest_Lid::Ready_Components(void* pArg)
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


	return S_OK;
}

HRESULT CMapElement_Chest_Lid::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix))) {
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

_bool CMapElement_Chest_Lid::Opening(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fTimeAcc >= m_fOpenDuration)
	{
		m_fTimeAcc = m_fOpenDuration;
		m_pTransformCom->Rotation(XMConvertToRadians(m_fRotationAngle), 0.f, 0.f);
		return true;
	}

	m_pTransformCom->Rotation(XMConvertToRadians(m_fRotationAngle * m_fTimeAcc / m_fOpenDuration), 0.f, 0.f);

	XMStoreFloat4x4(&m_CombinedWorldMatrix, m_pTransformCom->Get_XMWorldMatrix() * XMLoadFloat4x4(m_pParentWorldMatrix));

	return false;
}

CMapElement_Chest_Lid* CMapElement_Chest_Lid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Chest_Lid* pInstance = new CMapElement_Chest_Lid(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Chest_Lid");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Chest_Lid::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Chest_Lid* pInstance = new CMapElement_Chest_Lid(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Chest_Lid");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Chest_Lid::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);

	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);
}

#ifdef _DEBUG
void CMapElement_Chest_Lid::Describe_Entity()
{
}
#endif // _DEBUG
