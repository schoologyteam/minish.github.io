/**
 * @file dampe.c
 * @ingroup NPCs
 *
 * @brief Dampe NPC
 */
#define NENT_DEPRECATED
#include "entity.h"
#include "flags.h"
#include "item.h"
#include "message.h"
#include "npc.h"
#include "player.h"
#include "room.h"

typedef struct {
    /*0x00*/ Entity base;
    /*0x68*/ u8 fusionOffer;
} DampeEntity;

void Dampe(Entity* this) {
    switch (this->action) {
        case 0:
            this->action = 1;
            this->spriteSettings.draw = 1;
            SetDefaultPriority(this, PRIO_MESSAGE);
            InitScriptForNPC(this);
            return;
        case 1:
            if (this->interactType == INTERACTION_FUSE) {
                this->action = 2;
                this->interactType = INTERACTION_NONE;
                InitAnimationForceUpdate(this,
                                         GetAnimationStateForDirection4(GetFacingDirection(this, &gPlayerEntity)) + 4);
                InitializeNPCFusion(this);
            } else {
                ExecuteScriptAndHandleAnimation(this, NULL);
            }
            return;
        case 2:
            UpdateAnimationSingleFrame(this);
            if (UpdateFuseInteraction(this)) {
                this->action = 1;
            }
    }
}

void Dampe_MakeInteractable(DampeEntity* this) {
    this->fusionOffer = GetFusionToOffer(super);
    AddInteractableWhenBigFuser(super, this->fusionOffer);
}

void Dampe_Fusion(Entity* this) {
    if (this->action == 0) {
        this->action++;
        this->spriteSettings.draw = 1;
        InitAnimationForceUpdate(this, 2);
    } else {
        UpdateAnimationSingleFrame(this);
    }
}

void sub_0806BE84(Entity* this, ScriptExecutionContext* context) {
    static const u16 messageIndices[] = {
        TEXT_INDEX(TEXT_DAMPE, 0x1),
        TEXT_INDEX(TEXT_DAMPE, 0x8),
        TEXT_INDEX(TEXT_DAMPE, 0x4),
    };
    u32 msgIndex;

    context->condition = 0;
    msgIndex = 1;
    if (!CheckLocalFlag(DANPEI_TALK1ST)) {
        msgIndex = 0;
        SetLocalFlag(DANPEI_TALK1ST);
        context->condition = 1;
    }
    // Graveyard key
    if (GetInventoryValue(ITEM_QST_GRAVEYARD_KEY) >= 2) {
        msgIndex = 2;
    }
    MessageNoOverlap(messageIndices[msgIndex], this);
}

void sub_0806BEC8(Entity* this, ScriptExecutionContext* context) {
    static const u16 messageIndices[] = {
        TEXT_INDEX(TEXT_DAMPE, 0x2),
        TEXT_INDEX(TEXT_DAMPE, 0x3),
        TEXT_INDEX(TEXT_DAMPE, 0x4),
    };
    u32 hasGraveyardKey;
    u32 msgIndex;

    msgIndex = 0;
    context->condition = 0;
    hasGraveyardKey = GetInventoryValue(ITEM_QST_GRAVEYARD_KEY);
    if (hasGraveyardKey == 1) {
        msgIndex = 1;
        context->condition = 1;
    } else if (hasGraveyardKey >= 2) {
        msgIndex = 2;
    }

    MessageNoOverlap(messageIndices[msgIndex], this);
}

void sub_0806BEFC(void) {
    SetTileType(0x17E, 0x58E, 1);
    SetTileType(0x17F, 0x58F, 1);
    SetTileType(0x180, 0x5CE, 1);
    SetTileType(0x181, 0x5CF, 1);
}

void sub_0806BF44(Entity* this, ScriptExecutionContext* context) {
    context->condition = 0;
    if (((this->x.HALF.HI - gRoomControls.scroll_x) + 0x10U < 0x110) &&
        ((this->y.HALF.HI - gRoomControls.scroll_y) + 0x18U < 0xD0)) {
        context->condition = 1;
    }
}
