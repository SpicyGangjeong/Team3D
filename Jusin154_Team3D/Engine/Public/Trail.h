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

#ifdef _DEBUG
	HRESULT Save_Trail(HANDLE hFile);
#endif
	HRESULT Load_Trail(HANDLE hFile);

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

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

	VTXPOSTEX*  m_pVertices = { nullptr };

	_float		m_fAccTime = {};
	_float		m_fTrailUpdateTime = {};

	_uint       m_iNumVerticesPerInstance = {};
};

NS_END
