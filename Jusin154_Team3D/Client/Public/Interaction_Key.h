#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CInteraction_Key final : public CGameObject
{
private:
	CInteraction_Key(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInteraction_Key(const CInteraction_Key& Prototype);
	virtual ~CInteraction_Key() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_Active(_bool bAtcive);

private:
	_float4 ComputeUV(_tchar Alphabet);
	void ReLockOnTarget();

private:
	const _float4x4* m_pSocketMatrices = {  };
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CInfoInstance* m_pInfoInstance = { nullptr };

	LOCKON_INFO m_LockOnInteractionObject = { nullptr };

	_bool m_bActive = { false };
	_bool m_bAtteckTrigger = { false };

	_float3 m_fSize{};
	_float m_fAlpha{};
	_float m_fTime{};
	_float4 m_vUV{};
	_float2 m_fViewPort{};
private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CInteraction_Key* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, CGameObject* pOwner = nullptr)override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};
NS_END
