#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CGameObject_Manager final : public CBase
{
private:
	CGameObject_Manager();
	virtual ~CGameObject_Manager() = default;

public:
	HRESULT Add_GameObject_ToLayer(_uint iLayerLevelIndex, const _wstring& strLayerTag, class CGameObject* pInstance);

	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

	void Clear(_uint iLevelIndex);
	void Clear_DeadObj();
	class CLayer* Get_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);
	void Clear_Layer(_uint iLevelIndex, const _wstring& strLayerTag);

private:
	class CGameInstance*				m_pGameInstance = { nullptr };
	map<const _wstring, class CLayer*>* m_pLayers = { nullptr };
	_uint								m_iNumLevels = {};

private:
	HRESULT Initialize(_uint iNumLevels);
	class CLayer* Find_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag);

public:
	static CGameObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;

#ifdef _DEBUG
	void Describe_Entity();
#endif // _DEBUG

	

};

NS_END
