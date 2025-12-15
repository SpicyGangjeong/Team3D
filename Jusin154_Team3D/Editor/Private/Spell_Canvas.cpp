#include "pch.h"
#include "UI_Manager.h"
#include "Spell_Canvas.h"
#include "GameInstance.h"
#include "Spell_Panel.h"

CSpell_Canvas::CSpell_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCanvasObject(pDevice, pContext)
{
}

CSpell_Canvas::CSpell_Canvas(const CSpell_Canvas& rhs)
	:CCanvasObject(rhs)
{
}

HRESULT CSpell_Canvas::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpell_Canvas::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 1920.f;
	Desc.fSizeY = 1080.f;

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Panel(pArg)))
	{
		return E_FAIL;
	}
	Visible(false);
	return S_OK;
}

void CSpell_Canvas::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSpell_Canvas::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CSpell_Canvas::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CSpell_Canvas::Render()
{
	return S_OK;
}

_vector CSpell_Canvas::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpell_Canvas::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CSpell_Canvas::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpell_Canvas::Ready_Panel(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpell_Panel>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast <CSpell_Panel**>(&m_pSpell_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("Spell_Panel"), m_pSpell_Panel);
	return S_OK;
}

void CSpell_Canvas::Clear_Penel()
{

}

CSpell_Canvas* CSpell_Canvas::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpell_Canvas* pInstance = new CSpell_Canvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpell_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpell_Canvas::Clone(void* pArg, CGameObject* pOwner)
{
	CSpell_Canvas* pInstance = new CSpell_Canvas(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpell_Canvas::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

void CSpell_Canvas::Describe_Entity()
{
}

