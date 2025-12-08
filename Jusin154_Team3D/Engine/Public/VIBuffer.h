#pragma once
#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract :
public CComponent
{
protected:
	CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;

public:
	virtual HRESULT Bind_Resources();
	virtual HRESULT Render();

protected:
	ID3D11Buffer*				m_pVB = { nullptr };
	ID3D11Buffer*				m_pIB = { nullptr };
	_float3*					m_pVertexPositions = { nullptr };
	_uint*						m_pIndices = { nullptr };
	_uint						m_iNumVertexBuffers = {};
	_uint						m_iNumVertices = {};
	_uint						m_iVertexStride = {};
	_uint						m_iNumIndices = {};
	_uint						m_iIndexStride = {};
	DXGI_FORMAT					m_eIndexFormat = {};
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitive = {};


public:
	virtual void Free();
};

NS_END
