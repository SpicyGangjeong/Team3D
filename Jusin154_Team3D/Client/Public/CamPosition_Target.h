#pragma once
#include "Client_Define.h"
#include "CamPosition.h"

NS_BEGIN(Engine)
class CUnit;
NS_END

NS_BEGIN(Client)

class CCamPosition_Target final : public CCamPosition
{
public:
	typedef struct tagCamPosition_Target : public CCamPosition::CAMERAPOSITION_DESC
	{
	}CAMERAPOSITION_TARGET_DESC;
private:
	CCamPosition_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamPosition_Target(const CCamPosition_Target& rhs);
	virtual ~CCamPosition_Target() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual _vector Get_WorldPostion() override;
	virtual void Set_WorldPostion(_vector vPos);
	virtual _bool IsStalking();
	virtual void Stalking_Target(CUnit* pStalkingTarget, const _float4x4* pTargetSocketMatrix);
	virtual void Stop_Stalking();

private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources() override;

	CUnit* m_pStalkingTarget = { nullptr };
	const _float4x4* m_pTargetSocketMatrix = { nullptr };
public:
	static CCamPosition_Target* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCamPosition_Target* Clone(void* pArg, class CGameObject* pOWner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;

#endif // _DEBUG

};

NS_END
