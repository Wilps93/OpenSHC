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

            int unitArea
                = (int)(short)
                      DAT_TileMapState::instance.PathConnectionLayer[DAT_UnitsState::instance.units[unitID].tile];

            for (int i = 1; i < this->maxEntityCount; ++i) {
                Entity& entity = this->entityArray[i];

                if (entity.logicalState != 2)
                    continue;
                if (entity.entityType != Map::Entities::ET_COW_POISON_CLOUD)
                    continue;
                if (entity.unknownAnimationFrameRelated > 1000)
                    continue;
                if (entity.unknownDistanceRelatedValue != 0)
                    continue;

                short healerUnitID = entity.unitID_healer;
                if (healerUnitID != 0) {
                    if (entity.unitUID == DAT_UnitsState::instance.units[healerUnitID].uid) {
                        UnitStateShort unitState = DAT_UnitsState::instance.units[healerUnitID].state.generic;
                        if (unitState == Map::Units::States::US_AIM_WEAPONUnk
                            || unitState == Map::Units::States::US_FIRE_WEAPONUnk) {
                            continue;
                        }
                    }
                    entity.unitID_healer = 0;
                }

                int entityArea = (int)(short)DAT_TileMapState::instance.PathConnectionLayer[entity.tile];

                if (entityArea != unitArea) {
                    int canNavigate = MACRO_CALL_MEMBER(
                        Map::Navigation::PathFindingState_Func::calculateCanPlayerUnitsNavigateToAreaFromArea,
                        DAT_PathFindingState::ptr)((int)(short)DAT_UnitsState::instance.units[unitID].owner,
                        (unsigned int)unitArea, (unsigned int)entityArea, 0);
                    if (canNavigate == 0)
                        continue;
                }

                if (DAT_GameSynchronyState::instance.currentGameMode != Game::GM_SOLITARY) {
                    int workplaceID = DAT_UnitsState::instance.units[unitID].workplaceBuildingID_1;
                    if (workplaceID != 0) {
                        MACRO_CALL_MEMBER(Map::Navigation::DirectionAlgorithmState_Func::setAxisBasedDistanceResult,
                            DAT_DirectionAlgorithmState::ptr)(
                            (int)(short)DAT_BuildingsState::instance.buildings[workplaceID].x,
                            (int)(short)DAT_BuildingsState::instance.buildings[workplaceID].y,
                            (int)(short)entity.xPosition, (int)(short)entity.yPosition);
                        if (DAT_DirectionAlgorithmState::instance.distanceHigh > 30)
                            continue;
                    }
                }

                MACRO_CALL_MEMBER(Map::Navigation::DirectionAlgorithmState_Func::setAxisBasedDistanceResult,
                    DAT_DirectionAlgorithmState::ptr)((int)(short)DAT_UnitsState::instance.units[unitID].x,
                    (int)(short)DAT_UnitsState::instance.units[unitID].y, (int)(short)entity.xPosition,
                    (int)(short)entity.yPosition);

                if (DAT_DirectionAlgorithmState::instance.distanceHigh < minDistance) {
                    nearestEntityIndex = i;
                    minDistance = DAT_DirectionAlgorithmState::instance.distanceHigh;
                }
            }
            return nearestEntityIndex;
        }

    } // namespace Entities
} // namespace Map
} // namespace OpenSHC
