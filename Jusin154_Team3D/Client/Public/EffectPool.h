#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CLight;
NS_END

NS_BEGIN(Client)

class CEffectPool final : public CGameObject
{

private:
	CEffectPool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectPool(const CEffectPool& Prototype);
	virtual ~CEffectPool() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	HRESULT Create_Effect(SKILL_TYPE eType, _uint iNumEffect, _uint iPrototypeLevel, _uint iCloneLevel , function<class CEffect_Container*(_uint , _uint)> AddPrototypeEvent);
	HRESULT Use_Skill(SKILL_TYPE eType , CGameObject* pOwner, void* pArg = nullptr);
private:
	list<class CEffect_Container*> m_EffectList[ENUM_CLASS(SKILL_TYPE::END)] = {};
	list<class CEffect_Container*> m_ActiveEffectList = {};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Ready_Effect();
	HRESULT Ready_MonsterEffect();
public:
	static CEffectPool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG

};

NS_END

