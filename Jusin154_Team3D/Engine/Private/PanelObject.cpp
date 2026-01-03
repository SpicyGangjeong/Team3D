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
	UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);
	if (pDesc->m_bowner == true)
	{
		m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
			m_fX + m_pOwner->Get_WorldPostion().m128_f32[0],
			m_fY + m_pOwner->Get_WorldPostion().m128_f32[1],
			m_fSortZ, 1.f));

		m_fCurrent_Position = _float4(m_fX, m_fY, 0.f, 1.f);
	}

	m_fAlpha = 1.f;
	return S_OK;
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
		m_fY + m_pOwner->Get_WorldPostion().m128_f32[1],
		m_fSortZ, 1.f));

	m_fCurrent_Position = _float4(m_fX, m_fY, 0.f, 1.f);

	m_fOwnerAlpha = static_cast<CUIObject*>(m_pOwner)->Get_Alpha();

	if (m_bFadeIn == true)
	{
		if (m_fAlpha <= 1.f)
		{
			m_fAlpha += fTimeDelta;
		}

		if (m_fAlpha >= 1.f)
		{
			m_bFadeIn = false;
			m_fAlpha = 1.f;
		}
	}

	if (m_bFadeOut == true)
	{
		if (m_fAlpha >= 0.f)
			m_fAlpha -= fTimeDelta;

		if (m_fAlpha <= 0.f)
		{
			m_bFadeOut = false;
			m_fAlpha = 0.f;
		}
	}

	for (auto& iter : m_Elements)
	{

	}
}

void CPanelObject::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_Elements.size(); ++i)
	{

	}
}

HRESULT CPanelObject::Render()
{
	return S_OK;
}

HRESULT CPanelObject::Ready_Components(void* pArg)
{
	__super::Ready_Components(pArg);
	return S_OK;
}

HRESULT CPanelObject::Ready_Element(void* pArg)
{
	return S_OK;
}

void CPanelObject::Visible(_bool bVisible)
{
	m_bActive = bVisible;
}

CGameObject* CPanelObject::Get_Element(const wstring& Name)
{
	return Find_Element(Name);
}

_int CPanelObject::Element_Count()
{
	return m_iElements_Count;
}

const vector<wstring> CPanelObject::Element_Name()
{
	return m_ElementName;
}

void CPanelObject::ElementAllVisible(_bool bVisible)
{
	if (m_Elements.empty())
		return;

	for (auto& iter : m_Elements)
	{
		static_cast<CUIObject*>(iter)->Visible(bVisible);
	}
}

void CPanelObject::Add_Function(wstring Name, function<void(void*)>Event)
{
	m_ElementFunction_map.emplace(Name, Event);
}

void CPanelObject::Function_Callback(wstring Name, void* pArg)
{
	auto range = m_ElementFunction_map.equal_range(Name);
	for (auto it = range.first; it != range.second; ++it)
	{
		it->second(pArg);
	}
}

_bool CPanelObject::Start_Lerp(_float fTimeDelta)
{
	_vector Pos = XMLoadFloat4(&m_fCurrent_Position);

	_vector Target = XMLoadFloat4(&m_vLerp_Position);

	_vector Dir = XMVectorSubtract(Target, Pos);
	_vector LenVec = XMVector3Length(Dir);
	_float Distance = XMVectorGetX(LenVec);

	_float move = m_fMoveSpeed * fTimeDelta;

	if (Distance <= move)
	{
		m_fX = m_vLerp_Position.x;
		m_fY = m_vLerp_Position.y;
		return true;
	}

	else
	{
		XMVECTOR DirNorm = XMVector3Normalize(Dir);
		m_fX = (Pos + DirNorm * move).m128_f32[0];
		m_fY = (Pos + DirNorm * move).m128_f32[1];
		return false;
	}
}

void CPanelObject::Reset_Pos(_float fTimeDelta)
{
	_vector Pos = XMLoadFloat4(&m_fCurrent_Position);

	_vector Target = XMVectorSet(m_fOrigin_Position.x, m_fOrigin_Position.y, 0.f, 0.f);

	_vector Dir = XMVectorSubtract(Target, Pos);
	_vector LenVec = XMVector3Length(Dir);
	_float Distance = XMVectorGetX(LenVec);

	_float move = m_fMoveSpeed * fTimeDelta;

	if (Distance <= move)
	{
		m_fX = m_fOrigin_Position.x;
		m_fY = m_fOrigin_Position.y;
		m_bLerpOff = false;
	}

	else
	{
		XMVECTOR DirNorm = XMVector3Normalize(Dir);
		m_fX = (Pos + DirNorm * move).m128_f32[0];
		m_fY = (Pos + DirNorm * move).m128_f32[1];
	}
}

void CPanelObject::Add_Element(wstring Name, CGameObject* pElement)
{
	if (pElement == nullptr)
	{
		return;
	}

	if (Find_Element(Name) != nullptr)
	{
		return;
	}

	m_Elements.push_back(pElement);
	m_ElementName.push_back(Name);
	m_Elements_map.emplace(Name, pElement);
	m_iElements_Count++;
}

_bool CPanelObject::Chack_Visible()
{
	if (m_pOwner->Get_Visible() == false)
	{
		m_bVisible = m_pOwner->Get_Visible();
	}
	else
	{
		m_bVisible = m_bActive;
	}
	return m_bVisible;
}

_bool CPanelObject::Chack_Element(_float2 Position, _float2 Target, _float Scale, _float TargetScale)
{
	_vector vPos = XMVectorSet(Position.x, Position.y, 0.f, 1.f);
	_vector vTarget = XMVectorSet(Target.x, Target.y, 0.f, 1.f);

	_float Dir = XMVectorGetX(XMVector3Length(XMVectorSubtract(vTarget, vPos)));

	if (Dir <= (Scale * 0.5f) + (TargetScale * 0.5f))
		return true;

	return false;
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

	m_Elements.clear();
	m_ElementName.clear();
	m_Elements_map.clear(); // map 비우기
}
