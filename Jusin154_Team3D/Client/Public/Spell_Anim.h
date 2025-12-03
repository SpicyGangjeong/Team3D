#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CSpell_Anim final : public CElementObject
{
private:
	CSpell_Anim(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpell_Anim(const CSpell_Anim& rhs);
	virtual ~CSpell_Anim() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

public:
	virtual void Set_FadeIn() override;

private:
	void Change_Image(_int SpellID);
	virtual void Set_SkillType(_int eType) override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	CInfoInstance* m_pInfoInstance = { nullptr };

	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CTexture* m_pDesendo_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };



	_int	m_iTotalFrames{};			// 전체 프레임 수
	_float	m_fFrameTime{};				// 한 프레임당 시간(초)
	_int	m_iCurrentFrame{};			// 현재 보여줄 프레임

	_int	m_iPerSpell{};
	_bool	m_bAnim_Start = { false };
public:
	static CSpell_Anim* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
