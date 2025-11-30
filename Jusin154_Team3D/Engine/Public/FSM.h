#include "Component.h"

NS_BEGIN(FSMSTATE)
enum ESTATE : _u64Flag
{
    NOT_DEFINE                  = 0,
    IDLE                        = 1ULL << 0,
    IDLE_TURN                   = 1ULL << 1,
    MOVE                        = 1ULL << 2,
    WALK                        = 1ULL << 3,
    JOG                         = 1ULL << 4,
    SPRINT                      = 1ULL << 5,
    JUMP                        = 1ULL << 6,
    LAND                        = 1ULL << 7,
    DODGE                       = 1ULL << 8,
    COMBAT                      = 1ULL << 9,
    LIGHT_ATTACK                = 1ULL << 10,
    SPELL                       = 1ULL << 11,
    SKILL                       = 1ULL << 12,
    SKILL2                      = 1ULL << 13,
	ANCIENT_THROW               = 1ULL << 14,        
    MAPHELP                     = 1ULL << 15,
	POTION                      = 1ULL << 16,
    STOP                        = 1ULL << 17,
	HIT                         = 1ULL << 18,
	BROOM_RIDE                  = 1ULL << 19,
	MOUNT_B                     = 1ULL << 20,
    END
};
NS_END

NS_BEGIN(Engine)

class CState;

class ENGINE_DLL CFSM final : public CComponent
{
public:
    typedef struct tagFSMDesc
    {
        unordered_map<size_t, class CState*>* pStates = { nullptr };
        size_t* pStateMask = { nullptr };
    }FSM_DESC;
private:
    CFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CFSM(const CFSM& rhs);
    virtual ~CFSM() = default;

public:
    void Change_State(size_t iStateMask);
    HRESULT Update_State(_float fTimeDelta);

    CState* Get_PrevState() { return m_pPreviousState; }
    size_t  Get_PrevStateMask() { return m_iPreviousStateMask; }

    inline void		Enable_State(size_t stateMask) { *m_pStateMask |= stateMask; }
    inline void		Set_State(size_t initialStateMask) { *m_pStateMask = initialStateMask; }
    inline void		Disable_State(size_t stateMask) { *m_pStateMask &= ~stateMask; }
    
    inline _bool	IsEnable(size_t stateMask) const { return (*m_pStateMask & stateMask) != 0; }
    inline _bool	IsEnable_Previous(size_t stateMask) const { return (m_iPreviousStateMask & stateMask) != 0; }


    HRESULT Bind_States(FSM_DESC& Desc);
private:
    unordered_map<size_t, class CState*>*   m_pStates = { nullptr };
    size_t*                                 m_pStateMask = { nullptr };

    size_t                                  m_iPreviousStateMask = { 0 };

    CState*                                 m_pCurrentState = { nullptr };
    CState*                                 m_pPreviousState = { nullptr };

private:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;

public:
    static CFSM* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CFSM* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
    virtual void Free() override;
#ifdef _DEBUG
    void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
