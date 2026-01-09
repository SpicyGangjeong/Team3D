#pragma once

#include "Client_Define.h"
#include "PanelObject.h"

NS_BEGIN(Engine)
class CTexture;
class CShader;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CDialogue_Panel final : public CPanelObject
{
private:

private:
	CDialogue_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDialogue_Panel(const CDialogue_Panel& rhs);
	virtual ~CDialogue_Panel() = default;

public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual _vector Get_WorldPostion() override;

private:
	virtual HRESULT	Bind_ShaderResources() override;
	virtual HRESULT	Ready_Components(void* pArg) override;
	virtual HRESULT Ready_Element(void* pArg) override;
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

private:
	void Setting_Choice(DIALOGUECHOICEINFO Choice);
	void Choice_Slot(CHOICEINFO Choice);

	void ReSet_Choice();

private:
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	CTexture* m_pDiffuse_TextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };

	CGameObject* m_pDialogue_Choice = { nullptr };

	vector<CGameObject*> m_Choices;
	deque<CGameObject*> m_CurrentChoices;

	CInfoInstance* m_pInfoInstance = { nullptr };

	_int m_iChoiceCount{};

	_bool		m_bBroomRace = { false };

public:
	static CDialogue_Panel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg, class CGameObject* pOwner) override;
	virtual void Free() override;
#ifdef _DEBUG
	void Describe_Entity() override;
#endif // _DEBUG
};

NS_END
