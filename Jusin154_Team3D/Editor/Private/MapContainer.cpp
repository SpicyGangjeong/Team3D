#include "pch.h"
#include "MapContainer.h"
#include "PartObject.h"

#include "MapObject_LOD.h"
#include "MapObject_Static.h"

CMapContainer::CMapContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CContainerObject(pDevice, pContext)
{
}

CMapContainer::CMapContainer(const CMapContainer& rhs)
	:CContainerObject(rhs)
{
}

HRESULT CMapContainer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapContainer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CMapContainer::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg))) {
		return E_FAIL;
	}

	return S_OK;
}

void CMapContainer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CMapContainer::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CMapContainer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CMapContainer::Render()
{
	return S_OK;
}

HRESULT CMapContainer::Save_PartObjects(tinyxml2::XMLDocument& Doc, tinyxml2::XMLElement* Container)
{
	for (auto& piar : m_PartObjects)
	{
		CMapObject* pMapObject = dynamic_cast<CMapObject*>(piar.second);

		if (nullptr == pMapObject)
			return E_FAIL;

		// <PartObject>
		tinyxml2::XMLElement* PartObject = Doc.NewElement("PartObject");
		PartObject->SetAttribute("Lod_Level", pMapObject->Get_LodLevel());
		Container->InsertEndChild(PartObject);

		for (_uint i = 0; i < pMapObject->Get_LodLevel() + 1; ++i)
		{
			// <PrototypeTag>≈ÿΩ∫∆Æ</PrototypeTag>
			tinyxml2::XMLElement* proto = Doc.NewElement("PrototypeTag");
			proto->SetText(CMyTools::ToString(pMapObject->Get_PrototypeTag(i)).c_str());
			PartObject->InsertEndChild(proto);
		}

		// <Position x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* PositionElem = Doc.NewElement("Position");

		_float3 vPosition = {};
		XMStoreFloat3(&vPosition, pMapObject->Get_Component<CTransform>()->Get_State(STATE::POSITION));

		PositionElem->SetAttribute("x", vPosition.x);
		PositionElem->SetAttribute("y", vPosition.y);
		PositionElem->SetAttribute("z", vPosition.z);
		PartObject->InsertEndChild(PositionElem);

		// <Scale x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* ScaleElem = Doc.NewElement("Scale");

		_float3 pScale = pMapObject->Get_Component<CTransform>()->Get_Scale();
		ScaleElem->SetAttribute("x", pScale.x);
		ScaleElem->SetAttribute("y", pScale.y);
		ScaleElem->SetAttribute("z", pScale.z);
		PartObject->InsertEndChild(ScaleElem);

		// <Rotation x="0.0" y="0.0" z="0.0"/>
		tinyxml2::XMLElement* RotationElem = Doc.NewElement("Rotation");

		_float3 vRotation = {};
		XMStoreFloat3(&vRotation, pMapObject->Get_Component<CTransform>()->Get_RollPitchYawVector());

		RotationElem->SetAttribute("x", vRotation.x);
		RotationElem->SetAttribute("y", vRotation.y);
		RotationElem->SetAttribute("z", vRotation.z);
		PartObject->InsertEndChild(RotationElem);

	}

	return S_OK;
}

void CMapContainer::Free()
{
	__super::Free();
}
