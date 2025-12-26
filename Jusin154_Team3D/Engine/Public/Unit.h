#pragma once

#include "Engine_Define.h"
#include "ContainerObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CUnit abstract : public CContainerObject
{
public:
	typedef struct tagObjectDesc
	{
		const _tchar* pModelPrototypeTag;
	}PARTS_OBJECT_DESC;

	struct PendingEvent
	{
		_float fRatio = 0.f;
		_uint AnimIndex = 0;
		function<void()> Callback;
		_bool bKeep = { false };
		_bool bExecuted = { false };
	};
protected:
	CUnit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUnit(const CUnit& Prototype);
	virtual ~CUnit() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	_wstring& Get_PrototypeTag() { return m_strModelPrototypeTag; }
	_bool IsAI() { return m_bAI; }

#ifdef _DEBUG
	void Load_KeyFrame();
#endif // _DEBUG

public:

#pragma region STATE
	pair<_uint, _bool> Get_AnimInfo(_uint iIndex) { return m_Animation[iIndex]; }
	virtual _bool Check(FSMSTATE::ESTATE state) { return false; }
	_float Get_KeyFrame(_string FrameName);
	_bool IsCurrentKeyFrame(_string FrameName);
	virtual _vector Get_LockOnPos() { return Get_WorldPostion(); }
	virtual pair<_float, _float> Get_Damage(_float fDamage) { return { 0.f, 0.f }; };

	virtual void Reset_Sprint() {};
	virtual void Reset_Walk() {};
#pragma endregion

protected:
	CShader*		m_pShaderCom = { nullptr };
	CModel*			m_pModelCom = { nullptr };
	CFSM*			m_pFSM = { nullptr };
	_wstring		m_strModelPrototypeTag;
	_bool			m_bRimLight = { true };
	_float			m_fRimLightPower = { 3.2f };
	_float			m_fRimLightStrength = { 3.04f };
	_float3			m_vRimLightColor = { 69.f / 255.f, 5.f / 255.f, 10.f / 255.f };

	_bool			m_bAI = {};

	map<_string, _float> m_KeyFrames;

	vector<pair<_uint, _bool>> m_Animation;

	unordered_map<size_t, CState*> m_States = { };
	size_t m_iStateMask = { 0 };

	_int m_eSpell = { };
	_int m_eHitSpell = {};
	_float m_fHitRadius = {};

	vector<PendingEvent> m_PendingEvents;

	_float m_fHitDegree = {};
	_float m_fHitCross = {};
	_int	m_eHitType = {};

private:
	virtual void Add_FSM() {};
	virtual void Set_FSM() {};
	virtual void Set_Anim() {};



protected:
	HRESULT Ready_Components(void*pArg);
	void Play_Event();
	void Add_Event(_uint AnimIndex, function<void()> Callback, _float fRatio = 0.f, _bool bKeep = false);
	void Check_HitAngle(_vector ProjectileDir);
public:
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)PURE;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
