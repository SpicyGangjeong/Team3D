#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComputeShader final : public CComponent
{
public:
	typedef struct tagComputeShaderInfo
	{
		_uint iNumElement = {};
		_uint iNumInputBuffer = {};
		_uint iNumOutputBuffer = {};
		_uint* iInputStructStride = { nullptr };
		_uint* iOutputStructStride = { nullptr };
	}CS_INFO;
private:
	CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComputeShader(const CComputeShader& rhs);
	virtual ~CComputeShader() = default;


public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const _char* pStartFunctionName);
	virtual HRESULT Initialize(void* pArg);
public:
	vector<D3D11_MAPPED_SUBRESOURCE> Dispatch(_uint iSRVIndex, _uint iUAVIndex, _float3 vGroupCount, ID3D11Buffer** ppBuffers, ID3D11Buffer* pConstantBuffer = nullptr);
	void    Bind_OutPut_SRV(_uint iIndex, _uint iBufferIndex);
	void    Bind_OutPut_SRV_VS(_uint iIndex, _uint iBufferIndex);
	ID3D11ComputeShader* Get_Compute() { return m_pComputeShader; }
	ID3D11UnorderedAccessView* GetOutputUAV(_uint iIndex) const;
private:
	void	Bind_SRV(_uint iIndex);
	void	Bind_UAV(_uint iIndex);
	void    Reset();

	HRESULT CreateBuffer(_uint iInputStructStride[], _uint iOuputStructStride[]);
	HRESULT CreateResurceViews();
	HRESULT CreateComputeShader();

private:
	vector<ID3D11ShaderResourceView*>	m_pInputSRV = {};
	vector<ID3D11UnorderedAccessView*>	m_pOutputUAV = {};
	vector<ID3D11ShaderResourceView*>	m_pOutputSRV = {};

	vector<ID3D11Buffer*>				m_pInputBuffer = {};
	vector<ID3D11Buffer*>				m_pOutputBuffer = {};


	ID3D11ComputeShader*				m_pComputeShader = nullptr;

	ID3DBlob*							m_pCSBlob = nullptr;

private:
	_uint						m_iNumElement = 0; // 인수 개수
	_uint						m_iNumInputBuffer = 0; // 인풋 버퍼의 개수
	_uint						m_iNumOutputBuffer = 0; // 인풋 버퍼의 개수

public:
	static CComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const _char* pStartFunctionName);
	virtual void Free() override;
	CComponent* Clone(void* pArg, CGameObject* pOwner) override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
