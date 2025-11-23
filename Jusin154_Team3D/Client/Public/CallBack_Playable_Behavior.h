#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CCallBack_Playable_Behavior final : public PSX::PxControllerBehaviorCallback
{
	// 구현 예시 
	// CCT는 RigidBody의 PxSimulationEventCallback에 안걸리고, 이 전용 콜백에서 처리되는 구조임
	// 
	// PxUserControllerHitReport		= "무엇과 부딪혔는지 알려줌(이벤트 알림)"
	// PxControllerBehaviorCallback		= "부딪힌 것에 대해 어떤 행동 정책을 쓸지 물어봄(질의)"
	// PxSimulationEventCallback		= "씬 전체에 대한 글로벌 콜백(일반 물리 이벤트)" 
private:
	CCallBack_Playable_Behavior();
public:
	~CCallBack_Playable_Behavior();

public:
	// PxControllerBehaviorCallback을(를) 통해 상속됨
	// 충돌알림이 아닌, 행동정책 정해주기. CCT vs CCT는 지원안함
	// slopeLimit로 미끄러져 갈 수 있는지 없는지 이분법적인 계산 대신, 원하는 물체에 대해서만 미끄러운 표면으로 정의해줌
	// 주인이 되는 CCT가 이 물체에 대해 이 물체를 어떻게 해야 할까? -> (타고 올라갈까? Ride?) (미끄러져갈까? Slide?) ( 직접 내가 구현할까? user defined ride? )
	virtual PSX::PxControllerBehaviorFlags getBehaviorFlags(const PSX::PxShape& shape, const PSX::PxActor& actor) override;

	virtual PSX::PxControllerBehaviorFlags getBehaviorFlags(const PSX::PxController& controller) override;

	virtual PSX::PxControllerBehaviorFlags getBehaviorFlags(const PSX::PxObstacle& obstacle) override;

public:
	HRESULT Initialize(CCharacter_Controller* pController, CRigidBody_Dynamic* pPartDynamicObject);
	HRESULT Finalize();
private:
	CCharacter_Controller*	m_pController = { nullptr };
	CRigidBody_Dynamic*		m_pPartDynamicBody = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

public:
	static CCallBack_Playable_Behavior* Create();
};
NS_END
