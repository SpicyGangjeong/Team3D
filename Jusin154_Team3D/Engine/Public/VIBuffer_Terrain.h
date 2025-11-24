#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
	typedef struct VIBuffeer_Terrain_Desc
	{
		_uint iSizeX{}, iSizeZ;
		_float fPosY{};
	}VIBUFFER_TERRAIN_DESC;
private:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

	_float2 Get_Size() { return _float2((_float)m_iNumVerticesX, (_float)m_iNumVerticesZ); }

public:
	virtual HRESULT Initialize_Prototype(const _char* pFilePath, _uint iSizeX, _uint iSizeZ);
	virtual HRESULT Initialize(void* pArg);

	_bool	Picking(_fmatrix WorldMatrix, _float3* pOut);
	void	Culling(_fmatrix WorldMatrix);
	void	FitY(_fmatrix WorldMatrix, _float fY);
	void    Change_HeigthRatio(_float fRatio);

#ifdef _DEBUG
	void	Set_CullingRadius(_float fRaduis);
	HRESULT Save_HeightMap(const _char* pFilePath);
	HRESULT Load_HeightMap(const _char* pFilePath);
#endif // _DEBUG


private:
	_bool				m_bChange = {};
	_uint				m_iNumVerticesX = {};
	_uint				m_iNumVerticesZ = {};

	vector<_float>		m_HeigthValues = {};

	class CQuadTree*	m_pQuadTree = { nullptr };

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pFilePath, _uint iSizeX, _uint iSizeZ);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
