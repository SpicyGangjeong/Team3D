#pragma once

#include "Client_Define.h"
#include "PartObject.h"

NS_BEGIN(Client)

class CRaceRing final : public CGameObject
{
private:
	CRaceRing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRaceRing(const CRaceRing& Prototype);
	virtual ~CRaceRing() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	class CBroomRaceManager* m_pBroomRaceManager = { nullptr };



private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRaceRing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
