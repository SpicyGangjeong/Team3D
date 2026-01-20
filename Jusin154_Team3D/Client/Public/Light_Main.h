#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CLight_Main final : public CGameObject
{
private:
	CLight_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLight_Main(const CLight_Main& Prototype);
	virtual ~CLight_Main() = default;

public:
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Capture_PreShadow();

private:
	CLight*		m_pLightCom = { nullptr };


private:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	HRESULT Ready_Components(void* pArg);
	HRESULT Bind_ShaderResources();
	_matrix Get_OffCenterProjMatrix(_fmatrix ViewMatrix, vector<_float3>& WorldPositions);
	void	Add_PreShadowRenderGroup(CLayer* pLayer);

public:
	static CLight_Main* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr) override;
	virtual void Free() override;
#ifdef _DEBUG
	virtual void Describe_Entity() override;
#endif // _DEBUG

};

NS_END
