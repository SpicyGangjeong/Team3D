#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CSpellLearn_ChaserPointer final : public CElementObject
{
private:
	CSpellLearn_ChaserPointer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpellLearn_ChaserPointer(const CSpellLearn_ChaserPointer& rhs);
	virtual ~CSpellLearn_ChaserPointer() = default;

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
	void Set_SpellLearn(_int Index);
	void Set_Move();
private:
	void Line(_float fTime);
	void SpeedUp();
	void Clear();

private:
	CInfoInstance* m_pInfoInstance = { nullptr };
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };


	_bool	m_bMoveStart = { false };

	_int	m_iLineIndex{};
	_int	m_iCurrentLine{};
	vector<_vector> m_MoveLine;

public:
	static CSpellLearn_ChaserPointer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
