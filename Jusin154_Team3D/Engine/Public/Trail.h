#pragma once
#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CTrail final :
	public CVIBuffer
{
public:
	typedef struct tagTrailDesc
	{
		_float3		vHigh;
		_float3		vLow;
	}TRAIL_DESC;
protected:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	void Trail_Update(_float fDeltaTime, _fmatrix WorldMatrix);
	void Reset_Trail();
	HRESULT ReStructVB(_uint iNumVertices);

#ifdef _DEBUG
	HRESULT Save_Trail(HANDLE hFile);
#endif
	HRESULT Load_Trail(HANDLE hFile);

public:
	void	Fixed_Trail(_fmatrix WorldMatrix);

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT			Create_VB();

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CTrail* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
	virtual HRESULT Render() override;
private:
	TRAIL_DESC  m_TrailDesc = {};
	
	_uint		m_iNumCount = {};

	_vector	    m_PreHigh[2] = {};
	_vector	    m_PreLow[2] = {};

	_vector     m_PreFixedHigh[3] = {};
	_vector     m_PreFixedLow[3] = {};

	VTXPOSTEX*  m_pVertices = { nullptr };

	_float		m_fAccTime = {};
	_float		m_fTrailUpdateTime = {};

	_uint       m_iNumVerticesPerInstance = {};

	_matrix     m_FixedMat = {};
	_bool		m_isFixedTrail = {};
};

NS_END
