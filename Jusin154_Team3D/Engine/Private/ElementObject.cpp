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

	m_fCurrent_Position = XMVectorSet(m_fX, m_fY, 0.f, 1.f);
	m_vNine_Slice = _float4(m_fLeft, m_fRight, m_fTop, m_fBottom);
	m_vLerp_Position = XMVectorSet(m_fLerpX, m_fLerpY, 0.f, 1.f);
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

void CElementObject::Nine_Slice_Left(_float X)
{
	m_fLeft = X;
}

void CElementObject::Nine_Slice_Right(_float Y)
{
	m_fRight = Y;
}

void CElementObject::Nine_Slice_Top(_float Z)
{
	m_fTop = Z;
}

void CElementObject::Nine_Slice_Bottom(_float W)
{
	m_fBottom = W;
}

_float CElementObject::Get_Nine_Slice_Left()
{
	return m_fLeft;
}

_float CElementObject::Get_Nine_Slice_Right()
{
	return m_fRight;
}

_float CElementObject::Get_Nine_Slice_Top()
{
	return m_fTop;
}

_float CElementObject::Get_Nine_Slice_Bottom()
{
	return m_fBottom;
}

void CElementObject::Start_Lerp(_float fTimeDelta, _bool Alpha)
{
	m_fX = MovDir(m_fCurrent_Position.m128_f32[0], m_vLerp_Position.m128_f32[0], fTimeDelta);
	m_fY = MovDir(m_fCurrent_Position.m128_f32[1], m_vLerp_Position.m128_f32[1], fTimeDelta);
}

void CElementObject::Reset_Pos(_float fTimeDelta, _bool Alpha)
{
	m_fX = MovDir(m_fCurrent_Position.m128_f32[0], m_fOrigin_Position.x, fTimeDelta);
	m_fY = MovDir(m_fCurrent_Position.m128_f32[1], m_fOrigin_Position.y, fTimeDelta);
}

_vector CElementObject::Get_Lerp_Pos()
{
	return m_vLerp_Position;
}

HRESULT CElementObject::Ready_Components(void* pArg)
{
	return S_OK;
}

void CElementObject::Free()
{
    __super::Free();
}
