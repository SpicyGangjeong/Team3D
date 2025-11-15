#include "pch.h"
#include "Spell_Slot.h"
#include "GameInstance.h"

CSpell_Slot::CSpell_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSpell_Slot::CSpell_Slot(const CSpell_Slot& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSpell_Slot::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_Slot::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 600.f;
	Desc.fY = 500.f;
	Desc.fSizeX = 70.f;
	Desc.fSizeY = 70.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CSpell_Slot::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSpell_Slot::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	m_fTime += fTimeDelta * m_fTimeMult;

	__super::Update(fTimeDelta);

}

void CSpell_Slot::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
}

HRESULT CSpell_Slot::Render()
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

_vector CSpell_Slot::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpell_Slot::Bind_ShaderResources()
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

HRESULT CSpell_Slot::Ready_Components(void* pArg)
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

CSpell_Slot* CSpell_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Slot* pInstance = new CSpell_Slot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Slot");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Slot::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Slot* pInstance = new CSpell_Slot(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Slot");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Slot::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpell_Slot::Describe_Entity()
{
}
