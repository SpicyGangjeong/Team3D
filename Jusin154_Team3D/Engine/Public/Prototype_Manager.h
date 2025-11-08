#pragma once
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Cell.h"
#include "Shader.h"
#include "Navigation.h"
#include "Texture.h"
#include "Cell.h"
#include "Model.h"


NS_BEGIN(Engine)
class CGameInstance;

class CPrototype_Manager final : public CBase
{
private:
	CPrototype_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CPrototype_Manager() = default;

public:
	HRESULT Clear_Resource(_uint _iLevelindex);

public:
	CComponent* Clone_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag, void* pArg, class CGameObject* pOwner);
	CComponent* Find_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag);
	HRESULT Add_Asset_Prototype(_uint iTargetLevel, const _wstring& strPrototypeTag, CComponent* pPrototype);

	template<typename T>
	HRESULT Add_Prototype(_uint _iLevelNumber, T* pPrototype)
	{
		assert(m_iLevelNumber > _iLevelNumber);
		if (nullptr == pPrototype) {
			return E_FAIL;
		}

		if (nullptr != Find_Prototype<T>(_iLevelNumber)) {
			return E_FAIL;
		}

		m_pPrototypes[_iLevelNumber].push_back(pPrototype);

		return S_OK;
	}
	template<typename T>
	T* Clone_Prototype(_uint _iLevelNumber, void* _pArg, class CGameObject* pOwner = nullptr)
	{
		assert(m_iLevelNumber > _iLevelNumber);

		T* pObject = { nullptr };
		pObject = CPrototype_Manager::Find_Prototype<T>(_iLevelNumber);

		if (pObject == nullptr) {
			_string out = "Failed to Cloned : No_Prototype : ";
			out.append(typeid(T).name());
			MessageBox(NULL, CMyTools::ToWstring(out).c_str(), L"System Message", MB_OK);

			return nullptr;
		}


		return dynamic_cast<T*>(pObject->Clone(_pArg, pOwner));
	}
private:
	CGameInstance* m_pGameInstance = { nullptr };
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	vector<class CBase*>* m_pPrototypes = { nullptr };
	map<_wstring, CComponent*>* m_pAssets = { nullptr };

	_uint								m_iLevelNumber = { };

private:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Ready_StaticComponents();
	HRESULT Ready_EngineAssets();

	template<typename T>
	T* Find_Prototype(_uint _iLevelNumber)
	{
		T* pObject = { nullptr };

		for (auto& pBase : m_pPrototypes[_iLevelNumber]) {
			_int iIterId = (_int)typeid(*pBase).hash_code();
			_int iTemplateID = (_int)typeid(T).hash_code();
			if (typeid(*pBase).hash_code() == typeid(T).hash_code()) {
				return dynamic_cast<T*>(pBase);
			}
		}
		return pObject;
	}

public:
	static CPrototype_Manager* Create(_uint iNumLevels, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END
