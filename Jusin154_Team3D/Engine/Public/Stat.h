#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CStat final : public CComponent
{
private:
	CStat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStat(const CStat& rhs);
	virtual ~CStat() = default;

private:
	virtual HRESULT Initialize_Prototype(tinyxml2::XMLNode* pChild);
	virtual HRESULT Initialize(void* pArg);

public:
	static CStat* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, tinyxml2::XMLNode* pChild);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG

public:
	_float Get_Stat(_int _iStatIndex);
	void Set_Stat(_int _iStatIndex, _float _iAmount);
	void Add_Stat(_int _iStatIndex, _float _iAmount);
	_bool Get_Condition(_uint _iConditionIndex) const;
	void Set_Condition(_uint _iConditionIndex, _bool _Condition);

	void Add_Hp(_float iAmount);
	_bool Get_Damage(_float fDamage);

private:
	_float m_StatArr[ENUM_CLASS(STAT::END)];
	_bool m_ConditionArr[ENUM_CLASS(CONDITION::END)] = { false };
};

NS_END
