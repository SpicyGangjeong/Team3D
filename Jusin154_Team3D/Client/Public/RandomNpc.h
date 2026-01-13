#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

class CRandomNpc final : public CUnit
{
public:
	typedef struct tagNpcInitDesc {
		_float4 vPos;
		_float4 vRotQ;
		_int iIndex;
	}NPCDESC;

	enum class NPC_STATE
	{
		IDLE,
		MOVE,
		OBSERVE,
		END
	};

protected:
	CRandomNpc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRandomNpc(const CRandomNpc& Prototype);
	virtual ~CRandomNpc() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	virtual void OnRayCollision(CGameObject* pCaster, _uint iCastedOrder, _float fDistance, _float3 vCastedWorldPos)override;
	
	virtual _wstring Get_Name() override;
	virtual _wstring Get_NpcName() override;
	void Set_Battle(_bool bBattle) { m_bBattle = bBattle; }
	_bool Get_BattleObserve_Npc() { return m_bBattleObserve_Npc; }
	void Set_NpcState(_int iState) { m_eNpcState = iState; }
	void Set_Target(CUnit& pTarget, CTransform& pTransform);
	void Set_BattleIndex(_int iIndex) { m_iBattleIndex = iIndex; }
protected:
	CInfoInstance*							m_pInfoInstance = { nullptr };
	CCharacter_Controller*					m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic*						m_pRigidBody = { nullptr };
	class CCallBack_NonPlayable_Behavior*	m_pCallBack_Behavior = { nullptr };
	class CCallBack_NonPlayable_HitReport*	m_pCallBack_HitReport = { nullptr };
	CUnit*									m_pPlayerAllyUnit = { nullptr };
	CRandomNpc*								m_pNPC_Ollivander = { nullptr };

	/*class CNPCInteraction* m_pNPCInteraction = { nullptr };*/
	CUnit*									m_pTarget = { nullptr };
	_float4									m_vTargetPos = { };
	_float3									m_vToTargetDir = { };
	_float									m_fTargetDistance = { FLT_MAX };
	_float									m_fDegree = {};
	_float									m_fCross = {};

	CNPCStat*								m_pNpcStat = { nullptr };
	_int									m_iEntered = { 0 };
	_float2									m_vEnteringTimer = { 0.f, 1.f };
	_float									m_fEncounterDistance = { 8.f };
	_int									m_iIndex = {};


	_bool									m_bInteract = { false };
	_bool									m_bPreviousInteract = { false };
	_float									m_fInteractTime{};
	_bool									m_bBattleObserve_Npc = {};
	_bool									m_bBattle = {};
	_int									m_eNpcState = ENUM_CLASS(NPC_STATE::END);
	_bool									m_bBroomRacer = {};
	_bool									m_bElf = {};
	_int									m_iBattleIndex = {};

	vector<_int>							m_Anims;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources() override;
	HRESULT Bind_ShaderParameters(_uint iMeshOrder);

private:

	void BattleObserve_Anim();

	void Load_AnimXML(const _char* pFilePath);

public:
	static CRandomNpc* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr);
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG
};

NS_END
