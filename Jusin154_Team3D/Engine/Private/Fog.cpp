#include "pch.h"
#include "Fog.h"
#include "Shader.h"
#include "GameInstance.h"

CFog::CFog()
    : m_pGameInstance{CGameInstance::GetInstance()},
    m_bVisible {true},
    m_fFogPow{ 30.f }
{
	SAFE_ADDREF(m_pGameInstance);
}

void CFog::Set_Fog(_float fFogDensity, _float fPow)
{
    m_fFogDensity = fFogDensity;
    m_fFogPow = fPow;
}

void CFog::Set_FogColor(_float4& vFogColor)
{
    memcpy(&m_vFogColor, &vFogColor, sizeof(_float4));
}

HRESULT CFog::Bind_FogValue(CShader* pShader)
{
#ifdef _DEBUG
    if (m_bVisible) {
        pShader->Bind_RawValue("g_fFogDensity", &m_fFogDensity, sizeof(_float));
        pShader->Bind_RawValue("g_fFogPow", &m_fFogPow, sizeof(_float));
        pShader->Bind_RawValue("g_vFogColor", &m_vFogColor, sizeof(_float4));
        pShader->Bind_RawValue("g_bFogVisible", &m_bVisible, sizeof(_bool));
        pShader->Bind_RawValue("g_fFar", m_pGameInstance->Get_CurrentCameraFar(), sizeof(_bool));
        pShader->Bind_Matrix("g_invMatView", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW_INV));
        pShader->Bind_Matrix("g_invmatProj", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ_INV));
       
    }
    else
    {
        _float fValue = 0.f;
        _float4 vColor = { 0.f, 0.f,0.f,0.f };
        pShader->Bind_RawValue("g_fFogDensity", &fValue, sizeof(_float));
        pShader->Bind_RawValue("g_fFogPow", &fValue, sizeof(_float));
        pShader->Bind_RawValue("g_vFogColor", &vColor, sizeof(_float4));
        pShader->Bind_RawValue("g_bFogVisible", &m_bVisible, sizeof(_bool));
    }
#else
    pShader->Bind_RawValue("g_fFogDensity", &m_fFogDensity, sizeof(_float));
    pShader->Bind_RawValue("g_fFogPow", &m_fFogPow, sizeof(_float));
    pShader->Bind_RawValue("g_vFogColor", &m_vFogColor, sizeof(_float4));
    pShader->Bind_RawValue("g_bFogVisible", &m_bVisible, sizeof(_bool));
#endif // _DEBUG

    return S_OK;
}

#ifdef _DEBUG
void CFog::Update_Fog()
{
    GUI::Begin("SYSTEM", 0, IMGUI_GLOBAL_BEGIN_FLAG);
    if (GUI::CollapsingHeader("FOG")) {
        GUI::Checkbox("Fog ON / OFF", &m_bVisible);
        GUI::InputFloat("g_fFogDensity", &m_fFogDensity, 0.001f);
        GUI::InputFloat("g_fFogPow", &m_fFogPow, 0.1f);
        GUI::ColorEdit4("Fog Color", (_float*)(&m_vFogColor), ImGuiColorEditFlags_NoInputs);
    }
    GUI::End();
}
#endif // _DEBUG

HRESULT CFog::Initialize()
{
   m_pNoiseTextureSRV = m_pGameInstance->Add_Resource("C:/MeshTable/Game/VFX/Textures/Noises/VFX_T_Noise01_D.png");

    if (nullptr == m_pNoiseTextureSRV)
        return E_FAIL;

    return S_OK;
}

CFog* CFog::Create()
{
	CFog* pInstance = new CFog();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created CFog");
		SAFE_RELEASE(pInstance);
    }
    return pInstance;
}

void CFog::Free()
{
    __super::Free();

	SAFE_RELEASE(m_pNoiseTextureSRV);
	SAFE_RELEASE(m_pGameInstance);
}
