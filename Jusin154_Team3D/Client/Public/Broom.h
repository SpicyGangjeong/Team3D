#pragma once

#include "Client_Define.h"
#include "Unit.h"

NS_BEGIN(Client)

class CBroom final : public CUnit
{
private:
	CBroom(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroom(const CBroom& Prototype);
	virtual ~CBroom() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	_bool m_bSprintToggle = { false };
	_bool m_bWalkToggle = { false };
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBroom* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG

#pragma region STATE
	virtual void Add_FSM();
	virtual void Set_Anim();

	_float m_fAmount = { 1.f };
	_float m_fInputTime = {};
	_bool m_bRatio = { false };

	HRESULT InputAction();
	HRESULT InputMove();
	HRESULT InputKeyUpMove();
	HRESULT InputSpell();
	HRESULT InputAim();

	void	Behavior_IdleEnter();
	HRESULT Behavior_IdleExitCheck(_float fTimeDelta);
	void	Behavior_IdleExit();

	void	Behavior_MoveEnter();
	HRESULT Behavior_MoveExitCheck(_float fTimeDelta);
	void	Behavior_MoveExit();

#pragma endregion


};

NS_END
