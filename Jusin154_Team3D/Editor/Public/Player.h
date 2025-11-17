#pragma once

#include "Editor_Define.h"
#include "Unit.h"

NS_BEGIN(Editor)

class CPlayer final : public CUnit
{
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

public:
	virtual _bool IsWalking() override;
	virtual _bool IsDodge() override;
	virtual _bool IsSprint() override;
	virtual _bool IsJump() override;
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

	virtual void Add_FSM();
	virtual void Set_Anim();
	void Check_State();


public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
