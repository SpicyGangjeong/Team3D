#pragma once
#include "Component.h"

NS_BEGIN(Engine)
class CTransform;
class ENGINE_DLL CCharacter_Controller final :
	public CComponent
{
	/* dev-treadmill.tistory.com/158 */
	/* dev-treadmill.tistory.com/159 */
	// 기본적으로 아무 힘도 받지 않는, 주지 않는 놈임
	// kinematic 기반임
public:
#pragma region STRUCT
	struct CapsuleInfo
	{
	};
	typedef struct tagCharacter_ControllerDesc
	{
		CTransform*			pTransform			= { nullptr };
		ACTOR				eBodyType			= { ACTOR::END };
		_float				fContactOffset		= { 0.f };
		_float3				fMaterial			= { 0.5f, 0.5f, 0.6f };
		_bool				bAutoStepping		= { false }; // 계단 높이 타게 할건지 // 박스 콜라이더는 생각한대로 되는데 캡슐은 바닥의 구체 때문에 예상보다 더 높은 곳까지 오를 수 있음
		_float				fStepOffset			= { 0.05f }; // 허용가능한 계단 높이, 작게 유지하는게 좋음

		PSX::PxUserControllerHitReport*		pCallback_HitReport = { }; // move가 호출될 때 호출 됨 // dll을 받는 각 프로젝트에서 구현해야함
		PSX::PxControllerBehaviorCallback*	pCallback_Behavior	= { }; // move가 호출될 때 호출 됨 // dll을 받는 각 프로젝트에서 구현해야함

		// 타고 올라갈 수 없는 경사면의 각도를 정의함.  0.f 면 비활성화 됨
		// _float fSlopeLimit = { 코사인각도 };
		
		
		
		// Box인 경우 채워넣기
		_float3				vBoxSize = { 0.5f, 0.5f, 0.5f };						// CCT박스는 회전없는 AABB임

		// Capsule인 경우 채워넣기
		_float fRadius;
		_float fHeight;
		PSX::PxCapsuleClimbingMode::Enum eClimbingMode; // 기본 (eEASY, stepOffset 값이 무제한), (eCONSTRAINED, stepOffset 값이 제대로 먹음)
	}Character_Controller_DESC;
#pragma endregion
private:
	CCharacter_Controller(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter_Controller(const CCharacter_Controller& rhs);
	virtual ~CCharacter_Controller() = default;

public:
#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif
	PSX::PxRigidDynamic* Get_Actor();

	_float			Get_ContactOffset() const { return m_pController->getContactOffset(); }
	void			Set_ContactOffset(_float fValue) { return m_pController->setContactOffset((PSX::PxF32)fValue); }

	void			Move(_float fTimeDelta);		// 이동 // 충돌보정만 있음, 
	void			Set_Position(_fvector vNewPos);	// 순간이동
	_vector			Get_Position();
	_float3			Get_FootPosition();				// ContactOffset이 고려된 발바닥 위치( 실제보다 바닥 위치가 더 아래에 위치한다는 뜻 )

	void			Resize_Volume(_float fHeight);	// 높이를 수정하고 바닥에 붙임
	void			Modify_Volume(_float3 fVolume);
	_float3			Get_Volume();

private:
	ACTOR					m_eBodyType = { ACTOR::END };
	PSX::PxController*		m_pController = { nullptr };
	CTransform*				m_pTransform = { nullptr };
	_bool					m_bAutoStepping = { false };
	_float					m_fSlopeLimit = { 0.f }; // cosf각도, 오르막 경사각 제한. ( 추가설정 필요함 )
	PhsXUserData			m_tagData = {};
	
	PSX::PxControllerCollisionFlags m_eBeforeCollisionFlags = {};
#ifdef _DEBUG
	unique_ptr<GeometricPrimitive> m_pMainShape = { nullptr };
	unique_ptr<GeometricPrimitive> m_pSubShape = { nullptr };
#endif // _DEBUG

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
#ifdef _DEBUG
	virtual HRESULT Debug_Initialize();
#endif // _DEBUG


	_bool Check_Overlap();

public:
	static CCharacter_Controller* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
	virtual void Describe_Entity() override;
};

NS_END
