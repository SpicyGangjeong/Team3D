#include "pch.h"
#include "LoadingWidget_Flame.h"
#include "GameInstance.h"

CLoadingWidget_Flame::CLoadingWidget_Flame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CElementObject(pDevice, pContext)
{
}

CLoadingWidget_Flame::CLoadingWidget_Flame(const CLoadingWidget_Flame& rhs)
	:CElementObject(rhs)
{
}

HRESULT CLoadingWidget_Flame::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoadingWidget_Flame::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = -237.f;
	Desc.fY = 90.f;
	Desc.fSizeX = 100.f;
	Desc.fSizeY = 100.f;

	m_pRect = { long(Desc.fX - Desc.fSizeX * 0.5f), long(Desc.fY - Desc.fSizeY * 0.5f), long(Desc.fX + Desc.fSizeX * 0.5f), long(Desc.fY + Desc.fSizeY * 0.5f) };

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	// UI의 알파값을 조절
	m_fAlpha = 1.f;
	m_fAlphaTime = 3.f;

	// 스프라이트 애니메이션의 정보
	m_fTimeMult = 3.f;		// 속도
	m_fDelayTime = 1.f;		// 딜레이
	m_fFrame = 0.2f;		// 프레임
	m_fEndTime = 1.8f;		// 한번 실행되는 시간
	m_iImageFrameX = 3;		// 이미지의 x 갯수
	m_iImageFrameY = 3;		// 이미지의 y 갯수

	// 이 친구는 FadeIn을 해주면 알아서 한번 깜빡이고 들어감
	// 조절을 할꺼면 나중에 제대로 확인을 한 다음에 할 예정
	Set_FadeIn();
	return S_OK;
}

void CLoadingWidget_Flame::Priority_Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}
	__super::Priority_Update(fTimeDelta);
}

void CLoadingWidget_Flame::Update(_float fTimeDelta)
{
	if (!__super::Chack_Visible())
	{
		return;
	}

	if (m_fAlpha >= 1.f)
	{
		if (m_fDelayTime >= 0.f)
		{
			m_fDelayTime -= fTimeDelta * m_fTimeMult;
		}
		else
		{
			m_fTime += fTimeDelta * m_fTimeMult;
		}

		if (m_fTime >= m_fEndTime)
		{
			m_fTime = 0.f;
			m_fDelayTime = 1.f;
			m_bFadeOut = true;
		}
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

	__super::Update(fTimeDelta);
}

void CLoadingWidget_Flame::Late_Update(_float fTimeDelta)
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

HRESULT CLoadingWidget_Flame::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::LODING)))) {
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

_vector CLoadingWidget_Flame::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CLoadingWidget_Flame::Bind_ShaderResources()
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFrame", &m_fFrame, sizeof(_float))))
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
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iImageCountX", &m_iImageFrameX, sizeof(_int))))
	{
		return E_FAIL;
	}	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCanvasAlpha", &m_fCanvasAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iImageCountY", &m_iImageFrameY, sizeof(_int))))
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CLoadingWidget_Flame::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Texture_LoadingWidget_Flame"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLoadingWidget_Flame* CLoadingWidget_Flame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoadingWidget_Flame* pInstance = new CLoadingWidget_Flame(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoadingWidget_Flame");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLoadingWidget_Flame::Clone(void* pArg, CGameObject* pOwner)
{
	CLoadingWidget_Flame* pInstance = new CLoadingWidget_Flame(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoadingWidget_Flame");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLoadingWidget_Flame::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CLoadingWidget_Flame::Describe_Entity()
{
}
#endif // _DEBUG
