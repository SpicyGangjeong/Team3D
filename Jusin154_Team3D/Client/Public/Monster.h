#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

class CMonster abstract : public CUnit
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& Prototype);
	virtual ~CMonster() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render_DeadDisolve();
	virtual HRESULT Render_OutLine() override;
	virtual void Set_DrawOutLine();
	_float4 Get_TargetPos() { return m_vTargetPos; }
	virtual pair<_float, _float> Get_Damage(_float fDamage);
	virtual _float2 Get_Hp();
	virtual CStat* Get_Stat();
	const _float4x4*  Get_HeadMatrix();
protected:
	CInfoInstance*	m_pInfoInstance = { nullptr };
	CUnit*			m_pTarget = { nullptr };
	CStat*			m_pStat = { nullptr };
	_float4			m_vTargetPos = { };
	_float3			m_vToTargetDir = { };
	_float			m_fTargetDistance = { FLT_MAX };
	_float			m_fDeadRatio = { 0.f };
	_bool			m_bDrawOutLine = { false };
	_bool			m_bLookAt = { true };
	_float4			m_vOutLineColor = CMyTools::ColorRGBA_HEXtoFLOAT4(0xfefefe00);
	_float			m_fOutLineThickness = { 5.f };
	_float			m_fOutLineScale = { 1.f };
	_float			m_fOutLinePower = { 1.f };
	_float			m_fDisolveTime = { 0.f };
	_bool			m_bDisolve = { false };
	_float			m_fDegree = {};
	_float			m_fCross = {};

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	void Set_Target(CUnit& pTarget, CTransform& pTransform);
	virtual HRESULT Render_Disolve();
public:
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)PURE;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG

};

NS_END
