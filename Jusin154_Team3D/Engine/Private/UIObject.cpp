#include "pch.h"
#include "UIObject.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CUIObject::CUIObject(const CUIObject& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUIObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUIObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	// 투영 행렬릉 하기 위해서 일단 뷰 포트를 받아온다.
	D3D11_VIEWPORT       ViewportDesc{};
	// 그리고 설정할 뷰 포트의 개수를 선언 해준다.
	_uint                iNumViewports = { 1 };

	// 이제 뷰포트의 개수랑 정보를 저장할 포인터를 넣어준다.
	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	// 이제 받아온 fX,fY,fSizeX,fSizeY를 다 저장 해 놓는다.
	UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	m_fOrigin_Posigion = _float2(pDesc->fX, pDesc->fY);


	m_fWinSizeX = ViewportDesc.Width;
	m_fWinSizeY = ViewportDesc.Height;

	// 설정을 해준 구조체를 바탕으로 사이즈를 설정 해준다.
	// 사이즈를 설정 했으면 이제 위치도 잡아준다.
	// 먼저 Position을 세팅 해주기 위해서 가로를 반으로 갈라서 fx를 뺀다.
	// 그 후에 세로를 반으로 갈라서 fy랑 더해주는데 왜 이친구는 -인가?
	// DX의 중점은 정 중앙이다 그래서 지금이렇게 계산하는 이유는 화면 중앙이 0,0이다
	// 그러면 이제 위치를 잡을 때 화면 중앙을 기준으로 왼쪽에 배치를 하고 싶으면
	// 이제 값을 -로 해주고 위에 배치하려면 또 -를 해야해서 계산하는게 복잡하다.
	// 그래서 이제 계산하기 편하게 하기 위에서 중점을 우상단 api처럼 좌표계를 바꿔주기 위해
	// x는 api랑 같이 왼쪽이 - 오른쪽이 +이다. 하지만 DX는 위쪽이 +인데 api는 위로
	// 올라가면 -고 내려가면 +이다. 그래서 이거를 바꿔주기 위해서 -를 해주고 화면의 절반을
	// +로 해준다. 그리고 z값은 이제 렌더링을 할 때 깊이 버퍼르 0~1사이에 있는 친구들만
	// 그려 주는데 이제 UI는 화면 제일 앞에 있어야 해서 z값을 0으로 해준다.
	// 그리고 w값은 1을 줬는데 1을 줘야 x,y,z값이 계산이 되는데 만약 0을 준다면
	// 계산이 안되서 1로 준다. 만약 2나 3으로 주면 1이 원본 크기인데 2면 절반 3이면
	// 1/3만큼 줄어든다. 그래서 1을 주면 원래 크기대로 나온다.
	/* 뷰스페이스 상에 네모를 어떻게 배치시킬건지를 설정하는거다. */
	m_vScale = _float3(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Scale(m_vScale);
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
		pDesc->fX - ViewportDesc.Width * 0.5f, -
		pDesc->fY + ViewportDesc.Height * 0.5f,
		0.f, 1.f));

	m_fOrigin_Size = m_vScale;

	// 직교 투영을 하기 위해서 먼저 뷰 행렬을 항등 행렬로 바꿔준다.
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

	// 이제 투영 행렬을 직교 투영 행렬로 바꿔 주는데 화면의 사이즈를 받고 near 0 far 1로
	// 설정을 한다. 
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_bVisible = true;


	return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
	CTransform* pOwnerTransform = m_pOwner->Get_Component<CTransform>();
}

void CUIObject::Update(_float fTimeDelta)
{
	m_vScale = _float3(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_Scale(m_vScale);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - m_fWinSizeX * 0.5f,-m_fY + m_fWinSizeY * 0.5f,0.f, 1.f));

	m_fCurrent_Posigion = XMVectorSet(m_fX, m_fY, 0.f, 1.f);

}

void CUIObject::Late_Update(_float fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
	return S_OK;
}

HRESULT CUIObject::Ready_Components(void* pArg)
{
	__super::Ready_Components(pArg);
	return S_OK;
}

_vector CUIObject::Get_WorldPostion()
{
	return m_pOwner->Get_WorldPostion();
}

void CUIObject::MoveX(_float fX)
{
	m_fX = fX;
}

void CUIObject::MoveY(_float fY)
{
	m_fY = fY;
}

void CUIObject::SizeUpdate(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
}

void CUIObject::SizeUpdate_float(_float3 fSizeXY)
{
	m_fSizeX = fSizeXY.x;
	m_fSizeY = fSizeXY.y;
}

void CUIObject::Set_Time(_float fTime)
{
	m_fTime = fTime;
}

void CUIObject::Set_TimeMult(_float Mult)
{
	m_fTimeMult = Mult;
}

_float CUIObject::Get_EndTime()
{
	return m_fEndTime;
}

_float CUIObject::Get_DelayTime()
{
	return m_fDelayTime;
}

_float CUIObject::Get_TimeMult()
{
	return m_fTimeMult;
}

void CUIObject::Visible(_bool bVisible)
{
	m_bVisible = bVisible;
}

_bool CUIObject::Get_Visible()
{
	return m_bVisible;
}

_bool CUIObject::Get_Active()
{
	return m_bActive;
}

_bool CUIObject::Chack_Visible()
{
	return _bool();
}

void CUIObject::Set_FadeIn()
{
	m_bFadeIn = true;
	m_bFadeOut = false;
}

void CUIObject::Set_FadeOut()
{
	m_bFadeIn = false;
	m_bFadeOut = true;
}

_bool CUIObject::Get_FadeIn()
{
	return _bool();
}

_bool CUIObject::Get_FadeOut()
{
	return _bool();
}

void CUIObject::Set_Alpha(_float fAlpha)
{
	m_fAlpha = fAlpha;
}

void CUIObject::Set_AlphaTime(_float fAlpha)
{
	m_fAlphaTime = fAlpha;
}

void CUIObject::Set_OwnerAlpha(_float fAlpha)
{
	m_fOwnerAlpha = fAlpha;
}

_float CUIObject::Get_Alpha()
{
	return m_fAlpha;
}

_float CUIObject::Get_OwnerAlpha()
{
	return m_fOwnerAlpha;
}

_float CUIObject::Get_AlphaTime()
{
	return m_fAlphaTime;
}
_float2 CUIObject::Get_Origin_Position()
{
	return m_fOrigin_Posigion;
}

_vector CUIObject::Get_Current_Position()
{
	return m_fCurrent_Posigion;
}

_float3 CUIObject::Get_Origin_Size()
{
	return m_fOrigin_Size;
}

_float3 CUIObject::Get_Current_Size()
{
	return m_pTransformCom->Get_Scale();
}

void CUIObject::Free()
{
	__super::Free();
}
