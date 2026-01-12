#include "pch.h"
#include "Wand.h"

#include "GameInstance.h"
#include "TrailObject.h"
#include "EffectParts.h"

CWand::CWand(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CWand::CWand(const CWand& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CWand::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWand::Initialize(void* pArg)
{
	WAND_DESC* pDesc = static_cast<WAND_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))){
		return E_FAIL;
	}


	if (FAILED(Ready_Components())){
		return E_FAIL;
	}

#ifdef _DEBUG
	m_pGripShape = (GeometricPrimitive::CreateSphere(m_pContext, 0.1f, 10, false, false));
	m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, 0.1f, 10, false, false));
#endif // _DEBUG

	return S_OK;
}

void CWand::Priority_Update(_float fTimeDelta)
{
	m_pModelCom->Combined_BoneMatrix();
#ifdef _DEBUG
	Describe_Entity();

#endif // _DEBUG
}

void CWand::Update(_float fTimeDelta)
{


}

void CWand::Late_Update(_float fTimeDelta)
{

	_matrix socketMatrix = {};

	socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

	for (int i = 0; i < 3; ++i) {
		socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
	}

	m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	XMStoreFloat4x4(&m_pWandTipMatrix, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Effect")) * m_pTransformCom->Get_XMWorldMatrix());

	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CWand::Render()
{


	if (!m_bVisible)
		return S_OK;

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(ENUM_CLASS(SHADER_PASS_ANIM::DEFAULT)))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}
#ifdef _DEBUG
	//m_pGripShape->Draw(m_pTransformCom->Get_XMWorldMatrix(), m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW), m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ), DirectX::Colors::Green, nullptr, true);
	//m_pSubShape->Draw(XMLoadFloat4x4(&m_pWandTipMatrix), m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW), m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ), DirectX::Colors::Purple, nullptr, true);
#endif // _DEBUG
	
	return S_OK;
}

_vector CWand::Get_WorldPostion()
{
	return XMLoadFloat4((_float4*)&m_pWandTipMatrix.m[3][0]);
}

_matrix CWand::Get_WorldMatrix()
{
	return XMLoadFloat4x4(&m_pWandTipMatrix);
}


HRESULT CWand::Ready_Components()
{
	__super::Ready_Components(nullptr);


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Wand_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_MESH,
		reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CWand::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_PrevWorldMatrix", m_pTransformCom->Get_PrevWorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevViewMatrix", D3DTS::VIEW))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_PrevMatrix(m_pShaderCom, "g_PrevProjMatrix", D3DTS::PROJ))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW)))) {
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ)))) {
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_float)))) {
		return E_FAIL;
	}

	return S_OK;
}


CWand* CWand::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWand* pInstance = new CWand(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWand");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CWand::Clone(void* pArg, CGameObject* pOwner)
{
	CWand* pInstance = new CWand(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWand");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CWand::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CWand::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("WAND")) {
		_float4 vPos;
		_float4 vTipWorldPos;

		_float4 vTipRight;
		_float4 vTipUp;
		_float4 vTipLook;
		_float4 vTipPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
		XMStoreFloat4(&vTipWorldPos, Get_WorldPostion());

		XMStoreFloat4(&vTipRight, XMLoadFloat4((_float4*)&m_pWandTipMatrix.m[0][0]));
		XMStoreFloat4(&vTipUp, XMLoadFloat4((_float4*)&m_pWandTipMatrix.m[1][0]));
		XMStoreFloat4(&vTipLook, XMLoadFloat4((_float4*)&m_pWandTipMatrix.m[2][0]));
		XMStoreFloat4(&vTipPos, XMLoadFloat4((_float4*)&m_pWandTipMatrix.m[3][0]));

		GUI::Text("vPos, %.2f, %.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z, vPos.w);
		GUI::Text("vTipWorldPos, %.2f, %.2f, %.2f, %.2f", vTipWorldPos.x, vTipWorldPos.y, vTipWorldPos.z, vTipWorldPos.w);
		GUI::Text("vTipRight, %.2f, %.2f, %.2f, %.2f", vTipRight.x, vTipRight.y, vTipRight.z, vTipRight.w);
		GUI::Text("vTipUp, %.2f, %.2f, %.2f, %.2f", vTipUp.x, vTipUp.y, vTipUp.z, vTipUp.w);
		GUI::Text("vTipLook, %.2f, %.2f, %.2f, %.2f", vTipLook.x, vTipLook.y, vTipLook.z, vTipLook.w);
		GUI::Text("vTipPos, %.2f, %.2f, %.2f, %.2f", vTipPos.x, vTipPos.y, vTipPos.z, vTipPos.w);
	}
	GUI::End();
}

#endif // _DEBUG
