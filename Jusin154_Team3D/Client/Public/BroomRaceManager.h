#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CBroomRaceManager final : public CGameObject
{
public:
	struct RacerInfo
	{
		class CPlayer* pRacer = nullptr;
		class CBroomRacerAI* pAI = nullptr;
		_uint       curRing = 0;
		_vector      prevPos = XMVectorZero();
	};

private:
	CBroomRaceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroomRaceManager(const CBroomRaceManager& Prototype);
	virtual ~CBroomRaceManager() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	void SetTargetRing(CGameObject* pRacer);
	void Push_BroomRacer(RacerInfo Info);
	void Push_RaceRing(class CRaceRing* Ring);
private:
	vector<class CRaceRing*>			m_pRaceRings = {};
	vector<RacerInfo>					m_Racers = {};


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT			Ready_Components();
	HRESULT			Bind_ShaderResources() override;
	void			Check_RingPassed();

public:
	static CBroomRaceManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;

#endif 

};

NS_END
