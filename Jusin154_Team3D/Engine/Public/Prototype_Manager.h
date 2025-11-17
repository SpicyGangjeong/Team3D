#pragma once
#include "Transform.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Cell.h"
#include "Shader.h"
#include "Navigation.h"
#include "RigidBody_Dynamic.h"
#include "RigidBody_Static.h"
#include "Texture.h"
#include "Cell.h"
#include "Model.h"
#include "Instance_Model.h"
#include "Character_Controller.h"
#include "Light.h"
#include "FSM.h"
#include "State.h"
#include "VIBuffer_UI_Instance.h"


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
#ifdef _DEBUG
public:
	//특정 타입의 컴포넌트들의 이름을 통한 콤보박스 , 이미지는 이미지 클립을
	//만들고 선택할 시에 내 ppOut에 클론하는 기능을 담당

	template<typename T>
	_string Asset_Description(_uint iLevel, const _char* pComponentName, Engine::CComponent** ppOut, void* pDesc, class CGameObject* pOwner = nullptr, _wstring wstrGroupName = L"")
	{
		vector<const _char*> pComponentNames = {};
		vector<_string> strNames = {};

		for (auto& Pair : m_pAssets[iLevel])
		{

			if (typeid(*Pair.second) == typeid(T)) // 같은 타입의 컴포넌트 였다면
			{
				strNames.push_back(CMyTools::ToString(Pair.first));

				pComponentNames.push_back(strNames.back().c_str());

			}
		}

		if (typeid(CTexture) == typeid(T)) // 텍스쳐일 경우 이미지로 띄우기
		{
			_int iIndex = {};

			for (auto& strName : strNames)
			{
				_wstring wstrComponentName = CMyTools::ToWstring(strName);

				auto    iter = m_pAssets[iLevel].find(wstrComponentName);

				if (iter == m_pAssets[iLevel].end()) {
					continue;
				}

				CTexture* pTexture = dynamic_cast<CTexture*>(iter->second);

				if (pTexture == nullptr)
					continue;

				if (pTexture->Compare_GroupName(wstrGroupName) == false) //  그룹네임을 비교해서 같은 것만 띄울 수 있도록 한다 , Default시 L""
					continue;

				if (GUI::ImageButton(strName.c_str(), pTexture->Get_SRV(0), ImVec2(48, 48)))
				{
					if (*ppOut != nullptr)
						SAFE_RELEASE(*ppOut);


					*ppOut = iter->second->Clone(pDesc, pOwner);

					return strName;
				}

				if (ImGui::IsItemHovered()) {

					ImGui::BeginTooltip();           // 툴팁 창 시작

					ImGui::TextUnformatted(CMyTools::ToString(wstrComponentName).c_str());    // 텍스트 출력 (Text 대신 Unformatted 추천)

					ImGui::EndTooltip();             // 툴팁 창 종료
				};

				if ((iIndex++ + 1) % 4 != 0)
					GUI::SameLine(); // 같은 줄에 이어서

			}

			return "";
		}

		static _int s_iCurrentItem = 0;
		static ImGuiTextFilter filter;

		// 다른 어셋일 경우 콤보로 띄우기

		if(GUI::BeginCombo(pComponentName, (s_iCurrentItem >= 0 && s_iCurrentItem < pComponentNames.size())
			? pComponentNames[s_iCurrentItem] : "NONE"))
		{
			if (ImGui::IsWindowAppearing())
			{
				ImGui::SetKeyboardFocusHere();
				filter.Clear();
			}


			filter.Draw("##Filter", -FLT_MIN);


			for (_uint i = 0; i < pComponentNames.size(); i++)
			{
				bool is_selected = (s_iCurrentItem == i);

				if (filter.PassFilter(pComponentNames[i]))
				{

					if (GUI::Selectable(pComponentNames[i], is_selected))
					{
						s_iCurrentItem = i;
					}

					if (is_selected)
					{
				
						ImGui::SetItemDefaultFocus();
					}

				}

			}

			GUI::EndCombo();
		}

		if (GUI::Button("Select"))
		{
			auto    iter = m_pAssets[iLevel].find(CMyTools::ToWstring(pComponentNames[s_iCurrentItem]));

			if (iter == m_pAssets[iLevel].end()) {


				GUI::EndCombo();

				return "";
			}

			if (*ppOut != nullptr)
				SAFE_RELEASE(*ppOut);

			*ppOut = iter->second->Clone(pDesc, pOwner);

			return pComponentNames[s_iCurrentItem];
		}

		return "";
	}
#endif

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
