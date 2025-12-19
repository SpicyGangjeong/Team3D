#include "pch.h"
#include "SPellLeam_SpeedPointer.h"
#include "GameInstance.h"

CSPellLeam_SpeedPointer::CSPellLeam_SpeedPointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CElementObject(pDevice, pContext)
{
}

CSPellLeam_SpeedPointer::CSPellLeam_SpeedPointer(const CSPellLeam_SpeedPointer& rhs)
    :CElementObject(rhs)
{
}

HRESULT CSPellLeam_SpeedPointer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSPellLeam_SpeedPointer::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 476.f;
	Desc.fY = -253.f;
	Desc.fSizeX = 32.f;
	Desc.fSizeY = 32.f;
	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fTimeMult = 3.f;
	m_fAlpha = 1.f;
	m_fAlphaTime = 5.f;
	m_fMoveSpeed = 15.f;
	m_fAngle = XMConvertToRadians(0);
	Visible(false);
	return S_OK;
}

void CSPellLeam_SpeedPointer::Priority_Update(_float fTimeDelta)
{
}

void CSPellLeam_SpeedPointer::Update(_float fTimeDelta)
{
}

void CSPellLeam_SpeedPointer::Late_Update(_float fTimeDelta)
{
}

HRESULT CSPellLeam_SpeedPointer::Render()
{
    return S_OK;
}

_vector CSPellLeam_SpeedPointer::Get_WorldPostion()
{
    return _vector();
}

HRESULT CSPellLeam_SpeedPointer::Bind_ShaderResources()
{
    return S_OK;
}

HRESULT CSPellLeam_SpeedPointer::Ready_Components(void* pArg)
{
    return S_OK;
}

CSPellLeam_SpeedPointer* CSPellLeam_SpeedPointer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    return nullptr;
}

CGameObject* CSPellLeam_SpeedPointer::Clone(void* pArg, CGameObject* pOwner)
{
    return nullptr;
}

void CSPellLeam_SpeedPointer::Free()
{
}

void CSPellLeam_SpeedPointer::Describe_Entity()
{
}
