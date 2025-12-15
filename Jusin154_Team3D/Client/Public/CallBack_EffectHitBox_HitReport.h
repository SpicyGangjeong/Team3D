#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CCallBack_EffectHitBox_HitReport final : public PSX::PxUserControllerHitReport
{
	// 구현 예시 
	// CCT는 RigidBody의 PxSimulationEventCallback에 안걸리고, 이 전용 콜백에서 처리되는 구조임
	// 
	// PxUserControllerHitReport		= "무엇과 부딪혔는지 알려줌(이벤트 알림)"
	// PxControllerBehaviorCallback		= "부딪힌 것에 대해 어떤 행동 정책을 쓸지 물어봄(질의)"
	// PxSimulationEventCallback		= "씬 전체에 대한 글로벌 콜백(일반 물리 이벤트)" 
private:
	CCallBack_EffectHitBox_HitReport();
public:
	~CCallBack_EffectHitBox_HitReport();

	void BeginFrame();

public:
	// PxUserControllerHitReport을(를) 통해 상속됨
	// 충돌알림!!
	// 주인이 되는 CCT가 이 물체에 대해 어떻게 반응해야 할까?
	virtual void onShapeHit(const PSX::PxControllerShapeHit& hit) override; // statc, rigid 등과 부딪힌 경우

	virtual void onControllerHit(const PSX::PxControllersHit& hit) override; // 다른 CCT와 부딪힌 경우

	virtual void onObstacleHit(const PSX::PxControllerObstacleHit& hit) override; // 사용자 정의 장애물과 부딪힌 경우

	void Set_CurrentSlop();

public:
	HRESULT Initialize(CCharacter_Controller* pController, CRigidBody_Dynamic* pPartDynamicObject);
	HRESULT Finalize();
	_bool	IsOnGround();
	_vector Get_GroundVector();
private:
	CCharacter_Controller* m_pController = { nullptr };
	CRigidBody_Dynamic* m_pPartDynamicBody = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

	_float3		m_vClimbNormal = { 0.f, 1.f, 0.f };
	_float		m_fBestNormal = { 0.f };
	_bool		m_bGroundHit = { false };
public:
	static CCallBack_EffectHitBox_HitReport* Create();
};
NS_END
