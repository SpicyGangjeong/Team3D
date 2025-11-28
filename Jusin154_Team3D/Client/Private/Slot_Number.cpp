#include "pch.h"
#include "Slot_Number.h"
#include "GameInstance.h"

CSlot_Number::CSlot_Number(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CSlot_Number::CSlot_Number(const CSlot_Number& rhs)
	:CElementObject(rhs)
{
}

HRESULT CSlot_Number::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSlot_Number::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 35.f;
	Desc.fSizeY = 35.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	m_fAlpha = 1.f;
	m_fTimeMult = 3.f;
	m_fAlphaTime = 1.f;
	m_fOffSetX = 101.f;
	m_fOffSetY = 101.f;
	m_iCols = 4;
	UV();
	m_pVIBufferCom->Set_Cloned(true);
	m_pVIBufferCom->Set_Pos(350.f, 10.f, m_fOffSetX, m_fOffSetY, m_iCols);
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
	m_pVIBufferCom->Set_ImageUV(pUVDesc);
	m_bActive = true;
	return S_OK;
}

void CSlot_Number::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CSlot_Number::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_bFadeIn == true)
	{
		if (m_fAlpha <= 1.f)
			m_fAlpha += fTimeDelta * m_fAlphaTime;

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
	m_fTime += fTimeDelta * m_fTimeMult;
	__super::Update(fTimeDelta);
}

void CSlot_Number::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible)
	{
			m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CSlot_Number::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::ALPHABLEND))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

_vector CSlot_Number::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CSlot_Number::SizeUpX(_float fSizeX)
{
	m_fSizeX = fSizeX;
	m_pVIBufferCom->Set_SizeX(m_fSizeX);
}

void CSlot_Number::SizeUpY(_float fSizeY)
{
	m_fSizeY = fSizeY;
	m_pVIBufferCom->Set_SizeY(m_fSizeY);
}

void CSlot_Number::SizeUpdate(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_pVIBufferCom->Set_Size(m_fSizeX, m_fSizeY);
}

HRESULT CSlot_Number::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fOwnerAlpha", &m_fOwnerAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCanvasAlpha", &m_fCanvasAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSlot_Number::Ready_Components(void* pArg)
{
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_VIBuffer_Slot_Number_UI_Instance"), (CComponent**)&m_pVIBufferCom, nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_Atlas_Number"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIINSTANCE, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CSlot_Number::UV()
{
	pUVDesc[0].fUVStart = Meter_Index(1);
	pUVDesc[0].fUVEnd = Compute_UVY(1);
	pUVDesc[1].fUVStart = Meter_Index(2);
	pUVDesc[1].fUVEnd = Compute_UVY(2);
	pUVDesc[2].fUVStart = Meter_Index(3);
	pUVDesc[2].fUVEnd = Compute_UVY(3);
	pUVDesc[3].fUVStart = Meter_Index(4);
	pUVDesc[3].fUVEnd = Compute_UVY(4);
}

_float2 CSlot_Number::Meter_Index(_uint Number)
{
	m_fIamge_Size = { 320.f, 128.f };

	_uint iXCount = 5;
	_uint iYCount = 2;

	_float frameWidth = 64.f;
	_float frameHeight = 64.f;

	_uint frameX = Number % iXCount;
	_uint frameY = Number / iXCount;

	_float2 UVStart;
	UVStart.x = frameX * frameWidth / m_fIamge_Size.x;
	UVStart.y = frameY * frameHeight / m_fIamge_Size.y;

	return UVStart;
}

_float2 CSlot_Number::Compute_UVY(_uint Number)
{
	m_fIamge_Size = { 320.f, 128.f };

	_float frameWidth = 64.f;
	_float frameHeight = 64.f;

	// Start 구함
	_float2 UVStart = Meter_Index(Number);

	_float2 UVEnd;
	UVEnd.x = UVStart.x + (frameWidth / m_fIamge_Size.x);
	UVEnd.y = UVStart.y + (frameHeight / m_fIamge_Size.y);

	return UVEnd;
}

CSlot_Number* CSlot_Number::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSlot_Number* pInstance = new CSlot_Number(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSlot_Number");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CSlot_Number::Clone(void* pArg, CGameObject* pOwner)
{
	CSlot_Number* pInstance = new CSlot_Number(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSlot_Number");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CSlot_Number::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CSlot_Number::Describe_Entity()
{
}
