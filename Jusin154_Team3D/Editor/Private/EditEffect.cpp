#include "pch.h"
#include "EditEffect.h"

#include "GameInstance.h"
#include "Effect_Editor.h"

CEditEffect::CEditEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffectObject{ pDevice, pContext }
{
}

CEditEffect::CEditEffect(const CEditEffect& rhs)
	: CEffectObject(rhs)
{
}

HRESULT CEditEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEditEffect::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;


	return S_OK;
}

void CEditEffect::Priority_Update(_float fTimeDelta)
{

}

void CEditEffect::Update(_float fTimeDelta)
{



	if (m_isBillboard)
		m_pGameInstance->BillBoard(m_pTransformCom);

	if (m_pInstance_ModelCom == nullptr)
		return;

	m_pInstance_ModelCom->Drop(fTimeDelta);



}

void CEditEffect::Late_Update(_float fTimeDelta)
{

	if (m_pInstance_ModelCom == nullptr)
		return;

	_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);

	if (m_isBlur == true)
		m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this, *vPos, m_pTransformCom->Get_Radius());

	m_pGameInstance->Add_RenderGroup(m_eRenderOrder, this, *vPos, m_pTransformCom->Get_Radius());
}

void CEditEffect::Reference_Mat_For_EditEffect()
{


	const char* pCompute[] = { "DIFFUSE" , "MASK", "NOISE", "DISSOLVE" };

	if (ImGui::Combo("OPTION", &m_iSelectTextureNum, pCompute, 4))
	{
	}
	switch (m_iSelectTextureNum)
	{
	case 0:
		dynamic_cast<CEffect_Editor*>(m_pOwner)->Reference_Mat_For_EditEffect((CComponent**)&m_pDiffuse_TextureCom, this);
		break;
	case 1:
		dynamic_cast<CEffect_Editor*>(m_pOwner)->Reference_Mat_For_EditEffect((CComponent**)&m_pMasking_TextureCom, this);
		break;
	case 2:
		dynamic_cast<CEffect_Editor*>(m_pOwner)->Reference_Mat_For_EditEffect((CComponent**)&m_pNoise_TextureCom, this);
		break;
	case 3:
		dynamic_cast<CEffect_Editor*>(m_pOwner)->Reference_Mat_For_EditEffect((CComponent**)&m_pDissolve_TextureCom, this);
		break;
	default:
		break;
	}


}

HRESULT CEditEffect::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Asset_Component(g_iStaticLevel, FX_INSTANCE_MODEL, (CComponent**)&m_pShaderCom, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}


CEditEffect* CEditEffect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEditEffect* pInstance = new CEditEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEditEffect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

CGameObject* CEditEffect::Clone(void* pArg, CGameObject* pOwner)
{
	CEditEffect* pInstance = new CEditEffect(*this);
	pInstance->m_pOwner = pOwner;
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEditEffect");
		SAFE_RELEASE(pInstance);
	}

	return pInstance;
}

void CEditEffect::Free()
{
	__super::Free();
}

void CEditEffect::Describe_Entity()
{
	//여기서 모델, 텍스쳐, 선택할 수 있도록 함

	const char* pLerp[] = { "Linear" , "EaseInQuad", "EaseOutQuad", "EaseInCubic" , "EaseOutCubic" , "EaseInOutSin" , "EaseInBack" , "Expo" , "Circle" };
	const char* pRenderNames[] = { "PRIORITY" , "SHADOW", "NONBLEND", "BLUR" , "NONLIGHT" ,"EFFECT", "BLEND" , "UI" };

	_int iCurrentItem = static_cast<_int>(m_eRenderOrder);

	if (ImGui::Combo("Render Order", &iCurrentItem, pRenderNames, ENUM_CLASS(RENDER::END)))
	{
		m_eRenderOrder = static_cast<RENDER>(iCurrentItem);
	}

	m_pTransformCom->Describe_Entity();

	GUI::Checkbox("Diffuse", &m_isDiffuse);
	GUI::Checkbox("Masking", &m_isMasking);
	GUI::Checkbox("Dissolve", &m_isDissolve);
	GUI::Checkbox("Noise", &m_isNoise);


	if (GUI::Checkbox("Billboard", &m_isBillboard))
	{
		m_pTransformCom->Rotation(0.f, 0.f, 0.f);
	}

	GUI::ColorEdit4("MixColor", (_float*)&m_vColor);

	ImGui::PushItemWidth(80);

	m_pShaderCom->Describe_Entity();

	ImGui::PopItemWidth();

	if (GUI::TreeNode("BLUR"))
	{
		GUI::Checkbox("Blur", &m_isBlur);

		ImGui::PushItemWidth(80);
		GUI::DragFloat("BlurIntensity", &m_fBlurIntensity, 0.005f, 0.f, 1.f);
		GUI::DragInt("BlurWeight", &m_iBlurWeight, 1.f, 0, 32);

		
		ImGui::PopItemWidth();

		GUI::TreePop();
	}


	if (GUI::TreeNode("EMISSIVE"))
	{
		const char* pCompute[] = {"NONE",  "DIV" , "PLUS", "SUBSTRACT", "MULTY" , "EQUL" };

		_int iColorOption = (_int)m_fColorOption;

		if (ImGui::Combo("OPTION", &iColorOption, pCompute , 6))
		{
			m_fColorOption = (_float)iColorOption;
		}

		ImGui::PushItemWidth(80);
		GUI::DragFloat("EmissiveCutAlpha", &m_fEmissiveCutAlpha, 0.005f, 0.f, 1.f);
		ImGui::PopItemWidth();

		GUI::ColorEdit4("Emissive", (_float*)&m_vEmissive);


		GUI::TreePop();
	}



	if (GUI::TreeNode("MODEL"))
	{
		CInstance_Model::INSTANCE_DESC InstanceDesc = {};

		if (m_pInstance_ModelCom != nullptr)
			InstanceDesc = m_pInstance_ModelCom->Get_EffectValue();

		m_pGameInstance->Asset_Description<CInstance_Model>(ENUM_CLASS(LEVEL::EFFECT), "INSTANCE_MODEL", (CComponent**)&m_pInstance_ModelCom, &InstanceDesc, this);

		if (m_pInstance_ModelCom != nullptr)
		{
			m_pInstance_ModelCom->Describe_Entity();
		}

		GUI::TreePop();
	}


	if (m_isDiffuse == true)
	{
		if (GUI::TreeNode("DIFFUSE"))
		{


			ImGui::PushItemWidth(80);
			GUI::Checkbox("DiffuseUVMove", &m_isDiffuseUVMove);
			GUI::InputFloat2("DiffuseUVCutting", (_float*)&m_vUVCutting);

			GUI::Spacing();

			GUI::DragFloat2("DiffuseUVGainAmount", (_float*)&m_vDiffuseUVGainAmount, 0.01f);
	


			_int iDiffuseMoveLerpOption = (_int)m_iDiffuseMoveLerpOption;

			if (ImGui::Combo("Lerp DiffuseMove Option", &iDiffuseMoveLerpOption, pLerp, 9))
			{
				m_iDiffuseMoveLerpOption = iDiffuseMoveLerpOption;
			}

			GUI::Spacing();

			ImGui::PopItemWidth();

			if (GUI::TreeNode("DIFFUSE_TEX"))
			{
				m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "DIFFUSE_TEXTURE", (CComponent**)&m_pDiffuse_TextureCom, nullptr, this);
				GUI::TreePop();
			}

			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}


	if (m_isMasking == true)
	{
		if (GUI::TreeNode("MASKING"))
		{

			GUI::InputFloat2("MaskUVCutting", (_float*)&m_vUVMaskCutting);
			GUI::Checkbox("MaskUVMove", &m_isMaskUVMove);

			ImGui::PushItemWidth(80);
			GUI::DragFloat2("MaskingUVGainAmount", (_float*)&m_vMaskingUVGainAmount, 0.01f);
			ImGui::PopItemWidth();


			GUI::Spacing();


			_int iMaskMoveLerpOption = (_int)m_iMaskMoveLerpOption;

			if (ImGui::Combo("Lerp MaskMove Option", &iMaskMoveLerpOption, pLerp, 9))
			{
				m_iMaskMoveLerpOption = iMaskMoveLerpOption;
			}
		
		
			GUI::Spacing();

			if (GUI::TreeNode("MASKING_TEX"))
			{
				m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "MASKING_TEXTURE", (CComponent**)&m_pMasking_TextureCom, nullptr, this);
				GUI::TreePop();
			}
			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}

	if (m_isDissolve == true)
	{
		if (GUI::TreeNode("DISSOLVE"))
		{
			if (GUI::TreeNode("DISSOLV_TEX"))
			{
				m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "DISSOLVE_TEXTURE", (CComponent**)&m_pDissolve_TextureCom, nullptr, this);
				GUI::TreePop();
			}
			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}

	}
	if (m_isNoise == true)
	{
		if (GUI::TreeNode("NOISE"))
		{
			ImGui::PushItemWidth(80);

			GUI::DragFloat("NoiseDistortionIntensity", &m_fNoiseDistortionIntensity, 0.005f, 0.f, 1.f);
			
			GUI::Spacing();

			GUI::DragFloat2("DiffuseNoiseUVGainAmount", (_float*)&m_vDiffuseNoiseUVGainAmount, 0.01f);

			_int iDiffuseNoiseMoveLerpOption = (_int)m_iDiffuseNoiseMoveLerpOption;

			if (ImGui::Combo("Lerp DiffuseNoiseMove Option", &iDiffuseNoiseMoveLerpOption, pLerp, 9))
			{
				m_iDiffuseNoiseMoveLerpOption = iDiffuseNoiseMoveLerpOption;
			}

			GUI::Spacing();

			GUI::DragFloat2("MaskNoiseUVGainAmount", (_float*)&m_vMaskNoiseUVGainAmount, 0.01f);

			_int iMaskNoiseMoveLerpOption = (_int)m_iMaskNoiseMoveLerpOption;

			if (ImGui::Combo("Lerp MaskNoiseMove Option", &iMaskNoiseMoveLerpOption, pLerp, 9))
			{
				m_iMaskNoiseMoveLerpOption = iMaskNoiseMoveLerpOption;
			}
			
			GUI::Spacing();

			ImGui::PopItemWidth();

			if (GUI::TreeNode("NOISE_TEX"))
			{
				m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NOISE_TEXTURE", (CComponent**)&m_pNoise_TextureCom, nullptr, this);
				GUI::TreePop();
			}

			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}



}
