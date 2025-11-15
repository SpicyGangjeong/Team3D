#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(NAVI_POINT ePoint);
	const _float3* Get_ConstPoint(NAVI_POINT ePoint);
	const _float3* Get_Normarl(NAVI_LINE eLine);
	void Set_Neighbor(NAVI_LINE eLine, CCell* pNeighbor);
	_int Get_Neighbor(NAVI_LINE eLine);
	_ubyte Get_CellAttribute();

public:
	HRESULT isIn(_fvector vLocalPos, _float* fOuts, _uint& iCrossCnt);
	_bool Compare(_fvector vSourPoint, _fvector vDestPoint);
	_float Compute_Height(_fvector vPoint);

	inline void	 Enable_Attribute(_ubyte attributeMask) { m_ubyteCellAttribute |= attributeMask; }
	inline void	 Disable_Attribute(_ubyte attributeMask) { m_ubyteCellAttribute &= ~attributeMask; }
	inline _bool IsEnable(_ubyte attributeMask) const { return (m_ubyteCellAttribute & attributeMask) != 0; }
	inline void	 Set_Attribute(_ubyte attributeMask) { m_ubyteCellAttribute = attributeMask; }

	HRESULT LoadAsBinary(HANDLE hFile, DWORD& dwByte, _uint iCellIndex);
#ifdef _DEBUG
public:
	HRESULT SaveAsBinary(HANDLE hFile, DWORD& dwByte);
	void ReallocatedCellIndex(_uint iIndex);
	void Change_PrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY eType = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	HRESULT Render();
#endif

private:
	_uint					m_iIndex = { UINT_MAX };
	_float3					m_vPoints[ENUM_CLASS(NAVI_POINT::END)] = {};
	_float3					m_vNormals[ENUM_CLASS(NAVI_LINE::END)] = {};
	_float3					m_vLines[ENUM_CLASS(NAVI_LINE::END)] = {};

	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

	_int					m_NeighborIndices[ENUM_CLASS(NAVI_LINE::END)] = { -1, -1, -1 };
	_ubyte					m_ubyteCellAttribute = { 1 };

	_float4					m_vPlane = {};

#ifdef _DEBUG	
	class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

private:
	HRESULT Initialize(const _float3* vPoints, _uint iIndex);

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* vPoints = nullptr, _uint iIndex = 0);
	virtual void Free();
};

NS_END