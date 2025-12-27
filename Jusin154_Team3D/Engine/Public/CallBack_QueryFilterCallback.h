#pragma once
#include "Engine_Define.h"

class CPhysX_CctQueryFilterCallback final : public PSX::PxQueryFilterCallback
{
public:
    PSX::PxQueryHitType::Enum preFilter(const PSX::PxFilterData& controllerQueryFilterData, const PSX::PxShape* shape, 
        const PSX::PxRigidActor* actor, PSX::PxHitFlags& queryHitFlags) override
    {
        if (actor != nullptr && actor->userData != nullptr)
        {
            PhsXUserData* physxUserData = (PhsXUserData*)actor->userData;

            if (physxUserData->eKind == PHYSX_KIND::BODY_DYNAMIC)
            {
                switch (PXOBJECT(physxUserData->iSubKind))
                {
                case PXOBJECT::LEG:
                    return PSX::PxQueryHitType::eNONE;
                    break;
                case PXOBJECT::SKILL_PROTEGO:
                    return PSX::PxQueryHitType::eNONE;
                    break;
                case PXOBJECT::JOINT_ROUTE:
                    return PSX::PxQueryHitType::eNONE;
                    break;
                case PXOBJECT::JOINT_ANCHOR:
                    return PSX::PxQueryHitType::eNONE;
                    break;
                default:
                    break;
                }
            }
        }

        return PSX::PxQueryHitType::eBLOCK; // 기본은 막는 충돌
    }

    PSX::PxQueryHitType::Enum postFilter(const PSX::PxFilterData& controllerQueryFilterData, const PSX::PxQueryHit& hit, 
        const PSX::PxShape* shape, const PSX::PxRigidActor* actor) override
    {
        return PSX::PxQueryHitType::eBLOCK;
    }
};
