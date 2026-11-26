#include "../EntityState.func.hpp"

#include "OpenSHC/Map/Navigation/DirectionAlgorithmState.func.hpp"
#include "OpenSHC/Map/Navigation/PathFindingState.func.hpp"
#include "OpenSHC/Game/GameMode.hpp"
#include "OpenSHC/Map/Entities/EntityType.hpp"
#include "OpenSHC/Map/Units/States/UnitState.hpp"
#include "OpenSHC/Map/Units/States/UnitStateShort.hpp"

#include "OpenSHC/Globals/DAT_BuildingsState.hpp"
#include "OpenSHC/Globals/DAT_DirectionAlgorithmState.hpp"
#include "OpenSHC/Globals/DAT_GameSynchronyState.hpp"
#include "OpenSHC/Globals/DAT_PathFindingState.hpp"
#include "OpenSHC/Globals/DAT_TileMapState.hpp"
#include "OpenSHC/Globals/DAT_UnitsState.hpp"

namespace OpenSHC {
namespace Map {
    namespace Entities {

        using Game::GameMode;
        using Map::Entities::EntityType;
        using Map::Units::States::UnitState;
        using Map::Units::States::UnitStateShort;

        // FUNCTION: STRONGHOLDCRUSADER 0x004016E0
        int EntityState::findNearestDiseaseCloudForHealer(int unitID)
        {
            int nearestEntityIndex = 0;
            int minDistance = 10000;
            
            // Using int for iterator to match ebx loop counter
            int i = 1; 

            unsigned short unitArea = (unsigned short)DAT_TileMapState::instance.PathConnectionLayer[DAT_UnitsState::instance.units[unitID].tile];

            if (i < this->maxEntityCount) {
                do {
                    Entity& entity = this->entityArray[i];

                    if (entity.logicalState != 2) goto LAB_0040184d;
                    if (entity.entityType != Map::Entities::ET_COW_POISON_CLOUD) goto LAB_0040184d;
                    if (entity.unknownAnimationFrameRelated >= 1001) goto LAB_0040184d;
                    if (entity.unknownDistanceRelatedValue != 0) goto LAB_0040184d;

                    short healerUnitID = entity.unitID_healer;
                    if (healerUnitID != 0) {
                        if (entity.unitUID == DAT_UnitsState::instance.units[healerUnitID].uid) {
                            UnitStateShort unitState = DAT_UnitsState::instance.units[healerUnitID].state.generic;
                            if (unitState == Map::Units::States::US_AIM_WEAPONUnk || unitState == Map::Units::States::US_FIRE_WEAPONUnk) {
                                goto LAB_0040184d;
                            }
                        }
                        entity.unitID_healer = 0;
                    }

                    unsigned short entityArea = (unsigned short)DAT_TileMapState::instance.PathConnectionLayer[entity.tile];
                    
                    if (entityArea != unitArea) {
                        int canNavigate = MACRO_CALL_MEMBER(Map::Navigation::PathFindingState_Func::calculateCanPlayerUnitsNavigateToAreaFromArea, 
                                            DAT_PathFindingState::ptr)((int)DAT_UnitsState::instance.units[unitID].owner, 
                                            (unsigned int)unitArea, (unsigned int)entityArea, 0);
                        if (canNavigate == 0) goto LAB_0040184d;
                    }

                    if (DAT_GameSynchronyState::instance.currentGameMode != Game::GM_SOLITARY) {
                        int workplaceID = DAT_UnitsState::instance.units[unitID].workplaceBuildingID_1;
                        if (workplaceID != 0) {
                            MACRO_CALL_MEMBER(Map::Navigation::DirectionAlgorithmState_Func::setAxisBasedDistanceResult, 
                                            DAT_DirectionAlgorithmState::ptr)(
                                            (int)DAT_BuildingsState::instance.buildings[workplaceID].x,
                                            (int)DAT_BuildingsState::instance.buildings[workplaceID].y,
                                            (int)entity.xPosition,
                                            (int)entity.yPosition);
                            if (DAT_DirectionAlgorithmState::instance.distanceHigh >= 31) goto LAB_0040184d;
                        }
                    }

                    MACRO_CALL_MEMBER(Map::Navigation::DirectionAlgorithmState_Func::setAxisBasedDistanceResult, 
                            DAT_DirectionAlgorithmState::ptr)(
                            (int)DAT_UnitsState::instance.units[unitID].x,
                            (int)DAT_UnitsState::instance.units[unitID].y,
                            (int)entity.xPosition,
                            (int)entity.yPosition);
                    
                    if (DAT_DirectionAlgorithmState::instance.distanceHigh < minDistance) {
                        minDistance = DAT_DirectionAlgorithmState::instance.distanceHigh;
                        nearestEntityIndex = i;
                    }

                LAB_0040184d:
                    i++;
                } while (i < this->maxEntityCount);
            }
            return nearestEntityIndex;
        }

    } // namespace Entities
} // namespace Map
} // namespace OpenSHC