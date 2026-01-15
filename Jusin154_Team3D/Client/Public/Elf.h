#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

class CElf final : public CUnit
{
public:
	typedef struct tagNpcInitDesc {
		_float4 vPos;
		_float4 vRotQ;
	}ELFDESC;


	enum class FLOW_TRIGGER
	{
		NONE,           // Idle 대기
		ANIM_END,       // 애니메이션 끝나면 Flow++
		DIALOGUE_END,   // 대화 끝나면 Flow++
		ARRIVE_POS,      // 위치 도착하면 Flow++
		END
	};

	struct DialogueAnim
	{
		_uint AnimIndex = {};
		_bool Loop = {};

		_int Trigger = {};
	};

	enum class ELF_STATE
	{
		IDLE,
		MOVE,
		OBSERVE,
		END
	};

private:
	CElf(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElf(const CElf& Prototype);
	virtual ~CElf() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow(SHADOW eType) override;
	virtual void OnRayCollision(CGameObject* pCaster, _uint iCastedOrder, _float fDistance, _float3 vCastedWorldPos)override;

	virtual _wstring Get_Name() override;
	virtual _wstring Get_NpcName() override;
	void Set_NpcState(_int iState) { m_eNpcState = iState; }
	void Set_Target(CUnit& pTarget, CTransform& pTransform);
	virtual void Set_Flow(_int Index, _float fTime) override;
	virtual _int Get_Flow() override;
protected:
	CInfoInstance* m_pInfoInstance = { nullptr };
	CCharacter_Controller* m_pCharacter_Controller = { nullptr };
	CRigidBody_Dynamic* m_pRigidBody = { nullptr };
	class CCallBack_NonPlayable_Behavior* m_pCallBack_Behavior = { nullptr };
	class CCallBack_NonPlayable_HitReport* m_pCallBack_HitReport = { nullptr };
	CUnit* m_pPlayerAllyUnit = { nullptr };

	CUnit*									m_pTarget = { nullptr };
	_float4									m_vTargetPos = { };
	_float3									m_vToTargetDir = { };
	_float									m_fTargetDistance = { FLT_MAX };

	CNPCStat*								m_pNpcStat = { nullptr };
	_int									m_iEntered = { 0 };
	_float2									m_vEnteringTimer = { 0.f, 1.f };
	_float									m_fEncounterDistance = { 8.f };


	_bool									m_bInteract = { false };
	_bool									m_bPreviousInteract = { false };
	_float									m_fInteractTime{};
	_int									m_eNpcState = ENUM_CLASS(ELF_STATE::END);

	_int									m_iFlowIndex{};
	_float									m_fFlowTime{};
	_bool									m_bFlow = { false };

	unordered_map<_int, _float4>			m_Points;


	unordered_map<_int, DialogueAnim>		m_Anims;

	DialogueAnim							m_pairAnimInfo;

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources() override;
	HRESULT Bind_ShaderParameters(_uint iMeshOrder);
	void MoveTo(_float fTimeDelta);
	void Set_Anim();
	virtual void Load_AnimXML(const string& path) override;
	HRESULT Load_ElfPos(const _char* pFilePath);


public:
	static CElf* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr);
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif // _DEBUG
};

NS_END
