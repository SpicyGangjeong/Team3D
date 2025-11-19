#include "Component.h"

NS_BEGIN(FSMSTATE)
enum ESTATE
{
    IDLE,IDLE_TURN,
    MOVE, WALK, WALK_FWD, WALK_BWD, JOG, SPRINT,
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
private:
    CFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CFSM(const CFSM& rhs);
    virtual ~CFSM() = default;

public:
    void Add_State(_uint iIndex, CState* state);
    void Change_State(_uint iIndex);
    void Update(_float fTimeDelta);

    _uint Get_CurrState();
    _uint Get_PrevState();
    void Set_Parent(FSMSTATE::ESTATE Child, FSMSTATE::ESTATE Parent);
private:
    vector<CState*>     m_States;
    CState*             m_pCurrent = { nullptr };
    CState*             m_pPrevious = { nullptr };

private:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;


public:
    static CFSM* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CComponent* Clone(void* pArg, class CGameObject* pOwner = nullptr) override;
    virtual void Free() override;
    virtual void Describe_Entity() override;
};

NS_END
