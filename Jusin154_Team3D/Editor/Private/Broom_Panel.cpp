#include "pch.h"
#include "Broom_Panel.h"
#include "GameInstance.h"
#include "Broom_Flag.h"
#include "Broom_Circle.h"
#include "Broom_Scoreboard.h"

CBroom_Panel::CBroom_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CBroom_Panel::CBroom_Panel(const CBroom_Panel& rhs)
	:CPanelObject(rhs)
{
}

HRESULT CBroom_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBroom_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 960.f;
	Desc.fY = -400.f;
	Desc.fSizeX = 1920.f;
	Desc.fSizeY = 700.f;
	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };


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

	Visible(true);
	ElementAllVisible(true);
	return S_OK;	
}

void CBroom_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CBroom_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	__super::Update(fTimeDelta);
}

void CBroom_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
	__super::Late_Update(fTimeDelta);

}

HRESULT CBroom_Panel::Render()
{
	return S_OK;
}

_vector CBroom_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CBroom_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CBroom_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CBroom_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_Flag>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_Flag**>(&m_pBroom_Flag))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Broom_Flag"), m_pBroom_Flag);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_Circle>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_Circle**>(&m_pBroom_Circle))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Broom_Circle"), m_pBroom_Circle);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CBroom_Scoreboard>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CBroom_Scoreboard**>(&m_pBroom_Scoreboard))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Broom_Scoreboard"), m_pBroom_Scoreboard);

	return S_OK;
}

CBroom_Panel* CBroom_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBroom_Panel* pInstance = new CBroom_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CBroom_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CBroom_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CBroom_Panel* pInstance = new CBroom_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CBroom_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CBroom_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CBroom_Panel::Describe_Entity()
{
}
