#pragma once

#include "Client_Define.h"
#include "Monster.h"
#include "CallBack_Monster_Behavior.h"
#include "CallBack_Monster_HitReport.h"

NS_BEGIN(Engine)
class CTransform;
NS_END

NS_BEGIN(Client)

class CGoblin_Spector final : public CMonster
{
public:
	typedef struct tagSpector
	{
		CTransform* pParentTransform = { nullptr };
	}GOBLIN_SPECTOR_DESC;
private:
	CGoblin_Spector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGoblin_Spector(const CGoblin_Spector& Prototype);
	virtual ~CGoblin_Spector() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void OnCollision(CGameObject* pOther = nullptr, void* pDesc = nullptr)override;
	virtual void OnHit(CGameObject* pOther, CGameObject* pCaller = nullptr)override;

public:
	void    Spector_Trail_Visible(_bool isTrailVisible);
	void	Set_Disolve(_bool bDisolve) { m_bDisolve = bDisolve; }


private:
	class CGoblin* m_pGoblin = { nullptr };
	CTransform* m_pParentTransformCom = { nullptr };
	_uint iIndex;
	_float3 m_Offset = {};
	_float3 m_vOriginScale = {};
	_float3 m_vScale = { 1.f, 1.f, 1.f };
	_bool m_bDisolve = { false };
	_float m_fDisolveTime = {0.f};


	class CTrailObject* m_pLeft_Trail = { nullptr };
	class CTrailObject* m_pRight_Trail = { nullptr };

	const _float4x4* m_pLeftHand_BoneMat = { nullptr };
	const _float4x4* m_pRightHand_BoneMat = { nullptr };

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Ready_Parts();
	HRESULT Bind_ShaderResources();
	virtual HRESULT Render_Disolve();

public:
	static CGoblin_Spector* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG


	_bool m_bStep = { false };
	_float m_fTpTime = {};
	_float m_fAirTime = {};
	_vector m_vOriginPos = {};
	_float m_fLength = {};

};

NS_END
