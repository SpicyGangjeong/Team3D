#include "pch.h"
#include "Quest_Panel.h"
#include "GameInstance.h"
#include "Quest_Border.h"
#include "Quest_Header.h"
#include "Quest_HeaderLine.h"
#include "Quest_List.h"
#include "Quest_Info.h"
#include "Quest_Info_Header.h"
#include "Quest_Info_Line.h"
#include "Quest_Entry_Line.h"

CQuest_Panel::CQuest_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CPanelObject(pDevice, pContext)
{
}

CQuest_Panel::CQuest_Panel(const CQuest_Panel& rhs)
    :CPanelObject(rhs)
{
}

HRESULT CQuest_Panel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CQuest_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 960.f;
	Desc.fY = 540.f;
	Desc.fSizeX = g_iWinSizeX;
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
	m_fSortZ = 0.05f;
	m_iSpellType = -1;
	m_fDelayTime = 1.f;
	Visible(true);
	ElementAllVisible(true);
	return S_OK;
}

void CQuest_Panel::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CQuest_Panel::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	__super::Update(fTimeDelta);
}

void CQuest_Panel::Late_Update(_float fTimeDelta)
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

HRESULT CQuest_Panel::Render()
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

_vector CQuest_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CQuest_Panel::Bind_ShaderResources()
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

HRESULT CQuest_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Quest_Widget"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CQuest_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Border>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_Border**>(&m_pQuest_Border))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_Border"), m_pQuest_Border);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Header>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_Header**>(&m_pQuest_Header))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_Header"), m_pQuest_Header);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_HeaderLine>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_HeaderLine**>(&m_pQuest_HeaderLine))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_HeaderLine"), m_pQuest_HeaderLine);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_List>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_List**>(&m_pQuest_List))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_List"), m_pQuest_List);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Info>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_Info**>(&m_pQuest_Info))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_Info"), m_pQuest_Info);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Info_Header>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_Info_Header**>(&m_pQuest_Info_Header))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_Info_Header"), m_pQuest_Info_Header);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Info_Line>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_Info_Line**>(&m_pQuest_Info_Line))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_Info_Line"), m_pQuest_Info_Line);

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CQuest_Entry_Line>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CQuest_Entry_Line**>(&m_pQuest_Entry_Line))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("Quest_Entry_Line"), m_pQuest_Entry_Line);

    return S_OK;
}

CQuest_Panel* CQuest_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CQuest_Panel* pInstance = new CQuest_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CQuest_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CQuest_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CQuest_Panel* pInstance = new CQuest_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CQuest_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CQuest_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CQuest_Panel::Describe_Entity()
{
}
#endif // _DEBUG
