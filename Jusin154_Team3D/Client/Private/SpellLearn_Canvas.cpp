#include "pch.h"
#include "UI_Manager.h"
#include "SpellLearn_Canvas.h"
#include "GameInstance.h"
#include "SpellLearn_Panel.h"

CSpellLearn_Canvas::CSpellLearn_Canvas(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCanvasObject(pDevice, pContext)
{
}

CSpellLearn_Canvas::CSpellLearn_Canvas(const CSpellLearn_Canvas& rhs)
	:CCanvasObject(rhs)
{
}

HRESULT CSpellLearn_Canvas::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpellLearn_Canvas::Initialize(void* pArg)
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

void CSpellLearn_Canvas::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CSpellLearn_Canvas::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CSpellLearn_Canvas::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CSpellLearn_Canvas::Render()
{
	return S_OK;
}

_vector CSpellLearn_Canvas::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CSpellLearn_Canvas::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CSpellLearn_Canvas::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpellLearn_Canvas::Ready_Panel(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CSpellLearn_Panel>(g_iStaticLevel, g_iStaticLevel, LAYER_UI, nullptr, this, reinterpret_cast <CSpellLearn_Panel**>(&m_pSpellLearn_Panel))))
	{
		return E_FAIL;
	}
	Add_Panel(TEXT("SpellLearn_Panel"), m_pSpellLearn_Panel);

	return S_OK;
}

void CSpellLearn_Canvas::Clear_Penel()
{
}

CSpellLearn_Canvas* CSpellLearn_Canvas::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSpellLearn_Canvas* pInstance = new CSpellLearn_Canvas(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpellLearn_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSpellLearn_Canvas::Clone(void* pArg, CGameObject* pOwner)
{
	CSpellLearn_Canvas* pInstance = new CSpellLearn_Canvas(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpellLearn_Canvas");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSpellLearn_Canvas::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CSpellLearn_Canvas::Describe_Entity()
{
}
#endif // _DEBUG
