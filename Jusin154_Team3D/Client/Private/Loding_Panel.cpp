#include "pch.h"
#include "Loding_Panel.h"
#include "GameInstance.h"
#include "LoadingWidget.h"

CLoding_Panel::CLoding_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUIObject(pDevice, pContext)
{
}

CLoding_Panel::CLoding_Panel(const CLoding_Panel& rhs)
	:CUIObject(rhs)
{
}

HRESULT CLoding_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoding_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 960.f;
	Desc.fY = 540.f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	m_fCanvasAlpha = 1.f;
	m_fOwnerAlpha = 1.f;
	m_fAlpha = 1.f;
	m_iImageNum = 0;
	m_fSortZ = 0.1f;
	Visible(true);
	m_bActive = true;
	//m_vImageposSi = _float4(0.f, 0.f, 1792.f, 1024.f);
	m_iToolTip_Count = 0;
	ToolTipSetting();
	return S_OK;
}
void CLoding_Panel::Set_Image(_int Index)
{
	m_iImageNum = Index;
}
void CLoding_Panel::ToolTipSetting()
{
	ToolTip[0] = TEXT("알고 계셨나요?\nF9를 눌러 브레이크포인트를 설정할 수 있습니다.");
	ToolTip[1] = TEXT("알고 계셨나요?\nF12를 눌러 정의로 바로 이동할 수 있습니다!.");
	ToolTip[2] = TEXT("알고 계셨나요?\n디버깅 중에도 핫 리로드로 코드를 수정할 수 있습니다.");
	ToolTip[3] = TEXT("알고 계셨나요?\nCtrl + R, R로 변수 또는 함수 이름을 프로젝트 전체에서 일괄 변경할 수 있습니다.");
	ToolTip[4] = TEXT("알고 계셨나요?\n디버깅 중 변수에 마우스를 올리면 현재 값을 확인할 수 있습니다.");
}
void CLoding_Panel::Priority_Update(_float fTimeDelta)
{
}

void CLoding_Panel::Update(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_M))
	{
		m_iToolTip_Count++;
		if (m_iToolTip_Count > 4) // 4보다 크면 0으로
			m_iToolTip_Count = 0;
	}
	if (m_pGameInstance->Key_Down(DIK_N))
	{
		m_iToolTip_Count--;
		if (m_iToolTip_Count < 0) // 0보다 작으면 4로
			m_iToolTip_Count = 4;
	}

	__super::Update(fTimeDelta);
}

void CLoding_Panel::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

HRESULT CLoding_Panel::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::LODING_SCREEN)))) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Bind_Resources())) {
		return E_FAIL;
	}
	if (FAILED(m_pVIBufferCom->Render())) {
		return E_FAIL;
	}
	m_pGameInstance->Render_Text(TEXT("Font_size20"), TEXT("<- N M ->."), _float2(100.f, 750.f));
	
	m_pGameInstance->Render_Text(TEXT("Font_size20"), ToolTip[m_iToolTip_Count].c_str(), _float2(100.f, 800.f));

	return S_OK;
}

_vector CLoding_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CLoding_Panel::Bind_ShaderResources()
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
	if (FAILED(m_pDiffuse_TextureCom1->Bind_ShaderResource(m_pShaderCom, "g_Texture1", m_iImageNum)))
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
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageSipos1", &m_vImageposSi, sizeof(_float4))))
	//{
	//	return E_FAIL;
	//}
	//if (FAILED(m_pShaderCom->Bind_RawValue("g_fCurrent_Size", &m_vScale, sizeof(_float2))))
	//{
	//	return E_FAIL;
	//}
	return S_OK;
}

HRESULT CLoding_Panel::Ready_Components(void* pArg)
{
	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Screen_BG"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("LoadingScreen"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom1), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLoadingWidget>(g_iStaticLevel, ENUM_CLASS(LEVEL::LOADING), LAYER_UI, nullptr, this, reinterpret_cast<CLoadingWidget**>(&m_pLoadingWidget))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CLoding_Panel* CLoding_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoding_Panel* pInstance = new CLoding_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoding_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLoding_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CLoding_Panel* pInstance = new CLoding_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpell_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLoding_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pDiffuse_TextureCom1);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

#ifdef _DEBUG
void CLoding_Panel::Describe_Entity()
{
}

#endif // _DEBUG
