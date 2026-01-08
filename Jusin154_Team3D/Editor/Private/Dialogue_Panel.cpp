#include "pch.h"
#include "Dialogue_Panel.h"
#include "GameInstance.h"
#include "Dialogue_Choice.h"

CDialogue_Panel::CDialogue_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CDialogue_Panel::CDialogue_Panel(const CDialogue_Panel& rhs)
	:CPanelObject(rhs)
{
}

HRESULT CDialogue_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDialogue_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 500.f;
	Desc.fY = -625.f;
	Desc.fSizeX = 750.f;
	Desc.fSizeY = 410.f;

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
	Visible(true);
	ElementAllVisible(true);
	return S_OK;
}

void CDialogue_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CDialogue_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	__super::Update(fTimeDelta);
}

void CDialogue_Panel::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bVisible) {
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CDialogue_Panel::Render()
{
	return S_OK;
}

_vector CDialogue_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CDialogue_Panel::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CDialogue_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDialogue_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CDialogue_Choice>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CDialogue_Choice**>(&m_pDialogue_Choice))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Dialogue_Choice"), m_pDialogue_Choice);
	return S_OK; 
}

CDialogue_Panel* CDialogue_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDialogue_Panel* pInstance = new CDialogue_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDialogue_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDialogue_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CDialogue_Panel* pInstance = new CDialogue_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDialogue_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDialogue_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

void CDialogue_Panel::Describe_Entity()
{
}
