#include "pch.h"
#include "Effect_Editor.h"

#include "GameInstance.h"
#include "EditEffect.h"


CEffect_Editor::CEffect_Editor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CEffect_Editor::CEffect_Editor(const CEffect_Editor& rhs)
	: CEffectObject(rhs)
{
}

HRESULT CEffect_Editor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Editor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Child()))
		return E_FAIL;


	return S_OK;
}

void CEffect_Editor::Priority_Update(_float fTimeDelta)
{

}

void CEffect_Editor::Update(_float fTimeDelta)
{
	Describe_Entity();
}

void CEffect_Editor::Late_Update(_float fTimeDelta)
{

}

HRESULT CEffect_Editor::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CEffect_Editor::Ready_Child()
{
	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer<CEditEffect>(ENUM_CLASS(LEVEL::EFFECT), NEXT_LEVEL, LAYER_EFFECT , nullptr ,this , &m_pEditEffect)))
		return E_FAIL;

	return S_OK;
}


CEffect_Editor* CEffect_Editor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Editor* pInstance = new CEffect_Editor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Editor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Editor::Clone(void* pArg, CGameObject* pOwner)
{
	CEffect_Editor* pInstance = new CEffect_Editor(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Editor");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEffect_Editor::Free()
{
	__super::Free();
}

void CEffect_Editor::Describe_Entity()
{
	ImGui::Begin("Effect Editor");


	if (m_pEditEffect != nullptr)
		m_pEditEffect->Describe_Entity();

	ImGui::End();
}
