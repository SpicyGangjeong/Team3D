#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CBroom_Scoreboard final : public CElementObject
{
private:
	CBroom_Scoreboard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBroom_Scoreboard(const CBroom_Scoreboard& rhs);
	virtual ~CBroom_Scoreboard() = default;

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
	void Set_Setting();

public:
	void Set_MaxRing(_int iMaxRing);
	void Set_CurrentRing();

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	_vector m_fColor[4];
	_wstring Score[8];
	_float2 m_fFontPosition[8];

	_int m_iCurrentRing{};
	_float m_fCurreentRingFontSize{};

public:
	static CBroom_Scoreboard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
