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
#ifdef EDITOR_PROJECT
	virtual HRESULT Initialize_Prototype(const _char* pFilePath);
#endif // EDITOR_PROJECT
	virtual HRESULT Initialize_Prototype(const _char* pFilePath, _uint iSizeX, _uint iSizeZ);
	virtual HRESULT Initialize(void* pArg);
	void ConvertToHeightField(const _tchar* pStaticKey);

	_bool	Picking(_fmatrix WorldMatrix, _float3* pOut);
	void	Culling(_fmatrix WorldMatrix);
	void	FitY(_fmatrix WorldMatrix, _float fY);

	HRESULT Load_HeightMap(const _char* pFilePath);

#ifdef EDITOR_PROJECT
	void    Change_HeigthRatio(_float fRatio);
	void	Set_CullingRadius(_float fRaduis);
	HRESULT Save_HeightMap(const _char* pFilePath);
#endif 



private:
	_bool				m_bChange = {};
	_uint				m_iNumVerticesX = {};
	_uint				m_iNumVerticesZ = {};

	vector<_float>		m_HeigthValues = {};

	class CQuadTree*	m_pQuadTree = { nullptr };

public:
#ifdef EDITOR_PROJECT
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pFilePath);
#endif // EDITOR_PROJECT
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pFilePath, _uint iSizeX, _uint iSizeZ);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
