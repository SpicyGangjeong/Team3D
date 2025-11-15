#include "pch.h"
#include "Action_Panel.h"
#include "GameInstance.h"
#include "Spell_Slot.h"

CAction_Panel::CAction_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPanelObject(pDevice, pContext)
{
}

CAction_Panel::CAction_Panel(const CAction_Panel& rhs)
    :CPanelObject(rhs)
{
}

HRESULT CAction_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAction_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 1515.f;
	Desc.fY = 850.f;
	Desc.fSizeX = 700.f;
	Desc.fSizeY = 400.f;


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

	m_bVisible = true;
	return S_OK;
}

void CAction_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CAction_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	__super::Update(fTimeDelta);
}

void CAction_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bVisible) {
		_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this, *vPos, m_pTransformCom->Get_Radius());
	}
	__super::Late_Update(fTimeDelta);

}

HRESULT CAction_Panel::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::DEFAULT)))) {
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

_vector CAction_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CAction_Panel::Bind_ShaderResources()
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

	return S_OK;
}

HRESULT CAction_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_UI_T_ActionItemGoldleaf_4K"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CAction_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Slot>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CSpell_Slot**>(&m_pSpell_Slot))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Spell_Slot"), m_pSpell_Slot);
	return S_OK;
}

CAction_Panel* CAction_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAction_Panel* pInstance = new CAction_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAction_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CAction_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CAction_Panel* pInstance = new CAction_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAction_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CAction_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CAction_Panel::Describe_Entity()
{
}
