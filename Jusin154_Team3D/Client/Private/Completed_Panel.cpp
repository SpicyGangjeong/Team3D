#include "pch.h"
#include "Completed_Panel.h"
#include "GameInstance.h"

CCompleted_Panel::CCompleted_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CCompleted_Panel::CCompleted_Panel(const CCompleted_Panel& rhs)
	:CPanelObject(rhs)
{
}

HRESULT CCompleted_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCompleted_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 1575.f;
	Desc.fSizeY = g_iWinSizeY;

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Element(pArg)))
	{
		return E_FAIL;
	}
	m_fCanvasAlpha = 1.f;
	m_fSortZ = 0.04f;
	Visible(false);
	ElementAllVisible(true);
	return S_OK;
}

void CCompleted_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CCompleted_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	__super::Update(fTimeDelta);
}

void CCompleted_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		__super::Late_Update(fTimeDelta);
	}
}


HRESULT CCompleted_Panel::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::ALPHABLEND)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	return S_OK;
}

_vector CCompleted_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CCompleted_Panel::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOwnerAlpha", &m_fOwnerAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCanvasAlpha", &m_fCanvasAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CCompleted_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_UI_T_ActionItemGoldleaf_4K"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCompleted_Panel::Ready_Element(void* pArg)
{
	return S_OK;
}

CCompleted_Panel* CCompleted_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCompleted_Panel* pInstance = new CCompleted_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCompleted_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CCompleted_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CCompleted_Panel* pInstance = new CCompleted_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCompleted_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CCompleted_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CCompleted_Panel::Describe_Entity()
{
}
#endif // _DEBUG
