#include "pch.h"
#include "DummyRect.h"
#include "GameInstance.h"

CDummyRect::CDummyRect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CDummyRect::CDummyRect(const CDummyRect& rhs)
	:CGameObject(rhs)
{
}

void CDummyRect::Priority_Update(_float fTimeDelta)
{
}

void CDummyRect::Update(_float fTimeDelta)
{
}

void CDummyRect::Late_Update(_float fTimeDelta)
{
	if (m_bVisible) {
		_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);
		m_pGameInstance->Add_RenderGroup(RENDER::UI, this, *vPos, m_pTransformCom->Get_Radius());
	}
}

HRESULT CDummyRect::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_POSTEX::UI)))) {
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

HRESULT CDummyRect::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix))){
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix))){
		return E_FAIL;
	}
	if (FAILED(m_pDiffuse_TextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CDummyRect::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}
	{
		if (FAILED(Add_Component<CVIBuffer_Rect>(g_iStaticLevel, &m_pVIBufferCom))) {
			return E_FAIL;
		}
		if (FAILED(Add_Asset_Component(g_iStaticLevel, TEXT("Dororong"), reinterpret_cast<CComponent**>(&m_pDiffuse_TextureCom), nullptr))) {
			return E_FAIL;
		}
		if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_POSTEX, (CComponent**)&m_pShaderCom, nullptr))) {
			return E_FAIL;
		}
	}

	CGameObject* asdf = this;
	return S_OK;
}

HRESULT CDummyRect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDummyRect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}
	
	_float3 vScale = { 300.f, 300.f, 1.f };
	m_pTransformCom->Set_Scale(vScale);

	D3D11_VIEWPORT       ViewportDesc{};
	_uint                iNumViewports = { 1 };

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));
	return S_OK;
}

CDummyRect* CDummyRect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDummyRect* pInstance = new CDummyRect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDummyRect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CDummyRect::Clone(void* pArg, CGameObject* pOwner)
{
	CDummyRect* pInstance = new CDummyRect(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDummyRect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CDummyRect::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pDiffuse_TextureCom);
	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pVIBufferCom);
}

void CDummyRect::Describe_Entity()
{
}
