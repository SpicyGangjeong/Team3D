#include "pch.h"
#include "Troll_Weapon.h"

#include "GameInstance.h"
#include "TrailObject.h"
#include "EffectParts.h"

CTroll_Weapon::CTroll_Weapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CPartObject(pDevice, pContext)
{
}

CTroll_Weapon::CTroll_Weapon(const CTroll_Weapon& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CTroll_Weapon::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTroll_Weapon::Initialize(void* pArg)
{
	TROLLWEAPON_DESC* pDesc = static_cast<TROLLWEAPON_DESC*> (pArg);
	m_pSocketMatrices = pDesc->pSocketMatrices;

	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}

	m_pModelCom->Combined_BoneMatrix();
	_matrix socketMatrix = {};

	socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

	for (int i = 0; i < 3; ++i) {
		socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
	}

	m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	XMStoreFloat4x4(&m_HammerMatrix, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bone")) * m_pTransformCom->Get_XMWorldMatrix());	

#ifdef RELEASE_DEBUGGER
	m_pGripShape = (GeometricPrimitive::CreateSphere(m_pContext, 1.f, 10, false, false));
	m_pSubShape = (GeometricPrimitive::CreateSphere(m_pContext, 1.2f, 10, false, false));
#endif // RELEASE_DEBUGGER

	return S_OK;
}

void CTroll_Weapon::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->ResetVelocityVector();
	m_pModelCom->Combined_BoneMatrix();
	_matrix socketMatrix = {};

	socketMatrix = XMLoadFloat4x4(m_pSocketMatrices);

	for (int i = 0; i < 3; ++i) {
		socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
	}

	m_pTransformCom->Set_WorldMatrix(socketMatrix * XMLoadFloat4x4(m_pParentTransformCom->Get_WorldMatrixPtr()));
	XMStoreFloat4x4(&m_HammerMatrix, XMLoadFloat4x4(m_pModelCom->Get_BoneMatrixPtr("Bone")) * m_pTransformCom->Get_XMWorldMatrix());
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG
}

void CTroll_Weapon::Update(_float fTimeDelta)
{
	if (m_bDisolve) {
		m_fDisolveTime += fTimeDelta*0.5f;
		if (m_fDisolveTime >= 1.f)
		{
			m_bDisolve = false;
			m_bVisible = false;
		}
	}
}

void CTroll_Weapon::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CTroll_Weapon::Render()
{
	if (!m_pModelCom){
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	if (FAILED(Render_Disolve())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(i, m_pShaderCom))) {
			return E_FAIL;
		}


		if (FAILED(m_pModelCom->Begin(i, m_pShaderCom))) {
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

//#ifdef RELEASE_DEBUGGER
//	m_pGripShape->Draw(m_pTransformCom->Get_XMWorldMatrix(), m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW), m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ), DirectX::Colors::Green, nullptr, true);
//	m_pSubShape->Draw(XMLoadFloat4x4(&m_HammerMatrix), m_pGameInstance->Get_Transform_Matrix(D3DTS::VIEW), m_pGameInstance->Get_Transform_Matrix(D3DTS::PROJ), DirectX::Colors::Purple, nullptr, true);
//#endif // RELEASE_DEBUGGER

	{
		_bool bDisolve = false;
		_float zero = 0.f;
		m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_bool));
		m_pShaderCom->Bind_RawValue("g_fDisolveRatio", &zero, sizeof(_float));
	}

	return S_OK;
}

_vector CTroll_Weapon::Get_WorldPostion()
{
	return XMLoadFloat4((_float4*)&m_HammerMatrix.m[3][0]);
}

_matrix CTroll_Weapon::Get_WorldMatrix()
{
	return XMLoadFloat4x4(&m_HammerMatrix);
}

const _float4* CTroll_Weapon::Get_HammerPosition()
{
	return (_float4*)&m_HammerMatrix.m[3][0];
}

HRESULT CTroll_Weapon::Render_Disolve()
{
	if (FLT_EPSILON3 * 10 < m_fDisolveTime)
	{
		_bool bDisolve = true;
		_float fDisolveAmount = 0.1f;
		_float fDisolveEdgeWidth = 0.1f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_bDisolve", &bDisolve, sizeof(_bool)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveRatio", &m_fDisolveTime, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveAmount", &fDisolveAmount, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDisolveEdgeWidth", &fDisolveEdgeWidth, sizeof(_float)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Bind_GlobalSRV(m_pShaderCom, TEXT("GLOBAL_DISOLVE_NOISE_05"), "g_DeadDisolveTexture"))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CTroll_Weapon::Ready_Components()
{
	__super::Ready_Components(nullptr);


	/* Com_Model */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_Troll_Weapon_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom)))){
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NPC_PBR_ANIM,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))){
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CTroll_Weapon::Bind_ShaderResources()
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


CTroll_Weapon* CTroll_Weapon::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTroll_Weapon* pInstance = new CTroll_Weapon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTroll_Weapon");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CTroll_Weapon::Clone(void* pArg, CGameObject* pOwner)
{
	CTroll_Weapon* pInstance = new CTroll_Weapon(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTroll_Weapon");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CTroll_Weapon::Free()
{
	__super::Free();

	SAFE_RELEASE(m_pShaderCom);
	SAFE_RELEASE(m_pModelCom);

}
#ifdef _DEBUG

void CTroll_Weapon::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Troll_Hammer")) {
		_float4 vPos;
		_float4 vHammerWorldPos;

		_float4 vHammerRight;
		_float4 vHammerUp;
		_float4 vHammerLook;
		_float4 vHammerPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(STATE::POSITION));
		XMStoreFloat4(&vHammerWorldPos, Get_WorldPostion());

		XMStoreFloat4(&vHammerRight, XMLoadFloat4((_float4*)&m_HammerMatrix.m[0][0]));
		XMStoreFloat4(&vHammerUp, XMLoadFloat4((_float4*)&m_HammerMatrix.m[1][0]));
		XMStoreFloat4(&vHammerLook, XMLoadFloat4((_float4*)&m_HammerMatrix.m[2][0]));
		XMStoreFloat4(&vHammerPos, XMLoadFloat4((_float4*)&m_HammerMatrix.m[3][0]));

		GUI::Text("vPos, %.2f, %.2f, %.2f, %.2f", vPos.x, vPos.y, vPos.z, vPos.w);
		GUI::Text("vHammerWorldPos, %.2f, %.2f, %.2f, %.2f", vHammerWorldPos.x, vHammerWorldPos.y, vHammerWorldPos.z, vHammerWorldPos.w);
		GUI::Text("vHammerRight, %.2f, %.2f, %.2f, %.2f", vHammerRight.x, vHammerRight.y, vHammerRight.z, vHammerRight.w);
		GUI::Text("vHammerUp, %.2f, %.2f, %.2f, %.2f", vHammerUp.x, vHammerUp.y, vHammerUp.z, vHammerUp.w);
		GUI::Text("vHammerLook, %.2f, %.2f, %.2f, %.2f", vHammerLook.x, vHammerLook.y, vHammerLook.z, vHammerLook.w);
		GUI::Text("vHammerPos, %.2f, %.2f, %.2f, %.2f", vHammerPos.x, vHammerPos.y, vHammerPos.z, vHammerPos.w);
	}
	GUI::End();
}

#endif // _DEBUG
