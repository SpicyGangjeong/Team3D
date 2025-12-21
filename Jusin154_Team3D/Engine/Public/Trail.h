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
	HRESULT Load_Trail(TRAIL_DESC TrailDesc);
	HRESULT Load_Trail(HANDLE hFile);

public:
	void	Rope_Trail_Update(_fmatrix WorldMatrix, _float fTimeDelta  , _float fDamping, _float fLength, _float fMass, _fmatrix EndWorldMatrix);
	void    Rope_Trail_Update(_fmatrix WorldMatrix, _float fTimeDelta, _float fDamping, _float fLength, _float fMass);
	void    Rope_Fix(_bool isFix) { m_isFix = isFix; }
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT			Create_VB();
	HRESULT			Create_IB();

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

	_float3	    m_PreHigh[4] = {};
	_float3	    m_PreLow[4] = {};

	_vector     m_PreFixedHigh[3] = {};
	_vector     m_PreFixedLow[3] = {};

	VTXPOSTEX*  m_pVertices = { nullptr };
	
	_float3*    m_pOldPosition = { nullptr };

	_float		m_fAccTime = {};
	_float		m_fTrailUpdateTime = {};

	_uint       m_iNumVerticesPerInstance = {};


	/* 고정 트레일*/
	_bool		m_isFix = {};
	_float      m_fGravity = { 9.8f };

};

NS_END
