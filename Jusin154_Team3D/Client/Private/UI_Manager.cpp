#include "pch.h"
#include "UI_Manager.h"
#include "GameInstance.h"
#include "GamePlay_Canvas.h"
#include "Spell_Canvas.h"

CUI_Manager::CUI_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject(pDevice, pContext)
{
}

CUI_Manager::CUI_Manager(const CUI_Manager& rhs)
	:CUIObject(rhs)
{
}

HRESULT CUI_Manager::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Manager::Initialize(void* pArg)
{
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	m_bCanvas_Change = false;
	return S_OK;
}

void CUI_Manager::Priority_Update(_float fTimeDelta)
{
}

void CUI_Manager::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_T))
	{
		m_bCanvas_Change = !m_bCanvas_Change;
	}

	if (m_bCanvas_Change == false)
	{
		static_cast<CCanvasObject*>(m_pGamePlay_Canves)->Visible(true);
		static_cast<CCanvasObject*>(m_pSpell_Panel)->Visible(false);
	}
	else
	{
		static_cast<CCanvasObject*>(m_pSpell_Panel)->Visible(true);
		static_cast<CCanvasObject*>(m_pGamePlay_Canves)->Visible(false);
	}
}

void CUI_Manager::Late_Update(_float fTimeDelta)
{
}

HRESULT CUI_Manager::Render()
{
	return S_OK;
}

_vector CUI_Manager::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CUI_Manager::Bind_ShaderResources()
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CUI_Manager::Ready_Components(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CGamePlay_Canvas>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CGamePlay_Canvas**>(&m_pGamePlay_Canves)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Canvas>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast<CSpell_Canvas**>(&m_pSpell_Panel)))) {
		return E_FAIL;
	}

	return S_OK;
}

void CUI_Manager::Clear_Penel()
{
}

CUI_Manager* CUI_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Manager* pInstance = new CUI_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CUI_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CUI_Manager::Clone(void* pArg, CGameObject* pOwner)
{
	CUI_Manager* pInstance = new CUI_Manager(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUI_Manager");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CUI_Manager::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}
#ifdef _DEBUG

void CUI_Manager::Describe_Entity()
{
}

#endif // _DEBUG
