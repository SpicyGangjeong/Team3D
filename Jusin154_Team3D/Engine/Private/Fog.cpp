#include "pch.h"
#include "Fog.h"
#include "Shader.h"

CFog::CFog()
    : m_bVisible{ true }
{
}

void CFog::Set_FogDensity(_float fFogDensity)
{
    m_fFogDensity = fFogDensity;
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
    }
    else
    {
        _float fValue = 0.f;
        _float4 vColor = { 0.f, 0.f,0.f,0.f };
        pShader->Bind_RawValue("g_fFogDensity", &fValue, sizeof(_float));
        pShader->Bind_RawValue("g_fFogDensity", &fValue, sizeof(_float));
        pShader->Bind_RawValue("g_vFogColor", &vColor, sizeof(_float4));
    }
#else
    pShader->Bind_RawValue("g_fFogDensity", &m_fFogDensity, sizeof(_float));
    pShader->Bind_RawValue("g_fFogPow", &m_fFogPow, sizeof(_float));
    pShader->Bind_RawValue("g_vFogColor", &m_vFogColor, sizeof(_float4));
#endif // _DEBUG

    return S_OK;
}

#ifdef _DEBUG
void CFog::Update_Fog()
{
    GUI::Begin("Renderer");
    if (GUI::CollapsingHeader("FOG")) {
        GUI::Checkbox("Fog ON / OFF", &m_bVisible);
        GUI::InputFloat("g_fFogDensity", &m_fFogDensity, 0.001f);
        GUI::InputFloat("g_fFogPow", &m_fFogPow, 0.1f);
        GUI::ColorEdit4("Fog Color", (_float*)(&m_vFogColor), ImGuiColorEditFlags_NoInputs);
    }
    GUI::End();
}
#endif // _DEBUG

CFog* CFog::Create()
{
    return new CFog();
}

void CFog::Free()
{
    __super::Free();
}
