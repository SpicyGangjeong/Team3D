#include "pch.h"
#include "NPC_Ollivander.h"

#include "GameInstance.h"
#include "InfoInstance.h"
#include "Player.h"

CNPC_Ollivander::CNPC_Ollivander(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnit(pDevice, pContext)
{
}

CNPC_Ollivander::CNPC_Ollivander(const CNPC_Ollivander& Prototype)
	: CUnit(Prototype),
	m_pInfoInstance(CInfoInstance::GetInstance())
{
}

void CNPC_Ollivander::Priority_Update(_float fTimeDelta)
{
	m_pTransformCom->RewindMomentum();
	__super::Priority_Update(fTimeDelta);
}

void CNPC_Ollivander::Update(_float fTimeDelta)
{
	m_pFSM->Update_State(fTimeDelta);
	if (m_pModelCom->IsFinishedAnim()) {
		m_pModelCom->Set_AnimationIndex(m_pGameInstance->Random_Int(0, 19), false);
	}
	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom);
	__super::Update(fTimeDelta);
#ifdef _DEBUG
	Describe_Entity();
#endif // _DEBUG

}

void CNPC_Ollivander::Late_Update(_float fTimeDelta)
{
	m_pTransformCom->Set_State(STATE::POSITION, m_pTransformCom->Get_EstimatedPositionByMomentum());

	//m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	//m_pGameInstance->Add_RenderGroup(RENDER::SHADOW, this);

	__super::Late_Update(fTimeDelta);
}

HRESULT CNPC_Ollivander::Render()
{
	if (FAILED(Bind_ShaderResources())) {
		return E_FAIL;
	}

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
			return E_FAIL;
		}

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

	return S_OK;
}

HRESULT CNPC_Ollivander::Render_Shadow(SHADOW eType)
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrixPtr()))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ViewMatrix", D3DTS::VIEW, eType))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_Shadow_Resource(m_pShaderCom, "g_ProjMatrix", D3DTS::PROJ, eType))) {
		return E_FAIL;
	}
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices"))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Begin(i, m_pShaderCom))) {
			return E_FAIL;
		}
		if (FAILED(m_pModelCom->Render(i))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CNPC_Ollivander::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"))) {
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

HRESULT CNPC_Ollivander::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_Ollivander::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	if (FAILED(Ready_Components(pArg))) {
		return E_FAIL;
	}
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(40.f, 4.f, 68.9f, 1.f));
	m_pModelCom->Set_AnimationIndex(m_pGameInstance->Random_Int(0, 19), false);
	return S_OK;
}

HRESULT CNPC_Ollivander::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	/* Com_Shader */
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, FX_NPC_PBR_ANIM,
		reinterpret_cast<CComponent**>(&m_pShaderCom)))) {
		return E_FAIL;
	}
	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Prototype_Component_GerboldOlivander_Model"),
		reinterpret_cast<CComponent**>(&m_pModelCom)))) {
		return E_FAIL;
	}

	return S_OK;
}

CNPC_Ollivander* CNPC_Ollivander::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CNPC_Ollivander* pInstance = new CNPC_Ollivander(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNPC_Ollivander");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

CGameObject* CNPC_Ollivander::Clone(void* pArg, CGameObject* pOwner)
{
	CNPC_Ollivander* pInstance = new CNPC_Ollivander(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CNPC_Ollivander");
		SAFE_RELEASE(pInstance);
	}
	return pInstance;
}

void CNPC_Ollivander::Free()
{
	__super::Free();
}
#ifdef _DEBUG

void CNPC_Ollivander::Describe_Entity()
{
	GUI::Begin("UNIT", 0, IMGUI_GLOBAL_BEGIN_FLAG);
	if (GUI::CollapsingHeader("Ollivander")) {
		m_pModelCom->Describe_Entity();
		if (ImGui::TreeNode("ANIM STATE")) {

			for (auto& pState : m_States)
			{
				if (ImGui::Button(to_string(pState.first).c_str()))
				{
					m_pFSM->Change_State(pState.first);
				}
			}

			GUI::Text(to_string(m_pModelCom->Get_CurrentTrackProgressRatio()).c_str());

			ImGui::TreePop();
		}
	}
	GUI::End();
}

#endif // _DEBUG
