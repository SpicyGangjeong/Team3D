#include "Component.h"

NS_BEGIN(FSMSTATE)
enum ESTATE
{
    IDLE,
    WALK,WALK_FWD, WALK_BWD,
    JOG,
    SPRINT,
    JUMP,
    DODGE,
    ATTACK,
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

private:
    vector<CState*> m_States;
    CState* m_pCurrent = { nullptr };

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
