#include "pch.h"
#include "Loading_Panel.h"
#include "GameInstance.h"
#include "LoadingWidget.h"
#include "LoadingWidget_Flame.h"

CLoading_Panel::CLoading_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPanelObject(pDevice, pContext)
{
}

CLoading_Panel::CLoading_Panel(const CLoading_Panel& rhs)
	:CPanelObject(rhs)
{
}

HRESULT CLoading_Panel::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLoading_Panel::Initialize(void* pArg)
{
	CUIObject::UIOBJECT_DESC	Desc{};

	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = 100.f;
	Desc.fSizeY = 100.f;

	if (FAILED(__super::Initialize(&Desc)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg)))
	{
		return E_FAIL;
	}
	if (FAILED(Ready_Element(pArg)))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CLoading_Panel::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CLoading_Panel::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CLoading_Panel::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
		_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this, *vPos, m_pTransformCom->Get_Radius());
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CLoading_Panel::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_UIEDITOR::DEFAULT)))) {
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

_vector CLoading_Panel::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

HRESULT CLoading_Panel::Bind_ShaderResources()
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
	return S_OK;
}

HRESULT CLoading_Panel::Ready_Components(void* pArg)
{

	if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(ENUM_CLASS(LEVEL::UI), TEXT("Prototype_Texture_Keyboard_0"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_UIEDITOR, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLoading_Panel::Ready_Element(void* pArg)
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLoadingWidget>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CLoadingWidget**>(&m_pLoadingWidget))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("LoadingWidget"), m_pLoadingWidget);
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CLoadingWidget_Flame>(g_iStaticLevel, NEXT_LEVEL, LAYER_UI, nullptr, this, reinterpret_cast<CLoadingWidget_Flame**>(&m_pLoadingWidget_Flame))))
	{
		return E_FAIL;
	}
	Add_Element(TEXT("LoadingWidget_Flame"), m_pLoadingWidget_Flame);

	return S_OK;
}

CLoading_Panel* CLoading_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLoading_Panel* pInstance = new CLoading_Panel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLoading_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CLoading_Panel::Clone(void* pArg, CGameObject* pOwner)
{
	CLoading_Panel* pInstance = new CLoading_Panel(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLoading_Panel");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CLoading_Panel::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CLoading_Panel::Describe_Entity()
{
}
