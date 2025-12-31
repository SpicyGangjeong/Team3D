#pragma once

#include "Client_Define.h"
#include "GameObject.h"

NS_BEGIN(Client)

class CBroom_TargetGate final : public CGameObject
{
private:
	CBroom_TargetGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroom_TargetGate(const CBroom_TargetGate& rhs);
	virtual ~CBroom_TargetGate() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;


private:
	HRESULT	Bind_ShaderResources();
	HRESULT	Ready_Components();
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Set_TargetPosition(_vector TargetPos);

private:
	void Visibie();

private:
	CInfoInstance* m_pInfoInstance = { nullptr };
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_bool	m_vActive = { false };
	_wstring m_Distance;

	_vector m_vFontPosition{};

	_vector m_vPlayerPos{};
	_vector m_vTargetPos{};

	_float m_fAlpha{};
	_vector m_vPosition{};

	_float3 m_fSize{};
	_float2 m_fViewPort{};

	_float m_fFontSize{};

public:
	static CBroom_TargetGate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // DEBUG
};

NS_END
