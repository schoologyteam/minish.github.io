/**
 * @file houseSignManager.c
 * @ingroup Managers
 *
 * @brief Keeps track in its bitfield of whether 0x10 x 0x10 rects at certain positions are on the screen.
 *
 * Spawns HOUSE_SIGN objects that check this and unsets the value in the bitfield.
 * Creates the signs on the houses in hyrule town.
 */
#include "manager/houseSignManager.h"

#include "area.h"
#include "asm.h"
#include "object.h"
#include "object/houseSign.h"
#include "room.h"

typedef struct {
    u16 x;
    u16 y;
    u8 type;
    u8 frameIndex;
    u8 collisionLayer;
    u8 _padding;
} SpawnData;

void HouseSignManager_Main(HouseSignManager* this) {
    static const SpawnData gUnk_081084C8[] = {
        { 0x48, 0x90, 0x0, 0x0, 0x2, 0x0 },   { 0x138, 0x30, 0x1, 0x1, 0x2, 0x0 },
        { 0x168, 0x90, 0x2, 0x2, 0x2, 0x0 },  { 0x2c8, 0x80, 0x3, 0x3, 0x2, 0x0 },
        { 0x388, 0x240, 0x4, 0x4, 0x2, 0x0 }, { 0x158, 0x1ed, 0x5, 0x5, 0x2, 0x0 },
        { 0x15a, 0x228, 0x6, 0x6, 0x2, 0x0 }, { 0x158, 0x26b, 0x7, 0x7, 0x2, 0x0 },
        { 0x198, 0x2b1, 0x8, 0x8, 0x2, 0x0 }, { 0x258, 0x2ac, 0x9, 0x9, 0x2, 0x0 },
        { 0x298, 0x21a, 0xa, 0xa, 0x2, 0x0 }, { 0x298, 0x1cb, 0xb, 0xb, 0x2, 0x0 },
        { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }
    };
    static const SpawnData gUnk_08108530[] = {
        { 0x8, 0x30, 0x1, 0x1, 0x2, 0x0 },    { 0x38, 0x90, 0x2, 0x2, 0x2, 0x0 },
        { 0x198, 0x80, 0x3, 0x2, 0x2, 0x0 },  { 0x28, 0x1ed, 0x5, 0x5, 0x2, 0x0 },
        { 0x2a, 0x228, 0x6, 0x6, 0x2, 0x0 },  { 0x28, 0x26b, 0x7, 0x7, 0x2, 0x0 },
        { 0x68, 0x2b1, 0x8, 0x8, 0x2, 0x0 },  { 0x128, 0x2ac, 0x9, 0x9, 0x2, 0x0 },
        { 0x168, 0x21a, 0xa, 0xa, 0x2, 0x0 }, { 0x168, 0x1cb, 0xb, 0xb, 0x2, 0x0 },
        { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }
    };
    const SpawnData* spawnData;
    u32 type2;

    if (super->action == 0) {
        super->action = 1;
    }
    if (gRoomControls.area == AREA_FESTIVAL_TOWN) {
        spawnData = gUnk_08108530;
    } else {
        spawnData = gUnk_081084C8;
    }
    type2 = 0;
    while ((spawnData->x != 0) && (type2 < 0x20)) {
        u32 bitfieldFlag = 1 << type2;
        if ((((this->bitfield & bitfieldFlag) == 0) &&
             (CheckRectOnScreen(spawnData->x, spawnData->y, 0x10, 0x10) != 0))) {
            HouseSignEntity* object = (HouseSignEntity*)CreateObject(HOUSE_SIGN, spawnData->type, type2);
            if (object != NULL) {
                object->base.frameIndex = spawnData->frameIndex;
                object->base.x.HALF.HI = gRoomControls.origin_x + spawnData->x;
                object->base.y.HALF.HI = gRoomControls.origin_y + spawnData->y;
                object->base.parent = (Entity*)this;
                object->unk_80 = spawnData->x;
                object->unk_82 = spawnData->y;
                object->base.collisionLayer = spawnData->collisionLayer;
                this->bitfield |= bitfieldFlag;
            }
        }
        spawnData++;
        type2++;
    }
}
