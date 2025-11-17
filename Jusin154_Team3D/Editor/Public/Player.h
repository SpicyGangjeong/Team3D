#pragma once

#include "Editor_Define.h"
#include "Unit.h"

NS_BEGIN(Editor)

class CPlayer final : public CUnit
{
public:
	struct InputCondition
	{
		FSMSTATE::ESTATE state;
		function<_bool()> checker;
	};

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	vector<InputCondition> m_InputConditions;

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	void Setup_InputConditions();
	void Key_Input(_float fTimeDelta);

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;

#pragma region STATE
private:
	virtual void Add_FSM();
	virtual void Set_FSM();
	virtual void Set_Anim();
	_bool Check(FSMSTATE::ESTATE state);



#pragma endregion




};

NS_END
