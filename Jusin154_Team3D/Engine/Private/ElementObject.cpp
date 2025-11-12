#include "pch.h"
#include "ElementObject.h"

CElementObject::CElementObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CElementObject::CElementObject(const CElementObject& rhs)
    : CUIObject(rhs)
{
}

HRESULT CElementObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CElementObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CElementObject::Priority_Update(_float fTimeDelta)
{
	CTransform* pOwnerTransform = m_pOwner->Get_Component<CTransform>();
}

void CElementObject::Update(_float fTimeDelta)
{
	m_vScale = _float3(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Scale(m_vScale);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		m_fX + m_pOwner->Get_WorldPostion().m128_f32[0],
		-m_fY + m_pOwner->Get_WorldPostion().m128_f32[1],
		0.f, 1.f));

	m_fCurrent_Posigion = XMVectorSet(m_fX, m_fY, 0.f, 1.f);
}

void CElementObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CElementObject::Render()
{
    return S_OK;
}

_vector CElementObject::Get_WorldPostion()
{
	return m_pOwner->Get_WorldPostion();
}

void CElementObject::Visible(_bool bVisible)
{
	m_bActive = bVisible;
}

_bool CElementObject::Chack_Visible()
{
	if (m_pOwner->Get_Visible() == false )
	{
		m_bVisible = m_pOwner->Get_Visible();
	}
	else if (static_cast<CUIObject*>(m_pOwner)->Get_Active() == false)
	{
		m_bVisible = static_cast<CUIObject*>(m_pOwner)->Get_Active();
	}
	else
	{
		m_bVisible = m_bActive;
	}
	return m_bVisible;
}

HRESULT CElementObject::Ready_Components(void* pArg)
{
	return S_OK;
}

void CElementObject::Free()
{
    __super::Free();
}
