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
		_int iObjectType{};
		_int iTargetID{};
		_int iRequiredCount{};
		_int iCurrentCount{};
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
		_string					pQuestName;
		_string					pQuestInfo;
		vector<OBJECTIVEINFO>	ObjectiveInfo;
		vector<REWARDSINFO>		RewardsInfo;
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

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	//const SPELLINFO& Get_Info(_uint SpellID) const;

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	QUESTINFO QuestInfo = {};
	OBJECTIVEINFO ObjectiveInfo = {};
	REWARDSINFO RewardsInfo = {};

	vector<QUESTINFO> QuestInfos;

	_bool m_bClear[1] = { false };
public:
	static CQuest_Data* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
	void Describe_Entity() override;
};

NS_END
