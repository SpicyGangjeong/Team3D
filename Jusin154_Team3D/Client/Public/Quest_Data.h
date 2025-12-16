#pragma once

#include "Client_Define.h"
#include "Base.h"

NS_BEGIN(Engine)
class CGameInstance;
class CUIObject;
NS_END

NS_BEGIN(Client)

class CQuest_Data final : public CBase
{
private:
	CQuest_Data();
	~CQuest_Data() = default;

public:
	virtual void Update(_float fTimeDelta);

public:
	HRESULT Load_QuestInfo(const _char* pFilePath);
	QUESTINFO Get_Quest(_int QuestID);
	_int Get_Count();
	//const SPELLINFO& Get_Info(_uint SpellID) const;
	
private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	QUESTINFO QuestInfo = {};
	OBJECTIVEINFO ObjectiveInfo = {};
	REWARDSINFO RewardsInfo = {};

	vector<QUESTINFO> QuestInfos;

	_int m_iQuest_Count{};

	_bool m_bClear[1] = { false };

private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);

public:
	static CQuest_Data* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
