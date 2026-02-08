#include "pch.h"
#include "EditEffect.h"

#include "GameInstance.h"
#include "Effect_Editor.h"
#include "Layer.h"
#include "Player.h"
#include "Wand.h"

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


	XMStoreFloat4x4(&m_ScreenViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OrthographicMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));


	return S_OK;
}

void CEditEffect::Priority_Update(_float fTimeDelta)
{
	if(m_EffectInfo.isMotionBlur == true)
		m_pTransformCom->RewindMomentum();
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


	m_pInstance_ModelCom->Compute_CS(fTimeDelta);


	if (m_EffectInfo.isBillboard)
		m_pGameInstance->BillBoard(m_pTransformCom);


	if (m_EffectInfo.isLightDissolve == true)
	{
		if (m_pLightCom != nullptr)
		{
			m_pLightCom->Update_IntensityRatio(fTimeDelta , m_EffectInfo.isLightTime , m_EffectInfo.fLightDeley);
		}
	}



	if (m_isWandPos == true)
	{
		_vector vWandPos = m_pGameInstance->Get_Layer(CURRENT_LEVEL, LAYER_PLAYER)->Get_Object<CPlayer>()->Get_PartObject<CWand>()->Get_WorldPostion();

		m_pTransformCom->Set_State(STATE::POSITION, vWandPos);
	}

	if (m_isCamPos == true)
	{
		_vector vCampos = XMLoadFloat4(m_pGameInstance->Get_CamPosition());
		
		vCampos += XMLoadFloat4(&m_vCamOffset);
	
		m_pTransformCom->Set_State(STATE::POSITION, vCampos);
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


	if (m_pLightCom != nullptr)
		m_pGameInstance->Add_Light_Group(CURRENT_LEVEL , m_pLightCom);

	m_pGameInstance->Add_RenderGroup(m_EffectInfo.eRenderOrder, this);

}

_vector CEditEffect::Get_WorldPostion()
{
	return m_pTransformCom->Get_State(STATE::POSITION);
}

void CEditEffect::Reference_Mat_For_EditEffect()
{

	string strName = {};

	const char* pCompute[] = { "DIFFUSE" , "MASK", "NOISE", "DISSOLVE" , "DISTORTION" , "EMISSIVE"};

	if (GUI::Combo("OPTION", &m_iSelectTextureNum, pCompute, 6))
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

	if (m_EffectInfo.isNomalMap)
	{
		size_t iComponentLength = m_strNormalMapName.length();
		const _char* pFileComponentPath = m_strNormalMapName.c_str();

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


	if (FAILED(__super::Add_Asset_Component(g_iStaticLevel, TEXT("Base_MRO"),
		reinterpret_cast<CComponent**>(&m_pSurface_TextureCom))))
		return E_FAIL;

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
	const char* pRenderNames[] = { "PRIORITY" , "SHADOW_NEAR", "NONBLEND", "DECAL", "BLUR" , "NONLIGHT" ,"EFFECT", "BLEND" ,"BLOOM", "DISTORTION", "BULR_MESH", "UI", "OCCLUSION" , "SHADOW_MIDDLE" , "PRESHADOW" , "UI_OVERLAY", "EFFECT_NONBLEND" };

	const char* pEffectType[] = { "EFFECT" , "TRAIL" };
	const char* pShaderPass[] = { "DEFAULT" , "NON_NOMALMAP" , "BLUR" , "WEIGHTBLEND" , "NON_WORLD" , "NON_WORLD_BLUR",  "BLEND", "BLEND_NOWORLD", "BLOOM" ,"BLOOM_NOWORLD" ,"BLUR_NO_EMMISVE", 
		"BLUR_NO_WORLD_NO_EMISSIVE","WEIGHTBLEND_FOR_BLEND" , "DEPTH_STOP" , "WB_CULLING", "SCREEN_FX" , "DISTORTION" ,"NONPOS" , "NONPOS_BLUR" , "BULR_MESH" , "BLUR_CULLING" ,"BLUR_CULLING_NO_EMISSIVE" ,
		"BLOOM_CULLING" , "DEFAULT_NONPOS" , "NONWB_NONPOS",  "DECAL", "DECAL_WB" , "DECAL_BLUR", "BLEND_CULLING" , "DECAL_BLEND"};

	const char* pBloomType[] = { "NONE" , "BASIC" , "MUILTY"};
	_int iCurrentItem = static_cast<_int>(m_EffectInfo.eRenderOrder);
	_int iCurrentType = static_cast<_int>(m_EffectInfo.eEffectType);
	_int iCurrentBloomType = static_cast<_int>(m_EffectInfo.eBloomType);
	_int iCurrentPass = static_cast<_int>(m_EffectInfo.eShaderPass);


	if (GUI::Combo("Render Order", &iCurrentItem, pRenderNames, ENUM_CLASS(RENDER::END)))
	{
		m_EffectInfo.eRenderOrder = static_cast<RENDER>(iCurrentItem);
	}


	if (GUI::Combo("Effect Type", &iCurrentType, pEffectType, ENUM_CLASS(EFFECT_TYPE::END)))
	{
		m_EffectInfo.eEffectType = static_cast<EFFECT_TYPE>(iCurrentType);
	}

	if (GUI::Combo("Shader Pass", &iCurrentPass, pShaderPass, ENUM_CLASS(SHADER_PASS_INSTANCE_MODEL::END)))
	{
		m_EffectInfo.eShaderPass = static_cast<SHADER_PASS_INSTANCE_MODEL>(iCurrentPass);
	}


	GUI::Checkbox("Visible", &m_bVisible);
	//GUI::Checkbox("WAND POS", &m_isWandPos);
	//GUI::Checkbox("CAM POS", &m_isCamPos);

	GUI::Separator();

	m_pTransformCom->Describe_Entity();

	GUI::Checkbox("Diffuse", &m_EffectInfo.isDiffuse);
	GUI::Checkbox("Masking", &m_EffectInfo.isMasking);
	GUI::Checkbox("Dissolve", &m_EffectInfo.isDissolve);
	GUI::Checkbox("Distortion", &m_EffectInfo.isDistortion);
	GUI::Checkbox("Noise", &m_EffectInfo.isNoise);
	GUI::Checkbox("Bloom", &m_EffectInfo.isBloom);
	GUI::Checkbox("Blur", &m_EffectInfo.isBlur);
	GUI::Checkbox("RimLight", &m_EffectInfo.isRimLight);
	GUI::Checkbox("Nomal", &m_EffectInfo.isNomalMap);

	if (GUI::Checkbox("Billboard", &m_EffectInfo.isBillboard))
	{
		m_pTransformCom->Rotation(0.f, 0.f, 0.f);
	}

	GUI::ColorEdit4("MixColor", (_float*)&m_EffectInfo.vColor);


	GUI::Checkbox("NonSoftEffect", &m_EffectInfo.isNonSoftEffect);
	GUI::Checkbox("ScreenFX", &m_EffectInfo.isScreenFX);

	GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);

	//m_pShaderCom->Describe_Entity();

	GUI::PopItemWidth();

	if (m_EffectInfo.isBlur)
	{
		if (GUI::TreeNode("BLUR"))
		{

			GUI::Checkbox("OnlyBlur", &m_EffectInfo.isOnlyBlur);
			GUI::Checkbox("BlurNoEmissive", &m_EffectInfo.isBlurNoEmissive);
			GUI::Checkbox("BlurDissolve", &m_EffectInfo.isBlurDissolve);
			
			GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
			GUI::DragFloat("BlurIntensity", &m_EffectInfo.fBlurIntensity, 0.005f, 0.f, 1.f);
			GUI::DragInt("BlurWeight", &m_EffectInfo.iBlurWeight, 2.f, 0, 128);


			GUI::Checkbox("BlurColor", &m_EffectInfo.isBlurColor);
			GUI::PopItemWidth();
			GUI::ColorEdit4("BlurColor", (_float*)&m_EffectInfo.vBlurColor);

			GUI::TreePop();
		}
	}
	

	if (GUI::TreeNode("MOTION BLUR"))
	{
		GUI::Checkbox("MotionBlur", &m_EffectInfo.isMotionBlur);
		GUI::DragFloat("MotionBlurIntensity", &m_EffectInfo.fModelBlurIntensity , 0.005f , 0.f);
		GUI::TreePop();
	}

	if (m_EffectInfo.isBloom)
	{
		if (GUI::TreeNode("BLOOM"))
		{
			if (GUI::Combo("Bloom Type", &iCurrentBloomType, pBloomType, ENUM_CLASS(BLOOM_TYPE::END)))
			{
				m_EffectInfo.eBloomType = static_cast<BLOOM_TYPE>(iCurrentBloomType);
			}

			GUI::Checkbox("BloomDissolve", &m_EffectInfo.isBloomDissolve);
			GUI::Checkbox("BloomReverseDissolve", &m_EffectInfo.isBloomReverseDissolve);

			GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
			GUI::DragFloat("fBloomStrength", &m_EffectInfo.fBloomStrength, 0.001f, 0.f);
			GUI::PopItemWidth();

			GUI::TreePop();
		}
	}

	if (GUI::TreeNode("MODEL DISTORTION"))
	{
		GUI::DragFloat("Distortion Intensity", &m_EffectInfo.fModelDistortIntensity, 0.005f, 0.f, 1.f);
		GUI::TreePop();
	}

	if (GUI::TreeNode("EMISSIVE"))
	{

		GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
		GUI::DragFloat("EmissiveStrength", &m_EffectInfo.fEmissiveStrength, 0.005f, 0.f, 1.f);
		GUI::DragFloat("Radius", &m_EffectInfo.fRadius, 0.005f , 0.f);
		GUI::DragFloat("CoreBoost", &m_EffectInfo.fCoreBoost, 0.005f , 0.f);
		GUI::DragFloat("SoftStrength", &m_EffectInfo.fSoftStrength, 0.005f , 0.f);
		GUI::DragFloat("SoftenExp", &m_EffectInfo.fSoftenExp, 0.005f , 0.f);
		GUI::DragFloat("EmissiveColorCut", &m_EffectInfo.fEmissiveColorCut, 0.005f , 0.f);

		GUI::PopItemWidth();

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
	
	if (m_EffectInfo.isRimLight)
	{
		if (GUI::TreeNode("RIM LIGHT"))
		{
			GUI::ColorEdit4("Emissive", (_float*)&m_EffectInfo.vRimLightColor);
			GUI::DragFloat("RimLightPower", &m_EffectInfo.fRimLightPower, 0.005f , 0.f);
			GUI::DragFloat("RimLightStrength", &m_EffectInfo.fRimLightStrength, 0.005f, 0.f);

			GUI::TreePop();
		}
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

			GUI::Checkbox("Light Dissolve", &m_EffectInfo.isLightDissolve);
			GUI::InputFloat("Light Time", &m_EffectInfo.isLightTime);
			GUI::InputFloat("Light Delay", &m_EffectInfo.fLightDeley);
			
			if (GUI::InputFloat("Light Intensity", &m_EffectInfo.fLightIntensity))
			{

				m_pLightCom->Set_LightIntensity(m_EffectInfo.fLightIntensity);
			}

			if (GUI::Button("ADD_LIGHT_MANAGER"))
			{
				m_pGameInstance->Add_Light(CURRENT_LEVEL, m_pLightCom);
			}

			if (GUI::Button("Reset_Dissolve"))
			{
				m_pLightCom->Reset_IntensityRatio();
			}
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

			GUI::Checkbox("Diffuse_R", &m_EffectInfo.isDiffuse_R);
			GUI::Checkbox("Diffuse_G", &m_EffectInfo.isDiffuse_G);
			GUI::Checkbox("Diffuse_B", &m_EffectInfo.isDiffuse_B);

			GUI::DragFloat("Diffuse Alpha", &m_EffectInfo.fDiffuseAlpha , 0.01f, 0.f, 1.f);

			GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
			GUI::Checkbox("DiffuseUVMove", &m_EffectInfo.isDiffuseUVMove);
			GUI::InputFloat2("DiffuseUVCutting", (_float*)&m_EffectInfo.vUVCutting);

			GUI::Spacing();

			GUI::DragFloat2("DiffuseUVGainAmount", (_float*)&m_EffectInfo.vDiffuseUVGainAmount, 0.01f);
	


			_int iDiffuseMoveLerpOption = (_int)m_EffectInfo.iDiffuseMoveLerpOption;

			if (GUI::Combo("Lerp DiffuseMove Option", &iDiffuseMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iDiffuseMoveLerpOption = iDiffuseMoveLerpOption;
			}

			GUI::Spacing();

			GUI::PopItemWidth();

			if (GUI::TreeNode("DIFFUSE_TEX"))
			{
				if (m_pDiffuse_TextureCom != nullptr)
				{
					if (GUI::ImageButton("Current Texture", m_pDiffuse_TextureCom->Get_SRV(0), ImVec2(48, 48)))
					{
					}

					GUI::Spacing(); GUI::Spacing();
				}

				if (GUI::TreeNode("DECAL TEX"))
				{
					_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NORMAL_TEXTURE", (CComponent**)&m_pDiffuse_TextureCom, nullptr, this, L"DECAL");

					if (strName != "") {
						m_strDiffuseName = strName;
					}

					GUI::TreePop();
				}


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
			GUI::Checkbox("Mask G", &m_EffectInfo.isMask_G);
			GUI::SameLine();
			GUI::Checkbox("Mask B", &m_EffectInfo.isMask_B);


			GUI::InputFloat2("MaskUVCutting", (_float*)&m_EffectInfo.vUVMaskCutting);
			GUI::Checkbox("MaskUVMove", &m_EffectInfo.isMaskUVMove);
			GUI::Checkbox("MaskClampSample", &m_EffectInfo.isMaskClampSample);

			GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
			GUI::DragFloat2("MaskOffset", (_float*)&m_EffectInfo.vMaskOffset, 0.01f);
			GUI::DragFloat2("MaskingUVGainAmount", (_float*)&m_EffectInfo.vMaskingUVGainAmount, 0.01f);
			GUI::Spacing();
			GUI::DragFloat("SoftMask", &m_EffectInfo.fSoftMask, 0.01f, 0.f);
			GUI::DragFloat("fSoftMaskEdge", &m_EffectInfo.fSoftMaskEdge, 0.01f, 0.f);
			
			GUI::PopItemWidth();


			GUI::Spacing();


			_int iMaskMoveLerpOption = (_int)m_EffectInfo.iMaskMoveLerpOption;

			if (GUI::Combo("Lerp MaskMove Option", &iMaskMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iMaskMoveLerpOption = iMaskMoveLerpOption;
			}
		
		
			GUI::Spacing();

			if (GUI::TreeNode("MASKING_TEX"))
			{
				if (m_pMasking_TextureCom != nullptr)
				{
					if (GUI::ImageButton("Current Texture", m_pMasking_TextureCom->Get_SRV(0), ImVec2(48, 48)))
					{
					}

					GUI::Spacing(); GUI::Spacing();
				}

				if (GUI::TreeNode("DECAL TEX"))
				{
					_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NORMAL_TEXTURE", (CComponent**)&m_pMasking_TextureCom, nullptr, this, L"DECAL");

					if (strName != "") {
						m_strMaskingName = strName;
					}

					GUI::TreePop();
				}


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
			GUI::Checkbox("Dissolve G", &m_EffectInfo.isDissolve_G);
			GUI::SameLine();
			GUI::Checkbox("Dissolve B", &m_EffectInfo.isDissolve_B);

			GUI::Checkbox("Nomal Dissolve", &m_EffectInfo.isNomalDissolve);
			GUI::Checkbox("Reverse Dissolve", &m_EffectInfo.isReverseDissolve);
			GUI::Checkbox("StopDissolveSmoothStep", &m_EffectInfo.isNoDissolveSmoothStep);
			
			GUI::DragFloat2("DissolveSmoothRange", (_float*)&m_EffectInfo.vDissolveSmoothRange, 0.01f);

			GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
			GUI::Spacing();
			GUI::Checkbox("DissolveMove", &m_EffectInfo.isDissolveMove);
			GUI::DragFloat2("DissolveUVGainAmount", (_float*)&m_EffectInfo.vDissolveUVGainAmount, 0.01f);

			GUI::Spacing();
			GUI::DragFloat("DissolveDelay", &m_EffectInfo.fDissolveDelay, 0.005f);
			GUI::DragFloat("ReverseDissolveDelay", &m_EffectInfo.fReverseDissolveDelay, 0.005f);

			GUI::Spacing();

			GUI::DragFloat("DissolveMaskEdge", &m_EffectInfo.vDissolveValue.x, 0.005f);
			GUI::DragFloat("DissolveSoftMask", &m_EffectInfo.vDissolveValue.y, 0.005f);
			GUI::DragFloat("DissolveCutRatio", &m_EffectInfo.vDissolveValue.z, 0.005f);

			GUI::Spacing();

			GUI::PopItemWidth();

			if (GUI::TreeNode("DISSOLV_TEX"))
			{
				if (m_pDissolve_TextureCom != nullptr)
				{
					if (GUI::ImageButton("Current Texture", m_pDissolve_TextureCom->Get_SRV(0), ImVec2(48, 48)))
					{
					}

					GUI::Spacing(); GUI::Spacing();
				}

				if (GUI::TreeNode("DECAL TEX"))
				{
					_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "DECAL_TEXTURE", (CComponent**)&m_pDissolve_TextureCom, nullptr, this, L"DECAL");

					if (strName != "") {
						m_strDissolveName = strName;
					}

					GUI::TreePop();
				}

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
			GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);

			GUI::DragFloat("DistortionIntensity", &m_EffectInfo.fNoiseDistortionIntensity, 0.005f, 0.f, 1.f);

			GUI::Spacing();

			GUI::DragFloat2("vDistortionTime", (_float*)&m_EffectInfo.vDistortionTime, 0.01f);

			_int iDiffuseNoiseMoveLerpOption = (_int)m_EffectInfo.iDiffuseDistortionMoveLerpOption;

			if (GUI::Combo("LerpDiffuseDistortionMove Option", &iDiffuseNoiseMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iDiffuseDistortionMoveLerpOption = iDiffuseNoiseMoveLerpOption;
			}

			GUI::Spacing();

			GUI::DragFloat2("MaskDistortionUVGainAmount", (_float*)&m_EffectInfo.vMaskDistortionUVGainAmount, 0.01f);

			_int iMaskNoiseMoveLerpOption = (_int)m_EffectInfo.iMaskDistortionMoveLerpOption;

			if (GUI::Combo("Lerp MaskDistortionMove Option", &iMaskNoiseMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iMaskDistortionMoveLerpOption = iMaskNoiseMoveLerpOption;
			}

			GUI::Spacing();

			GUI::PopItemWidth();

			if (GUI::TreeNode("DISTORTION_TEX"))
			{
				if (m_pDistortion_TextureCom != nullptr)
				{
					if (GUI::ImageButton("Current Texture", m_pDistortion_TextureCom->Get_SRV(0), ImVec2(48, 48)))
					{
					}

					GUI::Spacing(); GUI::Spacing();
				}

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
			GUI::Checkbox("Noise G", &m_EffectInfo.isNoise_G);
			GUI::SameLine();
			GUI::Checkbox("Noise B", &m_EffectInfo.isNoise_B);

			GUI::Checkbox("NoiseColor", &m_EffectInfo.isNoiseColor);
			GUI::Checkbox("NoiseAlpha", &m_EffectInfo.isNoiseAlpha);
			GUI::Checkbox("NoiseMove", &m_EffectInfo.isNoiseUVMove);


			GUI::ColorEdit4("NoiseColor", (_float*)&m_EffectInfo.vNoiseColor);
			GUI::PushItemWidth(IMGUI_GLOBAL_ITEM_WIDTH);
			GUI::DragFloat2("NoiseUVGainAmount", (_float*)&m_EffectInfo.vNoiseUVGainAmount, 0.01f);
			GUI::DragFloat2("UVNoiseCutting", (_float*)&m_EffectInfo.vUVNoiseCutting, 0.01f);
			
			GUI::PopItemWidth();


			GUI::Spacing();


			_int iNoiseMoveLerpOption = (_int)m_EffectInfo.iNoiseMoveLerpOption;

			if (GUI::Combo("Lerp NoiseMove Option", &iNoiseMoveLerpOption, pLerp, 9))
			{
				m_EffectInfo.iNoiseMoveLerpOption = iNoiseMoveLerpOption;
			}

			if (GUI::TreeNode("NOISE_TEX"))
			{
				if (m_pNoise_TextureCom != nullptr)
				{
					if (GUI::ImageButton("Current Texture", m_pNoise_TextureCom->Get_SRV(0), ImVec2(48, 48)))
					{
					}

					GUI::Spacing(); GUI::Spacing();
				}

				if (GUI::TreeNode("DECAL TEX"))
				{
					_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NORMAL_TEXTURE", (CComponent**)&m_pNoise_TextureCom, nullptr, this, L"DECAL");

					if (strName != "") {
						m_strNoiseName = strName;
					}

					GUI::TreePop();
				}

				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NOISE_TEXTURE", (CComponent**)&m_pNoise_TextureCom, nullptr, this);

				if (strName != "") {
					m_strNoiseName = strName;
				}

		

				GUI::TreePop();
			}

			GUI::TreePop();
		}
	}

	if (m_EffectInfo.isNomalMap == true)
	{
		if (GUI::TreeNode("NORMAL_TEX"))
		{
			if (m_pNormal_TextureCom != nullptr)
			{
				if (GUI::ImageButton("Current Texture", m_pNormal_TextureCom->Get_SRV(0), ImVec2(48, 48)))
				{
				}

				GUI::Spacing(); GUI::Spacing();
			}

			if (GUI::TreeNode("NORMAL"))
			{
				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NORMAL_TEXTURE", (CComponent**)&m_pNormal_TextureCom, nullptr, this, L"NORMAL");

				if (strName != "") {
					m_strNormalMapName = strName;
				}

				GUI::TreePop();
			}

			if (GUI::TreeNode("DECAL NORMAL"))
			{
				_string strName = m_pGameInstance->Asset_Description<CTexture>(ENUM_CLASS(LEVEL::EFFECT), "NORMAL_TEXTURE", (CComponent**)&m_pNormal_TextureCom, nullptr, this, L"DECALNORMAL");

				if (strName != "") {
					m_strNormalMapName = strName;
				}

				GUI::TreePop();
			}
		
	

		

			GUI::TreePop();
		}
	}



	GUI::Separator(); GUI::Spacing();



}

HRESULT CEditEffect::Save_XML(tinyxml2::XMLDocument& doc, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement* object = doc.NewElement("Object");
	root->InsertEndChild(object);

	_float3 vPosition = {};
	XMStoreFloat3(&vPosition, m_pTransformCom->Get_State(STATE::POSITION));
	_float3 vScale = m_pTransformCom->Get_Scale();
	_float3 vRotation = m_pTransformCom->Get_Rotation();

	tinyxml2::XMLElement* Position = doc.NewElement("Position");
	Position->SetAttribute("x", vPosition.x);
	Position->SetAttribute("y", vPosition.y);
	Position->SetAttribute("z", vPosition.z);
	object->InsertEndChild(Position);

	tinyxml2::XMLElement* Scale = doc.NewElement("Scale");
	Scale->SetAttribute("x", vScale.x);
	Scale->SetAttribute("y", vScale.y);
	Scale->SetAttribute("z", vScale.z);
	object->InsertEndChild(Scale);

	tinyxml2::XMLElement* Rotation = doc.NewElement("Rotation");
	Rotation->SetAttribute("x", vRotation.x);
	Rotation->SetAttribute("y", vRotation.y);
	Rotation->SetAttribute("z", vRotation.z);
	object->InsertEndChild(Rotation);

	return S_OK;
}

void CEditEffect::Control_Transform()
{
	m_pTransformCom->Describe_Entity();

	_float3 vPosition = {};
	if (m_pGameInstance->Mouse_Down(DIM_LBUTTON) && m_pGameInstance->Key_Pressing(DIK_LSHIFT))
	{
		_float3 vPosition = {};
		if (m_pGameInstance->isPicking(&vPosition))
		{
			m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));
		}
	}
}
