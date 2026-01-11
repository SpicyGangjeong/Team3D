#include "pch.h"
#include "MapElement_Balloon.h"

#include "GameInstance.h"

CMapElement_Balloon::CMapElement_Balloon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMapElement{ pDevice, pContext }
{
}

CMapElement_Balloon::CMapElement_Balloon(const CMapElement_Balloon& rhs)
	: CMapElement(rhs)
	, m_fUsingSurfaceParams(rhs.m_fUsingSurfaceParams)
	, m_fTimeAcc(0.f)
{
}

HRESULT CMapElement_Balloon::Initialize_Prototype()
{
	m_fUsingSurfaceParams = MRO_PARAMETER;

	return S_OK;
}

HRESULT CMapElement_Balloon::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	m_isRotate = false;
	m_fRadius = 10.f;

	BALLOON_DESC* pDesc = static_cast<BALLOON_DESC*>(pArg);
	m_isFloating = pDesc->isFloating;
	m_iDiffuseIndex = pDesc->iDiffuseIndex;

	memcpy(&m_vOriginPosition, &pDesc->vPosition, sizeof(_float3));

	m_pTransformCom->Set_Scale(pDesc->vScale);
	m_pTransformCom->Rotation(XMConvertToRadians(pDesc->vRotation.x), XMConvertToRadians(pDesc->vRotation.y), XMConvertToRadians(pDesc->vRotation.z));
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));

	return S_OK;
}

void CMapElement_Balloon::Priority_Update(_float fTimeDelta)
{
}

void CMapElement_Balloon::Update(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_isFloating)
		Floating();

	if (m_isRotate)
		Rotation();
}

void CMapElement_Balloon::Late_Update(_float fTimeDelta)
{
	if(m_pGameInstance->IsIn_WorldFrustum(m_pTransformCom->Get_State(STATE::POSITION), m_fRadius))
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMapElement_Balloon::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_MESH::DEFAULT)))) {
		return E_FAIL;
	}

	if (FAILED(m_pModelComs[0]->Render(0))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapElement_Balloon::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	CModel* pModel = { nullptr };

	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(NEXT_LEVEL, TEXT("Prototype_Component_FloatingBalloon"),
		reinterpret_cast<CComponent**>(&pModel))))
		return E_FAIL;

	m_pModelComs.push_back(pModel);

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Balloon_Diffuse"),
		reinterpret_cast<CComponent**>(&m_pDiffuseTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Balloon_Normal"),
		reinterpret_cast<CComponent**>(&m_pNormalTextureCom))))
		return E_FAIL;
	/* Com_Texture */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Balloon_MRO"),
		reinterpret_cast<CComponent**>(&m_pMROTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapElement_Balloon::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_SRV("g_DiffuseTexture", m_pDiffuseTextureCom->Get_SRV(m_iDiffuseIndex))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_SRV("g_NormalTexture", m_pNormalTextureCom->Get_SRV(0))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_SRV("g_SurfaceParamsTexture", m_pMROTextureCom->Get_SRV(0))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUsingSurfaceParams", &m_fUsingSurfaceParams, sizeof(_float)))) {
		return E_FAIL;
	}

	return S_OK;
}

void CMapElement_Balloon::Rotation()
{

}

void CMapElement_Balloon::Floating()
{
	_float3 vPosition = m_vOriginPosition;

	vPosition.y = m_vOriginPosition.y + sinf(m_fTimeAcc);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
}

CMapElement_Balloon* CMapElement_Balloon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMapElement_Balloon* pInstance = new CMapElement_Balloon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMapElement_Balloon");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMapElement_Balloon::Clone(void* pArg, CGameObject* pOwner)
{
	CMapElement_Balloon* pInstance = new CMapElement_Balloon(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMapElement_Balloon");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMapElement_Balloon::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	for (auto& pModel : m_pModelComs)
		SAFE_RELEASE(pModel);

	SAFE_RELEASE(m_pDiffuseTextureCom);
	SAFE_RELEASE(m_pNormalTextureCom);
	SAFE_RELEASE(m_pMROTextureCom);
}

#ifdef _DEBUG
void CMapElement_Balloon::Describe_Entity()
{
}
#endif // _DEBUG


