#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
public:
	typedef struct tagInstanceDesc
	{
		_uint	iNum = {};
		_float2 vSize = {};
		_float	fPositionOffSetX{};
		_float	fPositionOffSetY{};
		_float2 vPosition = {};
	}INSTANCE_DESC;
protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& Prototype);
	virtual ~CVIBuffer_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const INSTANCE_DESC* pDesc);
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Bind_Resources() override;
	virtual HRESULT Render() override;

protected:
	ID3D11Buffer*			m_pVBInstance = { nullptr };
	D3D11_BUFFER_DESC		m_InstanceBufferDesc = {};
	D3D11_SUBRESOURCE_DATA	m_InstanceInitialDesc = {};
	_uint					m_iInstanceStride = {};
	_uint					m_iNumInstance = {};
	_uint					m_iCopyCount{};
	_uint					m_iNumIndexPerInstance = {};
public:
	virtual void Free() override;
};

NS_END
