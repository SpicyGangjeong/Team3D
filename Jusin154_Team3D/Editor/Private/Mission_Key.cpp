#include "pch.h"
#include "Mission_Key.h"
#include "GameInstance.h"

CMission_Key::CMission_Key(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CMission_Key::CMission_Key(const CMission_Key& rhs)
	:CElementObject(rhs)
{
}

HRESULT CMission_Key::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMission_Key::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -297.f;
	Desc.fY = 159.f;
	Desc.fSizeX = 40.f;
	Desc.fSizeY = 40.f;

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
	m_fAlphaTime = 3.f;
	AlphabetUV('V');
	return S_OK;
}

void CMission_Key::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CMission_Key::Update(_float fTimeDelta)
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

void CMission_Key::Late_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	if (m_bVisible) {
			m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
		__super::Late_Update(fTimeDelta);
	}
}

HRESULT CMission_Key::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::REMOVEUV)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}

	return S_OK;
}

_vector CMission_Key::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

_float4 CMission_Key::AlphabetUV(_tchar Alphabet)
{
	_uint Number = CMyTools::AlphabetToInt(Alphabet);

	_float2 fImage_Size = { 320.f, 384.f };

	_uint iXCount = 5;
	_uint iYCount = 6;

	_float frameWidth = 64.f;
	_float frameHeight = 64.f;

	_uint iframeX = Number % iXCount;
	_uint iframeY = Number / iXCount;

	_float2 UVStart;
	UVStart.x = iframeX * frameWidth / fImage_Size.x;
	UVStart.y = iframeY * frameHeight / fImage_Size.y;

	_float2 UVEnd;
	UVEnd.x = UVStart.x + (frameWidth / fImage_Size.x);
	UVEnd.y = UVStart.y + (frameHeight / fImage_Size.y);

	return m_vUV = _float4(UVStart.x, UVStart.y, UVEnd.x, UVEnd.y);
}

HRESULT CMission_Key::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fTime", &m_fTime, sizeof(_float))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageUV", &m_vUV, sizeof(_float4))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CMission_Key::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_Atlas_Keyboard"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CMission_Key* CMission_Key::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMission_Key* pInstance = new CMission_Key(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMission_Key");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CMission_Key::Clone(void* pArg, CGameObject* pOwner)
{
	CMission_Key* pInstance = new CMission_Key(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CMission_Key");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CMission_Key::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CMission_Key::Describe_Entity()
{
}
