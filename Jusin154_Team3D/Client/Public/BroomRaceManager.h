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
		_float4      prevPos;
	};

	enum class RACE_STATE
	{
		READY,
		COUNTDOWN,
		START,
		RACING,
		FINISH,
		END
	};


private:
	CBroomRaceManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroomRaceManager(const CBroomRaceManager& Prototype);
	virtual ~CBroomRaceManager() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	void SetTargetRing(CGameObject* pRacer);
	void Push_BroomRacer(RacerInfo Info);
	void Push_RaceRing(class CRaceRing* Ring);
	_int Get_RaceState() const { return m_eRaceState; }

	HRESULT Load_RaceRing();
	HRESULT	Load_Balloons();

private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	vector<class CRaceRing*>			m_pRaceRings = {};
	vector<RacerInfo>					m_Racers = {};
	_float								m_fCountTimer = {};
	_float								m_fRacingTimer = {};
	_int								m_iCount = { 3 };
	_int								m_iPerCount = {};
	_int								m_eRaceState = { ENUM_CLASS(RACE_STATE::END) };
	_wstring							ToolTip;

	_int								m_iLastRing{};

	_float								m_fRedayTime{};
	_bool								m_bRaceStart = { false };
	_bool								m_bRaceReady = { false };
	_float4								m_OriginPos = {};
	_float								m_fDelay = {};
	_bool								m_bRaceEnd = {};

	_bool								m_bReturnPosition = { false };
	_bool								m_bCurrentRace = { false };
private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT			Ready_Components();
	HRESULT			Bind_ShaderResources() override;	
	void			Update_Countdown(_float fTimeDelta);
	void			StartRaceMove();
	void			Check_RingPassed();
	void			Finish();

	void			RaceReady();

public:
	static CBroomRaceManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif 

};

NS_END
