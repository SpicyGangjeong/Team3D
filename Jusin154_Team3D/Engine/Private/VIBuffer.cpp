#include "pch.h"
#include "VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_pVertexPositions(rhs.m_pVertexPositions)
	, m_pIndices(rhs.m_pIndices)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iVertexStride(rhs.m_iVertexStride)
	, m_iNumIndices(rhs.m_iNumIndices)
	, m_iIndexStride(rhs.m_iIndexStride)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_ePrimitive(rhs.m_ePrimitive)
{
	SAFE_ADDREF(m_pVB);
	SAFE_ADDREF(m_pIB);
}

HRESULT CVIBuffer::Bind_Resources()
{
	ID3D11Buffer* VertexBuffers[] = {
		m_pVB,
	};

	_uint		VertexStrides[] = {
		m_iVertexStride,
	};

	_uint		Offsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, VertexBuffers, VertexStrides, Offsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitive);


	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_bCloned) {
		Safe_Delete_Array(m_pVertexPositions);
		Safe_Delete_Array(m_pIndices);
	}

	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
}
