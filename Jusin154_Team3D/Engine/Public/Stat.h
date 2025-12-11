#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CStat final : public CComponent
{
	typedef struct tagUnitInfo
	{
		_wstring		pUnit_Name;
		_float			fCurrentHp{};
		_float			fMaxHp{};
		_float			fTargetHp{};
		_float			fDamage{};
		_float			fDefense{};
		_float			fSpeed{};
		_int			iLevel{};
		_float			fExprince{};
		_float			fMaxExprience{};
		_int			iGold{};
		_bool			bBoss = false;
	}UNITINFO;
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

private:
	_float Compute_Damage(_float Damage);

public:
	UNITINFO Get_Stat();
	void Set_Stat(_int _iStatIndex, _float _iAmount);
	void Add_Stat(_int _iStatIndex, _float _iAmount);

	void Add_Hp(_float iAmount);
	pair<_float, _float> Get_Damage(_float fDamage);

private:
	UNITINFO m_UnitInfo;
	//_wstring m_pName;
	//_float m_StatArr[ENUM_CLASS(STAT::END)];
	//_bool m_ConditionArr[ENUM_CLASS(CONDITION::END)] = { false };
};

NS_END
