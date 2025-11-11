#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComputeShader final : public CBase
{

private:
	CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CComputeShader() = default;


public:
	 HRESULT Initialize(const _tchar* pShaderFilePath, const _char* pStartFunctionName, _uint iNumElement, _uint iNumInputBuffer, _uint iOutputStructStride, _uint iInputStructStride[]);
public:
	D3D11_MAPPED_SUBRESOURCE Dispatch(_uint iSRVIndex, _uint iUAVIndex, _float3 vGroupCount, ID3D11Buffer** ppBuffers, ID3D11Buffer* pConstantBuffer = nullptr);
	void    Bind_OutPut_SRV(_uint iIndex);
private:
	void	Bind_SRV(_uint iIndex);
	void	Bind_UAV(_uint iIndex);
	void    Reset();

	HRESULT CreateBuffer(_uint iNumElement, _uint iNumInputBuffer, _uint iStructStride[]);
	HRESULT CreateResurceViews(_uint iNumElement, _uint iNumInputBuffer);
	HRESULT CreateComputeShader(const _tchar* pShaderFilePath, const _char* pStartFunctionName);

private:
	ID3D11Device*		 m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	class CGameInstance* m_pGameInstance = { nullptr };

private:
	vector<ID3D11ShaderResourceView*>	m_pInputSRV = {};
	ID3D11UnorderedAccessView*			m_pOutputUAV = nullptr;

	vector<ID3D11Buffer*>				m_pInputBuffer = {};
	ID3D11Buffer*						m_pOutputBuffer = nullptr;

	vector<ID3D11Buffer*>				m_pStagingBuffer = {};
	ID3D11Buffer*						m_pOutputStagingBuffer = {};
	ID3D11ComputeShader*				m_pComputeShader = nullptr;

	ID3D11ShaderResourceView*			m_pOutputSRV = { nullptr };

private:
	_uint						m_iNumElement = 0; // 인수 개수
	_uint						m_iNumInputBuffer = 0; // 인풋 버퍼의 개수
	_uint						m_iOutputStructStride = 0; // 데이터 크기

public:
	static CComputeShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const _char* pStartFunctionName, _uint iNumElement, _uint iNumInputBuffer,  _uint iOutputStructStride, _uint iInputStructStride[]);
	virtual void Free() override;

};

NS_END