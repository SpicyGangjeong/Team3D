#pragma once

#include "Client_Define.h"
#include "ElementObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CQuest_Info_Header final : public CElementObject
{
private:
	CQuest_Info_Header(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CQuest_Info_Header(const CQuest_Info_Header& rhs);
	virtual ~CQuest_Info_Header() = default;

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
	void Set_Hover(void* pArg);
	void Set_QuestType(_int Index);

private:
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CInfoInstance* m_pInfoInstance = { nullptr };

	_int m_iColor = true;
	_int m_iQuest_Index{};
	_int m_iCurrentQuest{};

	_int m_iQuestSlot{};
	_int m_iCurrentQeustSlot{};

public:
	static CQuest_Info_Header* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
