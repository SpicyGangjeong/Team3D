#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CGoblin_BattleAxe final : public CPartObject
{
public:
	typedef struct tagGoblinBattleAxe : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrices = {  };
	}GOBLIN_BATTLEAXE_DESC;
private:
	CGoblin_BattleAxe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGoblin_BattleAxe(const CGoblin_BattleAxe& Prototype);
	virtual ~CGoblin_BattleAxe() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Render_Disolve();
	void Set_Disolve(_bool bDisolve) { m_bDisolve = bDisolve; }
	const _float4x4& Get_AxeMatrix() { return m_vAxeMat; };

private:
	const _float4x4* m_pSocketMatrices = {  };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	_bool m_bDisolve = { false };
	_float m_fDisolveTime = { 0.f };

	_float4x4 m_vAxeMat= {};


	PSX::PxSweepBufferN<12> m_SweepBufferAxe = {};
	PSX::PxSweepBufferN<12> m_SweepBufferGrip = {};
	const _float4* m_pAxePos = { nullptr };
	const _float4* m_pAxeGripPos = { nullptr };
	_float4 m_vStartAxePos = { };
	_float4 m_vStartGripPos = { };
	_bool m_bPlayerHit = {};

#ifdef _DEBUG
	unique_ptr<GeometricPrimitive> m_pGripShape = { nullptr };
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
#endif // _DEBUG
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	void CheckAxeHits(_uint& iHitCount, vector<PSX::PxSweepHit>& pxHits);

	void SwingHit(_bool& bPlayerHit);

public:
	static CGoblin_BattleAxe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
