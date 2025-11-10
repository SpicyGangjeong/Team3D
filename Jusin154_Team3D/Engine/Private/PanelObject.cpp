#include "pch.h"
#include "PanelObject.h"

CPanelObject::CPanelObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject{ pDevice, pContext }
{
}

CPanelObject::CPanelObject(const CPanelObject& rhs)
	: CUIObject(rhs)
{
}

_vector CPanelObject::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CPanelObject::Priority_Update(_float fTimeDelta)
{
	CTransform* pOwnerTransform = m_pOwner->Get_Component<CTransform>();
}

void CPanelObject::Update(_float fTimeDelta)
{
	m_vScale = _float3(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Scale(m_vScale);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		m_fX + m_pOwner->Get_WorldPostion().m128_f32[0],
		-m_fY + m_pOwner->Get_WorldPostion().m128_f32[1],
		0.f, 1.f));

	m_fCurrent_Posigion = XMVectorSet(m_fX, m_fY, 0.f, 1.f);
}

void CPanelObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CPanelObject::Render()
{
	return E_NOTIMPL;
}

HRESULT CPanelObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPanelObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPanelObject::Ready_Components(void* pArg)
{
	__super::Ready_Components(pArg);
	return S_OK;
}

HRESULT CPanelObject::Ready_Panel(void* pArg)
{
	return S_OK;
}

void CPanelObject::MoveX(_float fX)
{

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		fX - m_fWinSizeX * 0.5f,
		-m_fY + m_fWinSizeY * 0.5f,
		0.f, 1.f));
}

void CPanelObject::MoveY(_float fY)
{
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		m_fX - m_fWinSizeX * 0.5f,
		-fY + m_fWinSizeY * 0.5f,
		0.f, 1.f));
}

void CPanelObject::Visible(_bool bVisible)
{
	if (m_Elements.empty())
		return;

	for (auto iter : m_Elements)
	{
		static_cast<CUIObject*>(iter)->Visible(bVisible);
	}
}

CGameObject* CPanelObject::Get_Element(const wstring& Name)
{
	return Find_Element(Name);
}

void CPanelObject::Move_Panel(_float fX, _float fY)
{
}

void CPanelObject::Add_Element(wstring Name, CGameObject* pPanel)
{
	if (pPanel == nullptr)
	{
		return;
	}

	if (Find_Element(Name) != nullptr)
	{
		return;
	}

	m_Elements.push_back(pPanel);
	m_Elements_map.emplace(Name, pPanel);
}

CGameObject* CPanelObject::Find_Element(const wstring& Name)
{
	auto iter = m_Elements_map.find(Name);

	if (iter == m_Elements_map.end())
		return nullptr;

	return iter->second;
}

void CPanelObject::Free()
{
	__super::Free();
}
