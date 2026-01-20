#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Client)


class CBroom_Trophy final : public CElementObject
{
private:
	CBroom_Trophy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroom_Trophy(const CBroom_Trophy& rhs);
	virtual ~CBroom_Trophy() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Set_Start();
	void Score(_bool bScore);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };


public:
	static CBroom_Trophy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // DEBUG
};

NS_END
