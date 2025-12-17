#pragma once

#include "Editor_Define.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Editor)
class CQuest_Data final : public CGameObject
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
		_wstring					pQuestName;
		_wstring					pQuestInfo;
		vector<OBJECTIVEINFO>	ObjectiveInfo;
		vector<REWARDSINFO>		RewardsInfo;
		_int					iAcceptState;
	}QUESTINFO;
private:
	CQuest_Data(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Data(const CQuest_Data& rhs);
	virtual ~CQuest_Data() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

public:
	QUESTINFO Get_Quest(_int QuestID);
	_int Get_Count(_int Index);
	const vector<QUESTINFO>& Get_AllQuest() const;
	const vector<QUESTINFO>& Get_ClearQuest() const;
	const vector<QUESTINFO>& Get_AcceptQuest() const;
	HRESULT Set_AcceptQuest(_int index);
	void Set_ClearQuest(_int Index);
	void Update_AcceptQuest(_int MonsterIndex = -1);

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	//const SPELLINFO& Get_Info(_uint SpellID) const;

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CGameObject* m_pQuestInstance = { nullptr };


	QUESTINFO QuestInfo = {};
	OBJECTIVEINFO ObjectiveInfo = {};
	REWARDSINFO RewardsInfo = {};

	vector<QUESTINFO> QuestInfos;
	vector<QUESTINFO> m_QuestNoneInfos;
	vector<CGameObject*> m_QuestAcceptedInfoObject;
	vector<QUESTINFO> m_QuestAcceptedInfos;
	vector<QUESTINFO> m_QuestEndInfos;

	_int m_iQuest_Count{};
	_int m_iClearQeustCount{};
	_int m_iAcceptQeustCount{};

	_bool m_bClear[1] = { false };
public:
	static CQuest_Data* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
