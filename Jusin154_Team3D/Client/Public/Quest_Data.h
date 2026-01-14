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
	struct QuestAccepted
	{
		CGameObject* QeustData;
		QUESTINFO QeustInfo;
	};
private:
	CQuest_Data();
	~CQuest_Data() = default;

public:
	virtual void Update(_float fTimeDelta);

private:
	HRESULT Load_QuestInfo(const _char* pFilePath);
	void Set_ClearQuest(_int Index);
	void Update_AcceptQuest(_int MonsterIndex = -1);

public:
	QUESTINFO Get_Quest(_int QuestType, _int QuestID);
	_int Get_Count(_int Index);
	const vector<QUESTINFO>& Get_AllQuest() const;
	const vector<QUESTINFO>& Get_ClearQuest() const;
	vector<QUESTINFO>& Get_AcceptQuest();
	_int Set_AcceptQuest(_int index);

private:
	CInfoInstance* m_pInfoInstance = { nullptr };
	CGameObject* m_pQuestInstance = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	QUESTINFO QuestInfo = {};
	OBJECTIVEINFO ObjectiveInfo = {};
	REWARDSINFO RewardsInfo = {};

	QuestAccepted m_AcceptedInfo{};

	vector<QUESTINFO> QuestInfos;
	vector<QUESTINFO> m_QuestNoneInfos;
	vector<QuestAccepted> m_QuestAcceptedInfos;
	vector<QUESTINFO> m_QuestAcceptedInfo;
	vector<QUESTINFO> m_QuestEndInfos;

	_int m_iQuest_Count{};
	_int m_iClearQeustCount{};
	_int m_iAcceptQeustCount{};

	_bool m_bClear[1] = { false };

private:
	HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContex);

public:
	static CQuest_Data* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
