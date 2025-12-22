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

	m_fOrigin_Position = _float2(pDesc->fX, pDesc->fY);

	m_fOrigin_Position_vector = XMVectorSet(m_fOrigin_Position.x, m_fOrigin_Position.y, 0.f, 1.f);

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
		m_fSortZ, 1.f));

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
	//m_vScale = _float3(m_fSizeX, m_fSizeY, 1.f);
	//m_pTransformCom->Set_Scale(m_vScale);

	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_fX - m_fWinSizeX * 0.5f, -m_fY + m_fWinSizeY * 0.5f, m_fSortZ, 1.f));

	//m_fCurrent_Position = XMVectorSet(m_fX, m_fY, m_fSortZ, 1.f);

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

void CUIObject::Move(_float fX, _float fY)
{
	m_fX = fX;
	m_fY = fY;
}

_float2 CUIObject::Get_Position()
{
	return _float2(m_fX, m_fY);
}

void CUIObject::SizeUpdate(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
}

void CUIObject::SizeUpX(_float fSizeX)
{
	m_fSizeX = fSizeX;
}

void CUIObject::SizeUpY(_float fSizeY)
{
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
	if (!m_bFadeIn && m_fAlpha < 1.f)
	{
		m_bFadeIn = true;
		m_bFadeOut = false;
	}
}

void CUIObject::Set_FadeOut()
{
	if (!m_bFadeOut && m_fAlpha > 0.f)
	{
		m_bFadeOut = true;
		m_bFadeIn = false;
	}
}

_bool CUIObject::Get_FadeIn()
{
	return m_bFadeIn;
}

_bool CUIObject::Get_FadeOut()
{
	return m_bFadeOut;
}

void CUIObject::Set_OwnerFadeIn()
{
	m_bOwnerFadeIn = true;
	m_bOwnerFadeOut = false;
}

void CUIObject::Set_OwnerFadeOut()
{
	m_bOwnerFadeIn = false;
	m_bOwnerFadeOut = true;
}

_bool CUIObject::Get_OwnerFadeIn()
{
	return m_bOwnerFadeIn;
}

_bool CUIObject::Get_OwnerFadeOut()
{
	return m_bOwnerFadeOut;
}

void CUIObject::Set_Alpha(_float fAlpha)
{
	m_fAlpha = fAlpha;
}

void CUIObject::Set_AlphaTime(_float fAlpha)
{
	m_fAlphaTime = fAlpha;
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
void CUIObject::Set_U(_float U)
{
	m_vUVScale.x = U;
}
void CUIObject::Set_V(_float V)
{
	m_vUVScale.y = V;
}

_float2 CUIObject::Get_UV()
{
	return m_vUVScale;
}

void CUIObject::Add_Canvas(wstring Name, CGameObject* pCanvas)
{
}

void CUIObject::Nine_Slice_Left(_float X)
{
	m_vNine_Slice.x = X;
}

void CUIObject::Nine_Slice_Right(_float Y)
{
	m_vNine_Slice.y = Y;
}

void CUIObject::Nine_Slice_Top(_float Z)
{
	m_vNine_Slice.z = Z;
}

void CUIObject::Nine_Slice_Bottom(_float W)
{
	m_vNine_Slice.w = W;
}

_float CUIObject::Get_Nine_Slice_Left()
{
	return m_vNine_Slice.y;
}

_float CUIObject::Get_Nine_Slice_Right()
{
	return m_vNine_Slice.y;
}

_float CUIObject::Get_Nine_Slice_Top()
{
	return m_vNine_Slice.z;
}

_float CUIObject::Get_Nine_Slice_Bottom()
{
	return m_vNine_Slice.w;
}

_float2 CUIObject::Get_Origin_Position()
{
	return m_fOrigin_Position;
}

_vector CUIObject::Get_Current_Position()
{
	return m_fCurrent_Position;
}

_float3 CUIObject::Get_Origin_Size()
{
	return m_fOrigin_Size;
}

_float3 CUIObject::Get_Current_Size()
{
	return m_pTransformCom->Get_Scale();
}

void CUIObject::Lerp_PosX(_float PosX)
{
	m_fLerpX = PosX;
}

void CUIObject::Lerp_PosY(_float PosY)
{
	m_fLerpY = PosY;
}

_bool CUIObject::Start_Lerp(_float fTimeDelta)
{
	return false;
}

_bool CUIObject::Start_Lerp_Speed(_float fTimeDelta, _float2 Mouse_Point)
{
	return _bool();
}

void CUIObject::Reset_Pos(_float fTimeDelta)
{
}

void CUIObject::Start_Size_Lerp(_float fTimeDelta)
{
}

void CUIObject::Reset_Size_Lerp(_float fTimeDelta)
{
}

_vector CUIObject::Get_Lerp_Pos()
{
	return m_vLerp_Position;
}

void CUIObject::LerpOn()
{
	m_bLerpOn = true;
	m_bLerpOff = false;
}

void CUIObject::LerpOff()
{
	m_bLerpOn = false;
	m_bLerpOff = true;
}

_bool CUIObject::Get_LerpOn()
{
	return m_bLerpOn;
}

_bool CUIObject::Get_LerpOff()
{
	return m_bLerpOff;
}

_float CUIObject::MovDir(_float Start, _float End, _float Speed)
{
	if (Start < End)
	{
		Start += Speed;
		if (Start > End)
			Start = End;
	}

	else
	{
		Start -= Speed;
		if (Start < End)
			Start = End;

	}
	return Start;
}

void CUIObject::Set_Speed(_float Speed)
{
	m_fMoveSpeed = Speed;
}

_float CUIObject::Get_Speed()
{
	return m_fMoveSpeed;
}

void CUIObject::Set_Angle(_float fAngle)
{
	m_fAngle = XMConvertToRadians(fAngle);
}

_float CUIObject::Get_Angle()
{
	return XMConvertToDegrees(m_fAngle);
}

void CUIObject::Set_SkillType(_int eType)
{
	m_iSpellType = eType;
}

_int CUIObject::Get_SkillType()
{
	return m_iSpellType;
}

void CUIObject::Set_SpellType(_int eType)
{
	m_iSkillType = eType;
}

_int CUIObject::Get_SpellType()
{
	return m_iSkillType;
}

void CUIObject::Set_CoolTime(_float CoolTime)
{
	m_fCoolTime = CoolTime;
}

_float CUIObject::Get_CoolTime()
{
	return m_fCoolTime;
}

void CUIObject::Add_Function(wstring Name, function<void(void*)> Evnet)
{
}

void CUIObject::Function_Callback(wstring Name, void* pArg)
{
}

void CUIObject::Set_Hover(_bool Hover)
{
	m_bHover = Hover;
}

_bool CUIObject::Get_Hover()
{
	return m_bHover;
}

const CUIObject::SPELLINFO CUIObject::Get_Info(_int Index)
{
	return SPELLINFO();
}

const CUIObject::QUESTINFO CUIObject::Get_QuestInfo(_int Index)
{
	return QUESTINFO();
}

const CUIObject::SPELLLEARNINFO CUIObject::Get_Learninfo(_int Index)
{
	return SPELLLEARNINFO();
}

void CUIObject::Set_FontX(_float fFontX)
{
	m_fFontX = fFontX;
}

void CUIObject::Set_FontY(_float fFontY)
{
	m_fFontY = fFontY;
}

_float2 CUIObject::Get_Font()
{
	return _float2(m_fFontX, m_fFontY);
}

_float2 CUIObject::Get_PerPosition()
{
	return m_fPerPosition;
}

void CUIObject::Free()
{
	__super::Free();
}
