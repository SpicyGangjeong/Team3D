#include "pch.h"
#include "CanvasObject.h"

CCanvasObject::CCanvasObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CCanvasObject::CCanvasObject(const CCanvasObject& rhs)
	: CUIObject(rhs)
{
}

_vector CCanvasObject::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CCanvasObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCanvasObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CCanvasObject::Priority_Update(_float fTimeDelta)
{
}

void CCanvasObject::Update(_float fTimeDelta)
{
	m_vScale = _float3(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Scale(m_vScale);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - m_fWinSizeX * 0.5f, -m_fY + m_fWinSizeY * 0.5f, 0.f, 1.f));

	m_fCurrent_Posigion = XMVectorSet(m_fX, m_fY, 0.f, 1.f);
}

void CCanvasObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CCanvasObject::Render()
{
	return S_OK;
}

HRESULT CCanvasObject::Ready_Components(void* pArg)
{
	__super::Ready_Components(pArg);
	return S_OK;
}

HRESULT CCanvasObject::Ready_Panel(void* pArg)
{
	return S_OK;
}

void CCanvasObject::Visible(_bool bVisible)
{
	m_bVisible = bVisible;
}

CGameObject* CCanvasObject::Get_Panel(const wstring& Name)
{
	return Find_Panel(Name);
}

_int CCanvasObject::Panels_Count()
{
	return m_iPanel_Count;
}

const vector<wstring> CCanvasObject::Panel_Name()
{
	return m_PanelNames;
}

void CCanvasObject::Add_Panel(wstring Name, CGameObject* pPanel)
{
	if (pPanel == nullptr)
	{
		return;
	}

	if (Find_Panel(Name) != nullptr)
	{
		return;
	}

	m_Panels.push_back(pPanel);
	m_PanelNames.push_back(Name);
	m_Panels_map.emplace(Name, pPanel);
	m_iPanel_Count++;
}

CGameObject* CCanvasObject::Find_Panel(const wstring& Name)
{
	auto iter = m_Panels_map.find(Name);

	if (iter == m_Panels_map.end())
		return nullptr;

	return iter->second;
}

void CCanvasObject::Free()
{
	__super::Free();

	m_Panels.clear();
}
