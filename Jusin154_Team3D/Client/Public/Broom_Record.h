#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CBroom_Record final : public CElementObject
{
private:
	CBroom_Record(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroom_Record(const CBroom_Record& rhs);
	virtual ~CBroom_Record() = default;

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

private:
	void Set_Font();
	void Set_Best_Record(_float fTime);

public:
	void Rece_Results();
	void Finish(_float fTime);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_wstring m_fFont[5];
	_float2 m_fFontPos[6];
	_vector m_fFontColor[2];

	_float	m_fBaseTime{};

	_bool	m_bNewScore = { false };
public:
	static CBroom_Record* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
