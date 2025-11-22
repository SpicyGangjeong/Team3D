#include "Component.h"

NS_BEGIN(FSMSTATE)
enum ESTATE
{
    ROOT,
    IDLE,IDLE_TURN,
    MOVE, WALK, FWD, BWD, JOG, SPRINT,
    JUMP,
    LAND,
    DODGE,
    COMBAT, LIGHT_ATTACK, CAST, SKILL,SKILL2,
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

    //inline void		Enable_State(size_t stateMask) { *m_pStateMask |= stateMask; }
    //inline void		Set_State(size_t initialStateMask) { *m_pStateMask = initialStateMask; }
    //inline void		Disable_State(size_t stateMask) { *m_pStateMask &= ~stateMask; }
    
    inline _bool	IsEnable(size_t stateMask) const { return (*m_pStateMask & stateMask) != 0; }
    inline _bool	IsEnable_Previous(size_t stateMask) const { return (m_iPreviousStateMask & stateMask) != 0; }

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
    virtual void Describe_Entity() override;
};

NS_END
