#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;
public:
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);

	HRESULT Add_GameObject(class CGameObject* _pGameObject);
	void	Clear_DeadObj();
	HRESULT Clear_Layer();

public:
	template<typename T>
	T* Get_Object()
	{
		for (auto& iter : m_ObjectList)
		{
			if (typeid(*iter) == typeid(T))
			{
				return dynamic_cast<T*>(iter);
			}
		}

		return nullptr;
	}

	const list<class CGameObject*>* Get_Objects() const { return &m_ObjectList; }
private:
	class CGameInstance* m_pGameInstance = { nullptr };
	list<class CGameObject*> m_ObjectList;
private:
	HRESULT Initialize();

public:
	static CLayer* Create();
	virtual void Free() override;


#ifdef _DEBUG
	void Describe_Entity();
#endif // _DEBUG

};

NS_END

