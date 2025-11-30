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

	m_bVisible = false;

	return S_OK;
}

void CEditEffect::Priority_Update(_float fTimeDelta)
{

}

void CEditEffect::Update(_float fTimeDelta)
{

	if (m_bVisible == false)
		return;

	if (m_pInstance_ModelCom == nullptr)
		return;


}

void CEditEffect::Late_Update(_float fTimeDelta)
{

	if (m_bVisible == false)
		return;

	if (m_pInstance_ModelCom == nullptr)
		return;


	m_pInstance_ModelCom->Drop(fTimeDelta);


	if (m_EffectInfo.isBillboard)
		m_pGameInstance->BillBoard(m_pTransformCom);



	if (m_isGoStraight == true)
	{
		m_fAccTime += fTimeDelta;

		if (m_fAccTime > 1.f)
		{
			m_fAccTime = 0.f;
			m_iSign *= -1;
		}

		m_pTransformCom->Go_Straight(3 * fTimeDelta * m_iSign);
	}


	if (m_EffectInfo.isBloom == true)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::BLOOM, this);
	}


	if (m_EffectInfo.isBlur == true)
	{
		m_pGameInstance->Add_RenderGroup(RENDER::BLUR, this);
	}

	if (m_EffectInfo.isOnlyBlur == true)
		return;

	m_pGameInstance->Add_RenderGroup(m_EffectInfo.eRenderOrder, this);

}

void CEditEffect::Reference_Mat_For_EditEffect()
{

	string strName = {};

	const char* pCompute[] = { "DIFFUSE" , "MASK", "NOISE", "DISSOLVE" , "DISTORTION" , "EMISSIVE"};

	if (ImGui::Combo("OPTION", &m_iSelectTextureNum, pCompute, 6))
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
	case 4:
		strName = dynamic_cast<CEffect_Editor*>(m_pOwner)->Reference_Mat_For_EditEffect((CComponent**)&m_pDistortion_TextureCom, this);

		if (strName != "")
		{
			m_strDistortionName = strName;
		}

		break;
	case 5:
		strName = dynamic_cast<CEffect_Editor*>(m_pOwner)->Reference_Mat_For_EditEffect((CComponent**)&m_pEmissive_TextureCom, this);

		if (strName != "")
		{
			m_strEmissiveName = strName;
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
		MessageBox(NULL, L"이펙트 오브젝트 저장 실패", L"System Message", MB_OK);
		return E_FAIL;
	}

	m_strPath = strPerfectFilePath;

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

	if (m_EffectInfo.isDistortion)
	{
		size_t iComponentLength = m_strDistortionName.length();
		const _char* pFileComponentPath = m_strDistortionName.c_str();

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


	MessageBox(NULL, L"이펙트 저장 성공", L"System Message", MB_OK);

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CEditEffect::Save_Path(HANDLE hFile)
{
	DWORD dwByte;

	size_t iObjectLength = m_strPath.length();
	const _char* pFilePath = m_strPath.c_str();

	if (!WriteFile(hFile, &m_EffectInfo.eEffectType, sizeof(EFFECT_TYPE), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (!WriteFile(hFile, &iObjectLength, sizeof(size_t), &dwByte, nullptr)) {
		return E_FAIL;
	}

	if (pFilePath != 0)
	{
		if (!WriteFile(hFile, pFilePath, sizeof(_char) * ((DWORD)iObjectLength + 1), &dwByte, nullptr)) {
			return E_FAIL;
		}
	}

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
	const char* pRenderNames[] = { "PRIORITY" , "SHADOW", "NONBLEND", "DECAL", "BLUR" , "NONLIGHT" ,"EFFECT", "BLEND" ,"BLOOM" , "UI" };
	const char* pEffectType[] = { "EFFECT" , "TRAIL" };
	const char* pShaderPass[] = { "DEFAULT" , "NON_NOMALMAP" , "BLUR" , "WEIGHTBLEND" , "NON_WORLD" , "NON_WORLD_BLUR",  "BLEND", "BLEND_NOWORLD", "BLOOM" ,"BLOOM_NOWORLD" };
	const char* pBloomType[] = { "NONE" , "BASIC" , "MUILTY"};

	_int iCurrentItem = static_cast<_int>(m_EffectInfo.eRenderOrder);
	_int iCurrentType = static_cast<_int>(m_EffectInfo.eEffectType);
	_int iCurrentBloomType = static_cast<_int>(m_EffectInfo.eBloomType);
	_int iCurrentPass = static_cast<_int>(m_EffectInfo.eShaderPass);

	if (ImGui::Combo("Render Order", &iCurrentItem, pRenderNames, ENUM_CLASS(RENDER::END)))
	{
		m_EffectInfo.eRenderOrder = static_cast<RENDER>(iCurrentItem);
	}

	if (ImGui::Combo("Effect Type", &iCurrentType, pEffectType, ENUM_CLASS(EFFECT_TYPE::END)))
	{
		m_EffectInfo.eEffectType = static_cast<EFFECT_TYPE>(iCurrentType);
	}

	if (ImGui::Combo("Shader Pass", &iCurrentPass, pShaderPass, ENUM_CLASS(SHADER_PASS_INSTANCE_MODEL::END)))
	{
		m_EffectInfo.eShaderPass = static_cast<SHADER_PASS_INSTANCE_MODEL>(iCurrentPass);
	}



	GUI::Checkbox("Visible", &m_bVisible);
	GUI::Checkbox("GO", &m_isGoStraight);

	m_pTransformCom->Describe_Entity();

	GUI::Checkbox("Diffuse", &m_EffectInfo.isDiffuse);
	GUI::Checkbox("Masking", &m_EffectInfo.isMasking);
	GUI::Checkbox("Dissolve", &m_EffectInfo.isDissolve);
	GUI::Checkbox("Distortion", &m_EffectInfo.isDistortion);
	GUI::Checkbox("Noise", &m_EffectInfo.isNoise);
	GUI::Checkbox("Bloom", &m_EffectInfo.isBloom);
	GUI::Checkbox("Blur", &m_EffectInfo.isBlur);

	if (GUI::Checkbox("Billboard", &m_EffectInfo.isBillboard))
	{
		m_pTransformCom->Rotation(0.f, 0.f, 0.f);
	}

	GUI::ColorEdit4("MixColor", (_float*)&m_EffectInfo.vColor);

	ImGui::PushItemWidth(80);

	m_pShaderCom->Describe_Entity();

	ImGui::PopItemWidth();

	if (m_EffectInfo.isBlur)
	{
		if (GUI::TreeNode("BLUR"))
		{

			GUI::Checkbox("OnlyBlur", &m_EffectInfo.isOnlyBlur);

			ImGui::PushItemWidth(80);
			GUI::DragFloat("BlurIntensity", &m_EffectInfo.fBlurIntensity, 0.005f, 0.f, 1.f);
			GUI::DragInt("BlurWeight", &m_EffectInfo.iBlurWeight, 2.f, 0, 128);


			ImGui::PopItemWidth();

			GUI::TreePop();
		}
	}
	

	if (GUI::TreeNode("TEX BLUR"))
	{
		GUI::Checkbox("DiffuseBlur", &m_EffectInfo.isDiffuseBlur);
		GUI::Checkbox("MaskBlur", &m_EffectInfo.isMaskBlur);
		GUI::Checkbox("BlurDissolve", &m_EffectInfo.isBlurDissolve);
		GUI::Checkbox("BlurReverseDissolve", &m_EffectInfo.isBlurReverseDissolve);

		ImGui::PushItemWidth(80);
		GUI::DragFloat("BluringStrength", &m_EffectInfo.fBluringStrength, 0.001f, 0.f, 1.f);
		ImGui::PopItemWidth();

		GUI::TreePop();
	}
	if (m_EffectInfo.isBloom)
	{
		if (GUI::TreeNode("BLOOM"))
		{
			if (ImGui::Combo("Bloom Type", &iCurrentBloomType, pBloomType, ENUM_CLASS(BLOOM_TYPE::END)))
			{
				m_EffectInfo.eBloomType = static_cast<BLOOM_TYPE>(iCurrentBloomType);
			}

			GUI::Checkbox("BloomDissolve", &m_EffectInfo.isBloomDissolve);
			GUI::Checkbox("BloomReverseDissolve", &m_EffectInfo.isBloomReverseDissolve);

			ImGui::PushItemWidth(80);
			GUI::DragFloat("fBloomStrength", &m_EffectInfo.fBloomStrength, 0.001f, 0.f);
			ImGui::PopItemWidth();

			GUI::TreePop();
		}
	}
	

	if (GUI::TreeNode("EMISSIVE"))
	{

		ImGui::PushItemWidth(80);
		GUI::DragFloat("EmissiveStrength", &m_EffectInfo.fEmissiveStrength, 0.005f, 0.f, 1.f);
		GUI::DragFloat("Radius", &m_EffectInfo.fRadius, 0.005f);
		GUI::DragFloat("CoreBoost", &m_EffectInfo.fCoreBoost, 0.005f);
		GUI::DragFloat("SoftStrength", &m_EffectInfo.fSoftStrength, 0.005f);
		GUI::DragFloat("SoftenExp", &m_EffectInfo.fSoftenExp, 0.005f);
		ImGui::PopItemWidth();

		GUI::ColorEdit4("Emissive", (_float*)&m_EffectInfo.vEmissive);



		GUI::Checkbox("EmissiveTex", &m_EffectInfo.isEmissive);
		GUI::Checkbox("EmissiveDissolve", &m_EffectInfo.isEmissiveDissolve);
		GUI::Checkbox("EmissiveDissolveReverse", &m_EffectInfo.isEmissiveDissolveReverse);

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

			GUI::DragFloat("Diffuse Alpha", &m_EffectInfo.fDiffuseAlpha , 0.01f, 0.f, 1.f);

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
			GUI::Checkbox("MaskClampSample", &m_EffectInfo.isMaskClampSample);

			ImGui::PushItemWidth(80);
			GUI::DragFloat2("MaskingUVGainAmount", (_float*)&m_EffectInfo.vMaskingUVGainAmount, 0.01f);
			GUI::Spacing();
			GUI::DragFloat("SoftMask", &m_EffectInfo.fSoftMask, 0.01f, 0.f);
			GUI::DragFloat("fSoftMaskEdge", &m_EffectInfo.fSoftMaskEdge, 0.01f, 0.f);
			
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
				GUI::Checkbox("Nomal Dissolve", &m_EffectInfo.isNomalDissolve);
				GUI::Checkbox("Reverse Dissolve", &m_EffectInfo.isReverseDissolve);

				_string strName  = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "DISSOLVE_TEXTURE", (CComponent**)&m_pDissolve_TextureCom, nullptr, this);
				
				if (strName != "") {
					m_strDissolveName = strName;
				}

				GUI::TreePop();
			}
			GUI::Separator(); GUI::Spacing();

			GUI::TreePop();
		}

	}
	if (m_EffectInfo.isDistortion == true)
	{
		if (GUI::TreeNode("DISTORTION"))
		{
			ImGui::PushItemWidth(80);

			GUI::DragFloat("DistortionIntensity", &m_EffectInfo.fNoiseDistortionIntensity, 0.005f, 0.f, 1.f);

			GUI::Spacing();

			GUI::DragFloat2("DiffuseDistortionUVGainAmount", (_float*)&m_EffectInfo.vDiffuseDistortionUVGainAmount, 0.01f);

			_int iDiffuseNoiseMoveLerpOption = (_int)m_EffectInfo.iDiffuseDistortionMoveLerpOption;

			if (ImGui::Combo("LerpDiffuseDistortionMove Option", &iDiffuseNoiseMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iDiffuseDistortionMoveLerpOption = iDiffuseNoiseMoveLerpOption;
			}

			GUI::Spacing();

			GUI::DragFloat2("MaskDistortionUVGainAmount", (_float*)&m_EffectInfo.vMaskDistortionUVGainAmount, 0.01f);

			_int iMaskNoiseMoveLerpOption = (_int)m_EffectInfo.iMaskDistortionMoveLerpOption;

			if (ImGui::Combo("Lerp MaskDistortionMove Option", &iMaskNoiseMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iMaskDistortionMoveLerpOption = iMaskNoiseMoveLerpOption;
			}

			GUI::Spacing();

			ImGui::PopItemWidth();

			if (GUI::TreeNode("DISTORTION_TEX"))
			{
				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "DISTORTION_TEXTURE", (CComponent**)&m_pDistortion_TextureCom, nullptr, this);

				if (strName != "") {
					m_strDistortionName = strName;
				}

				GUI::TreePop();
			}

			GUI::TreePop();
		}

	}



		

	if (m_EffectInfo.isNoise == true)
	{
		if (GUI::TreeNode("NOISE"))
		{
			GUI::Checkbox("NoiseColor", &m_EffectInfo.isNoiseColor);
			GUI::Checkbox("NoiseAlpha", &m_EffectInfo.isNoiseAlpha);
			GUI::Checkbox("NoiseMove", &m_EffectInfo.isNoiseUVMove);

			ImGui::PushItemWidth(80);
			GUI::DragFloat2("NoiseUVGainAmount", (_float*)&m_EffectInfo.vNoiseUVGainAmount, 0.01f);
			ImGui::PopItemWidth();


			GUI::Spacing();


			_int iNoiseMoveLerpOption = (_int)m_EffectInfo.iNoiseMoveLerpOption;

			if (ImGui::Combo("Lerp NoiseMove Option", &iNoiseMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iNoiseMoveLerpOption = iNoiseMoveLerpOption;
			}

			if (GUI::TreeNode("NOISE_TEX"))
			{
				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NOISE_TEXTURE", (CComponent**)&m_pNoise_TextureCom, nullptr, this);

				if (strName != "") {
					m_strNoiseName = strName;
				}

				GUI::TreePop();
			}

			GUI::TreePop();
		}

		//ImGui::Begin("Simple Plot");


		//ImGui::PlotLines("Value", m_ValueVector.data(), (int)m_ValueVector.size(), 0, nullptr, FLT_MAX, FLT_MAX, ImVec2(0, 100));

		//GUI::InputFloat("InputValue", &m_fInputValue);

		//if (GUI::Button("AddValue"))
		//{
		//	m_ValueVector.push_back(m_fInputValue);
		//}

		//ImGui::End();

	}



	GUI::Separator(); GUI::Spacing();



}
