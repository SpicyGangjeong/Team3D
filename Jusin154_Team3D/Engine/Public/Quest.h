#pragma once

#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CQuest abstract : public CGameObject
{
public:
	typedef struct tagObjectiveInfo
	{
		_bool	bClear{};
		_int	iTargetID{};
		_int	iRequiredCount{};
		_int	iCurrentCount{};
	}OBJECTIVEINFO;

	typedef struct tagRewardsInfo
	{
		_int iRewardType{};
		_int iRewardID{};
	}REWARDSINFO;

	typedef struct tagQuestInfo
	{
		_int					iQuestID{};
		_int					iType{};
		_wstring				pQuestName;
		_wstring				pQuestInfo;
		vector<OBJECTIVEINFO>	ObjectiveInfo;
		vector<REWARDSINFO>		RewardsInfo;
		_int					iAcceptState;
	}QUESTINFO;
protected:
	CQuest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest(const CQuest& rhs);
	virtual ~CQuest() = default;

protected:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual _bool Update_Objective(_int MonsterID);
	virtual _int Get_QuestID();

protected:
	vector<OBJECTIVEINFO> m_ObjectiveInfo;
	_int m_iQuestID{};

public:
	virtual void Free() override;
};

NS_END
