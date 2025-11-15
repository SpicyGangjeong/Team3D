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



	if (m_EffectInfo.isBillboard)
		m_pGameInstance->BillBoard(m_pTransformCom);

	if (m_pInstance_ModelCom == nullptr)
		return;

	m_pInstance_ModelCom->Drop(fTimeDelta);



}

void CEditEffect::Late_Update(_float fTimeDelta)
{

	if (m_pInstance_ModelCom == nullptr)
		return;

	ASSERT_JINHO(false && 이거 고쳐야됨)
	//{
	//	if (m_pGameInstance->isIn_WorldFrustum(Get_WorldPostion(), m_pTransformCom->Get_Radius())) {
	//		m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
	//	}
	//}
	//{
	//	_float4* vPos = (_float4*)(m_pTransformCom->Get_WorldMatrixPtr()->m[3]);

	//	if (m_EffectInfo.isBlur == true)
	//		m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this, *vPos, m_pTransformCom->Get_Radius());

	//	m_pGameInstance->Add_RenderGroup(m_EffectInfo.eRenderOrder, this, *vPos, m_pTransformCom->Get_Radius());
	//}
}

void CEditEffect::Reference_Mat_For_EditEffect()
{

	string strName = {};

	const char* pCompute[] = { "DIFFUSE" , "MASK", "NOISE", "DISSOLVE" };

	if (ImGui::Combo("OPTION", &m_iSelectTextureNum, pCompute, 4))
	{
	}
	switch (m_iSelectTextureNum)
	{
	case 0:
		strName = dynamic_cast<CEffect_Editor*>(m_pOwner)->Reference_Mat_For_EditEffect((CComponent**)&m_pDiffuse_TextureCom, this);

		if (strName != "")
		{
			m_strDiffuseName = strName;
		}
		break;
	case 1:
		strName =dynamic_cast<CEffect_Editor*>(m_pOwner)->Reference_Mat_For_EditEffect((CComponent**)&m_pMasking_TextureCom, this);

		if (strName != "")
		{
			m_strMaskingName = strName;
		}
		break;
	case 2:
		strName = dynamic_cast<CEffect_Editor*>(m_pOwner)->Reference_Mat_For_EditEffect((CComponent**)&m_pNoise_TextureCom, this);
		
		if (strName != "")
		{
			m_strNoiseName = strName;
		}
		break;
	case 3:
		strName = dynamic_cast<CEffect_Editor*>(m_pOwner)->Reference_Mat_For_EditEffect((CComponent**)&m_pDissolve_TextureCom, this);

		if (strName != "")
		{
			m_strDissolveName = strName;
		}

		break;
	default:
		break;
	}


}

HRESULT CEditEffect::Save_Effect(const _char* pPath)
{

	_string strPerfectFilePath = pPath;
	strPerfectFilePath += ".bin";
	
	HANDLE	hFile = CreateFile(CMyTools::ToWstring(strPerfectFilePath).c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL
	);


	if (hFile == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"오브젝트 저장 실패", L"System Message", MB_OK);
		return E_FAIL;
	}


	
	DWORD	dwByte(0);

	if (m_pLightCom != nullptr) 
	{
		m_EffectInfo.LightDesc = *m_pLightCom->Get_LightDesc();
	}


	if (!WriteFile(hFile, &m_EffectInfo, sizeof(EFFECT_INFO), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (m_EffectInfo.isDiffuse)
	{
		size_t iComponentLength = m_strDiffuseName.length();
		const _char* pFileComponentPath = m_strDiffuseName.c_str();

		if (!WriteFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			if (!WriteFile(hFile, pFileComponentPath, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				return E_FAIL;
			}
		}
	}

	if (m_EffectInfo.isNoise)
	{
		size_t iComponentLength = m_strNoiseName.length();
		const _char* pFileComponentPath = m_strNoiseName.c_str();

		if (!WriteFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			if (!WriteFile(hFile, pFileComponentPath, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				return E_FAIL;
			}
		}
	}

	if (m_EffectInfo.isMasking)
	{
		size_t iComponentLength = m_strMaskingName.length();
		const _char* pFileComponentPath = m_strMaskingName.c_str();

		if (!WriteFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			if (!WriteFile(hFile, pFileComponentPath, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				return E_FAIL;
			}
		}
	}

	if (m_EffectInfo.isDissolve)
	{
		size_t iComponentLength = m_strDissolveName.length();
		const _char* pFileComponentPath = m_strDissolveName.c_str();

		if (!WriteFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			if (!WriteFile(hFile, pFileComponentPath, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				return E_FAIL;
			}
		}
	}

	if (m_EffectInfo.isEmissive)
	{
		size_t iComponentLength = m_strEmissiveName.length();
		const _char* pFileComponentPath = m_strEmissiveName.c_str();

		if (!WriteFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
			return E_FAIL;
		}

		if (iComponentLength != 0)
		{
			if (!WriteFile(hFile, pFileComponentPath, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
				return E_FAIL;
			}
		}
	}

	size_t iComponentLength = m_strModelName.length();
	const _char* pFileComponentPath = m_strModelName.c_str();

	if (!WriteFile(hFile, &iComponentLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (iComponentLength != 0)
	{
		if (!WriteFile(hFile, pFileComponentPath, sizeof(_char) * ((DWORD)iComponentLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}
	}

	m_pInstance_ModelCom->Save_InstanceModel(hFile);

	CloseHandle(hFile);

	return S_OK;
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

	_int iCurrentItem = static_cast<_int>(m_EffectInfo.eRenderOrder);

	if (ImGui::Combo("Render Order", &iCurrentItem, pRenderNames, ENUM_CLASS(RENDER::END)))
	{
		m_EffectInfo.eRenderOrder = static_cast<RENDER>(iCurrentItem);
	}

	m_pTransformCom->Describe_Entity();

	GUI::Checkbox("Diffuse", &m_EffectInfo.isDiffuse);
	GUI::Checkbox("Masking", &m_EffectInfo.isMasking);
	GUI::Checkbox("Dissolve", &m_EffectInfo.isDissolve);
	GUI::Checkbox("Noise", &m_EffectInfo.isNoise);


	if (GUI::Checkbox("Billboard", &m_EffectInfo.isBillboard))
	{
		m_pTransformCom->Rotation(0.f, 0.f, 0.f);
	}

	GUI::ColorEdit4("MixColor", (_float*)&m_EffectInfo.vColor);

	ImGui::PushItemWidth(80);

	m_pShaderCom->Describe_Entity();

	ImGui::PopItemWidth();

	if (GUI::TreeNode("BLUR"))
	{
		GUI::Checkbox("Blur", &m_EffectInfo.isBlur);

		ImGui::PushItemWidth(80);
		GUI::DragFloat("BlurIntensity", &m_EffectInfo.fBlurIntensity, 0.005f, 0.f, 1.f);
		GUI::DragInt("BlurWeight", &m_EffectInfo.iBlurWeight, 1.f, 0, 32);

		
		ImGui::PopItemWidth();

		GUI::TreePop();
	}


	if (GUI::TreeNode("EMISSIVE"))
	{
		const char* pCompute[] = {"NONE",  "DIV" , "PLUS", "SUBSTRACT", "MULTY" , "EQUL" };

		_int iColorOption = (_int)m_EffectInfo.fColorOption;

		if (ImGui::Combo("OPTION", &iColorOption, pCompute , 6))
		{
			m_EffectInfo.fColorOption = (_float)iColorOption;
		}

		ImGui::PushItemWidth(80);
		GUI::DragFloat("EmissiveCutAlpha", &m_EffectInfo.fEmissiveCutAlpha, 0.005f, 0.f, 1.f);
		ImGui::PopItemWidth();

		GUI::ColorEdit4("Emissive", (_float*)&m_EffectInfo.vEmissive);

		GUI::Checkbox("EmissiveTex", &m_EffectInfo.isEmissive);

		if (m_EffectInfo.isEmissive)
		{
			if (GUI::TreeNode("EMISSIVE_TEX"))
			{
				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "EMISSIVE_TEXTURE", (CComponent**)&m_pEmissive_TextureCom, nullptr, this);

				if (strName != "") {
					m_strEmissiveName = strName;
				}


				GUI::TreePop();
			}
		}


		GUI::TreePop();
	}

	if (GUI::TreeNode("LIGHT"))
	{
		if (GUI::Button("ADD LIGHT"))
		{
			if (m_pLightCom != nullptr)
			{
				GUI::TreePop();
				return;
			}
			LIGHT_DESC			LightDesc{};

			LightDesc.eType = LIGHT::POINT;
			LightDesc.vDiffuse = _float4(0.0f, 0.0f, 0.0f, 0.f);
			LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 0.f);
			LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
			LightDesc.pPosition = m_pTransformCom->Get_StatePtr(STATE::POSITION);
			LightDesc.iLevel = NEXT_LEVEL;

			/* Com_Light*/
			if (FAILED(Add_Component<CLight>(g_iStaticLevel, &m_pLightCom, &LightDesc)))
			{
				GUI::TreePop();
				return;
			}
		}

		if (m_pLightCom != nullptr)
		{
			m_pLightCom->Describe_Entity();
		}

		GUI::TreePop();
	}

	if (GUI::TreeNode("MODEL"))
	{
		CInstance_Model::INSTANCE_DESC InstanceDesc = {};

		if (m_pInstance_ModelCom != nullptr)
			InstanceDesc = m_pInstance_ModelCom->Get_EffectValue();

		_string strName  = m_pGameInstance->Asset_Description<CInstance_Model>(ENUM_CLASS(LEVEL::EFFECT), "INSTANCE_MODEL", (CComponent**)&m_pInstance_ModelCom, &InstanceDesc, this);

		if (strName != "") {
			m_strModelName = strName;
		}

		if (m_pInstance_ModelCom != nullptr)
		{
			m_pInstance_ModelCom->Describe_Entity();
		}

		GUI::TreePop();
	}





	if (m_EffectInfo.isDiffuse == true)
	{
		if (GUI::TreeNode("DIFFUSE"))
		{


			ImGui::PushItemWidth(80);
			GUI::Checkbox("DiffuseUVMove", &m_EffectInfo.isDiffuseUVMove);
			GUI::InputFloat2("DiffuseUVCutting", (_float*)&m_EffectInfo.vUVCutting);

			GUI::Spacing();

			GUI::DragFloat2("DiffuseUVGainAmount", (_float*)&m_EffectInfo.vDiffuseUVGainAmount, 0.01f);
	


			_int iDiffuseMoveLerpOption = (_int)m_EffectInfo.iDiffuseMoveLerpOption;

			if (ImGui::Combo("Lerp DiffuseMove Option", &iDiffuseMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iDiffuseMoveLerpOption = iDiffuseMoveLerpOption;
			}

			GUI::Spacing();

			ImGui::PopItemWidth();

			if (GUI::TreeNode("DIFFUSE_TEX"))
			{
				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "DIFFUSE_TEXTURE", (CComponent**)&m_pDiffuse_TextureCom, nullptr, this);
				
				if (strName != "") {
					m_strDiffuseName = strName;
				}

				
				GUI::TreePop();
			}

			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}


	if (m_EffectInfo.isMasking == true)
	{
		if (GUI::TreeNode("MASKING"))
		{

			GUI::InputFloat2("MaskUVCutting", (_float*)&m_EffectInfo.vUVMaskCutting);
			GUI::Checkbox("MaskUVMove", &m_EffectInfo.isMaskUVMove);

			ImGui::PushItemWidth(80);
			GUI::DragFloat2("MaskingUVGainAmount", (_float*)&m_EffectInfo.vMaskingUVGainAmount, 0.01f);
			ImGui::PopItemWidth();


			GUI::Spacing();


			_int iMaskMoveLerpOption = (_int)m_EffectInfo.iMaskMoveLerpOption;

			if (ImGui::Combo("Lerp MaskMove Option", &iMaskMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iMaskMoveLerpOption = iMaskMoveLerpOption;
			}
		
		
			GUI::Spacing();

			if (GUI::TreeNode("MASKING_TEX"))
			{
				string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "MASKING_TEXTURE", (CComponent**)&m_pMasking_TextureCom, nullptr, this);
				
				if (strName != "") {
					m_strMaskingName = strName;
				}

				GUI::TreePop();
			}
			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}

	if (m_EffectInfo.isDissolve == true)
	{
		if (GUI::TreeNode("DISSOLVE"))
		{
			if (GUI::TreeNode("DISSOLV_TEX"))
			{
				_string strName = m_strDissolveName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "DISSOLVE_TEXTURE", (CComponent**)&m_pDissolve_TextureCom, nullptr, this);
				
				if (strName != "") {
					m_strDissolveName = strName;
				}

				GUI::TreePop();
			}
			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}

	}
	if (m_EffectInfo.isNoise == true)
	{
		if (GUI::TreeNode("NOISE"))
		{
			ImGui::PushItemWidth(80);

			GUI::DragFloat("NoiseDistortionIntensity", &m_EffectInfo.fNoiseDistortionIntensity, 0.005f, 0.f, 1.f);
			
			GUI::Spacing();

			GUI::DragFloat2("DiffuseNoiseUVGainAmount", (_float*)&m_EffectInfo.vDiffuseNoiseUVGainAmount, 0.01f);

			_int iDiffuseNoiseMoveLerpOption = (_int)m_EffectInfo.iDiffuseNoiseMoveLerpOption;

			if (ImGui::Combo("Lerp DiffuseNoiseMove Option", &iDiffuseNoiseMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iDiffuseNoiseMoveLerpOption = iDiffuseNoiseMoveLerpOption;
			}

			GUI::Spacing();

			GUI::DragFloat2("MaskNoiseUVGainAmount", (_float*)&m_EffectInfo.vMaskNoiseUVGainAmount, 0.01f);

			_int iMaskNoiseMoveLerpOption = (_int)m_EffectInfo.iMaskNoiseMoveLerpOption;

			if (ImGui::Combo("Lerp MaskNoiseMove Option", &iMaskNoiseMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iMaskNoiseMoveLerpOption = iMaskNoiseMoveLerpOption;
			}
			
			GUI::Spacing();

			ImGui::PopItemWidth();

			if (GUI::TreeNode("NOISE_TEX"))
			{
				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NOISE_TEXTURE", (CComponent**)&m_pNoise_TextureCom, nullptr, this);

				if (strName != "") {
					m_strNoiseName = strName;
				}
				GUI::TreePop();
			}

			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}
	}



}
