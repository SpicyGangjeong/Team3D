#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CQuest_Status final : public CElementObject
{
private:
	CQuest_Status(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Status(const CQuest_Status& rhs);
	virtual ~CQuest_Status() = default;

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
	void Compute_UI(_int Index);
	void Set_Status(_int Index, _float m_PosY, _wstring Text);

	void Set_Click(_bool bClick);

private:
	void Hover();

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom1 = { nullptr };
	CTexture* m_pDiffuse_TextureCom2 = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_wstring m_pName;
	_int m_iIndex{};
	_float4 m_vUV{};
	_float4 m_vImageposi{};
	_bool m_bClick = { false };
public:
	static CQuest_Status* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
