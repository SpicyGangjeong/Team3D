#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CShader final :
    public CComponent
{
private:
    CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CShader(const CShader& rhs);
    virtual ~CShader() = default;

public:
    HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength);
    HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
    HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrices, _uint iNumMatrices);
    HRESULT Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
    HRESULT Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** pSRVs, _uint iOffset, _uint iCount);
    HRESULT Begin(_uint iPassIndex);

private:
    vector<ID3D11InputLayout*>      m_pLayouts = { nullptr };
    ID3DX11Effect*                  m_pEffect = { nullptr };
    _uint							m_iNumPasses = {};

private:
    virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
    virtual HRESULT Initialize(void* pArg);

public:
    static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
    virtual CComponent* Clone(void* pArg, class CGameObject* pOwner) override;
    virtual void Free() override;
    void Describe_Entity() override;
};

NS_END