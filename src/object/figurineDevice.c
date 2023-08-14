/**
 * @file figurineDevice.c
 * @ingroup Objects
 *
 * @brief Figurine Device object
 */
#define NENT_DEPRECATED
#include "figurineMenu.h"
#include "fileselect.h"
#include "functions.h"
#include "item.h"
#include "kinstone.h"
#include "message.h"
#include "object.h"
#include "screen.h"

typedef struct {
    /*0x00*/ Entity base;
    /*0x68*/ u8 unk_68[0x10];
    /*0x78*/ u16 unk_78;
    /*0x7a*/ u8 unk_7a;
    /*0x7b*/ u8 unk_7b;
#ifdef EU
    /*0x7c*/ u8 unk_80;
    /*0x7d*/ u8 unk_7c;
    /*0x7e*/ u8 unk_7d;
    /*0x7f*/ u8 unk_7e[2];
#else
    /*0x7c*/ u8 unk_7c;
    /*0x7d*/ u8 unk_7d;
    /*0x7e*/ u8 unk_7e[2];
    /*0x80*/ u8 unk_80;
#endif
    /*0x81*/ u8 unk_81;
#ifdef EU
    /*0x82*/ u8 unk_82;
    /*0x83*/ u8 unk_83;
#else
    /*0x82*/ s8 unk_82;
    /*0x83*/ s8 unk_83;
#endif
} FigurineDeviceEntity;

extern void ModShells(s32);
extern u8 gUnk_020227F0;

void sub_0808804C(FigurineDeviceEntity*);
void sub_08087F58(FigurineDeviceEntity*);
void sub_08088328(FigurineDeviceEntity*);
void sub_0808826C(FigurineDeviceEntity*);
void sub_080882A8(FigurineDeviceEntity*);
void sub_080880D8(FigurineDeviceEntity*);
void FigurineDevice_ChangeShellAmount(FigurineDeviceEntity*, s32);
void FigurineDevice_PlayErrorSound(FigurineDeviceEntity*);
bool32 sub_08088160(FigurineDeviceEntity*, s32);
void sub_08088424(FigurineDeviceEntity*);
void FigurineDevice_Init(FigurineDeviceEntity*);
void FigurineDevice_Action1(FigurineDeviceEntity*);
void FigurineDevice_Action2(FigurineDeviceEntity*);
void FigurineDevice_Action3(FigurineDeviceEntity*);
void FigurineDevice_Action4(FigurineDeviceEntity*);

void FigurineDevice(FigurineDeviceEntity* this) {

    static void (*const FigurineDevice_Actions[])(FigurineDeviceEntity*) = {
        FigurineDevice_Init,    FigurineDevice_Action1, FigurineDevice_Action2,
        FigurineDevice_Action3, FigurineDevice_Action4,
    };

    FigurineDevice_Actions[super->action](this);
}

void FigurineDevice_Init(FigurineDeviceEntity* this) {
    Entity* entity;

    super->action = super->type + 1;
    switch (super->type) {
        case 0:
            if (CheckLocalFlag(SHOP07_TANA)) {
                this->unk_7a = 1;
                AddInteractableCheckableObject(super);
            } else {
                this->unk_7a = 0;
            }
            super->spriteRendering.b3 = 3;
            super->spritePriority.b0 = 7;
            this->unk_78 = COORD_TO_TILE(super);
            sub_08087F58(this);
            break;
        case 1:
            entity = CreateObject(FIGURINE_DEVICE, 3, 0);
            if (entity != NULL) {
                entity->parent = super;
                super->child = entity;
            }
            this->unk_7a = 0;
            super->spriteRendering.b3 = 3;
            super->spritePriority.b0 = 7;
            this->unk_78 = COORD_TO_TILE(super);
            sub_08087F58(this);
            break;

        case 2:
            this->unk_7a = 0;
            SetDefaultPriority(super, 7);
            InitializeAnimation(super, 1);
            break;
        case 3:
            super->timer = 30;
            super->subtimer = 0;
            this->unk_81 = 1;
            this->unk_7a = 0;
            this->unk_7b = 0;
            this->unk_80 = 0;
            sub_0808804C(this);
            SetDefaultPriority(super, 6);
            break;
    }
}

void FigurineDevice_Action1(FigurineDeviceEntity* this) {
    switch (this->unk_7a) {
        case 0:
            this->unk_7a = 1;
            AddInteractableCheckableObject(super);
            break;
        case 1:
            if (super->interactType != 0) {
                super->interactType = 0;
                ResetPlayerAnimationAndAction();
                if (CheckLocalFlag(SHOP07_TANA)) {
                    this->unk_7a = 2;
                    SetRoomFlag(2);
                    MenuFadeIn(7, 0xff);
                } else {
                    SetRoomFlag(5);
                }
            }
            break;
        case 2:
        default:
            if (CheckRoomFlag(2) == 0) {
                this->unk_7a = 1;
                AddInteractableCheckableObject(super);
            }
            break;
    }
}

void FigurineDevice_Action2(FigurineDeviceEntity* this) {
    Entity* entity;
    const u8* ptr;

    switch (this->unk_7a) {
        case 0:
            if (CheckRoomFlag(0)) {
                if (CheckLocalFlag(SHOP07_TANA)) {
                    this->unk_7a = 1;
                } else {
                    this->unk_7a = 2;
                }
            }
            break;
        case 2:
            SetLocalFlag(SHOP07_TANA);
        case 1:
            this->unk_7a = 0;
            sub_08088328((FigurineDeviceEntity*)super->child);
            ClearRoomFlag(0);
            entity = CreateObject(FIGURINE_DEVICE, 2, 0);
            if (entity != NULL) {
                entity->parent = super;
                entity->subtimer = this->unk_7d;
                ptr = (u8*)gUnk_080FC3E4; // FIXME use struct
                entity->type2 = ptr[(entity->subtimer << 3) + 7];
                PositionRelative(super, entity, 0x80000, 0x70000);
            }
            EnqueueSFX(SFX_111);
            break;
    }
}

void FigurineDevice_Action3(FigurineDeviceEntity* this) {
    static const u8 gUnk_08120AA8[] = {
        0,
        1,
        2,
        4,
    };

    GetNextFrame(super);
    switch (this->unk_7a) {
        case 0:
            if ((super->frame & ANIM_DONE) != 0) {
                this->unk_7a = 1;
                super->timer = 40;
                ChangeObjPalette(super, gUnk_08120AA8[super->type2]);
                InitializeAnimation(super, 2);
                SoundReq(SFX_110);
            }
            break;

        case 1:
            if (((super->frame & ANIM_DONE) != 0) && (--super->timer == 0)) {
                this->unk_7a = 2;
                SetRoomFlag(2);
                MenuFadeIn(7, super->subtimer);
            }
            break;
        case 2:
        default:
            if (!CheckRoomFlag(2)) {
                super->spriteSettings.draw = 0;
#ifdef EU
                ClearRoomFlag(9);
#else
                ClearRoomFlag(4);
#endif
                DeleteThisEntity();
            }
            break;
    }
}

void FigurineDevice_Action4(FigurineDeviceEntity* this) {
    u8 old_81;
    s32 tmp;

    switch (this->unk_7a) {
        case 0:
            if (this->unk_7b == 4) {
                if (!CheckRoomFlag(1)) {
                    return;
                }
                this->unk_7a = 1;
                this->unk_81 = 1;
                ClearRoomFlag(1);
                sub_0808826C(this);
                sub_080882A8(this);
            } else {
                sub_080880D8(this);
            }
            break;
        case 1:
            if (super->timer != 0) {
                super->timer--;
            }
            if ((gInput.newKeys & A_BUTTON) != 0) {
                SoundReq(SFX_TEXTBOX_SELECT);
                this->unk_7a = 2;
                super->timer = 60;
                sub_08050384();
                return;
            }
            old_81 = this->unk_81;
#ifndef EU
            if ((gInput.heldKeys & R_BUTTON) != 0) {
                tmp = 10;
            } else {
                tmp = 1;
            }
#ifdef JP
            switch (gInput.unk4) {
#else
            switch (gInput.unk4 & 0xfffffeff) {
#endif
                case DPAD_UP:
                    FigurineDevice_ChangeShellAmount(this, tmp);
                    break;
                case DPAD_DOWN:
                    FigurineDevice_ChangeShellAmount(this, -tmp);
                    break;
            }
            if (old_81 != this->unk_81) {
                sub_080882A8(this);
            }
#else
            switch (gInput.unk4) {
                case DPAD_UP:
                    FigurineDevice_ChangeShellAmount(this, 1);
                    break;
                case DPAD_DOWN:
                    FigurineDevice_ChangeShellAmount(this, -1);
                    break;
            }
            if (old_81 != this->unk_81) {
                sub_080882A8(this);
            }
#endif
            break;
        case 2:
            this->unk_7a = 0;
            this->unk_7b = 4;
            SetRoomFlag(3);
            MessageFromTarget(TEXT_INDEX(TEXT_CARLOV, 0x1a));
#ifndef EU
            gMessage.textWindowPosX = 1;
            gMessage.textWindowPosY = 0xc;
#endif
            gMessage.rupees = this->unk_81;
            break;
    }
}

void sub_08087F58(FigurineDeviceEntity* this) {
    switch (super->type) {
        case 0:
        case 1:
            SetTile(0x4022, this->unk_78 - 1, 1);
            SetTile(0x4022, this->unk_78, 1);
            SetTile(0x4022, this->unk_78 + 1, 1);
            break;
    }
}

void FigurineDevice_ChangeShellAmount(FigurineDeviceEntity* this, s32 shellDifference) {
#ifdef EU
    u32 newAmount2;
    u32 newAmount;

    newAmount = this->unk_83 + shellDifference;
    if (CheckLocalFlag(SHOP07_COMPLETE)) {
        FigurineDevice_PlayErrorSound(this);
        return;
    }

    if (shellDifference < 0) {
        if (newAmount < this->unk_82) {
            if (this->unk_83 != this->unk_82) {
                this->unk_83 = this->unk_82;
                this->unk_81 = 1;
                SoundReq(SFX_TEXTBOX_CHOICE);
            } else {
                FigurineDevice_PlayErrorSound(this);
            }
        } else {
            this->unk_83 = newAmount;
            this->unk_81 += shellDifference;
            SoundReq(SFX_TEXTBOX_CHOICE);
        }
        return;
    }
    newAmount2 = this->unk_81 + shellDifference;
    if (newAmount2 > (s32)gSave.stats.shells) {
        if (gSave.stats.shells != this->unk_81) {
            newAmount2 = gSave.stats.shells;
            shellDifference = (gSave.stats.shells - this->unk_81);
            newAmount = this->unk_83 + shellDifference;
        } else {
            FigurineDevice_PlayErrorSound(this);
            return;
        }
    } else if (newAmount > 100) {
        if (this->unk_83 == 100) {
            FigurineDevice_PlayErrorSound(this);
            return;
        } else {
            newAmount = 100;
            shellDifference = (newAmount - this->unk_83);
            newAmount2 = this->unk_81 + shellDifference;
        }
    }

#else
    s32 newAmount2;
    s32 newAmount;
    s32 prevAmount, prevAmount2;

    if (CheckLocalFlag(SHOP07_COMPLETE)) {
        FigurineDevice_PlayErrorSound(this);
        return;
    }

    // This could probably be done without prevAmount and prevAmount2
    prevAmount = this->unk_83;
    newAmount = prevAmount + shellDifference;
    if (shellDifference < 0) {
        if (newAmount < this->unk_82) {
            if (this->unk_83 != this->unk_82) {
                this->unk_83 = this->unk_82;
                this->unk_81 = 1;
                SoundReq(SFX_TEXTBOX_CHOICE);
            } else {
                FigurineDevice_PlayErrorSound(this);
            }
        } else {
            this->unk_83 = newAmount;
            this->unk_81 += shellDifference;
            SoundReq(SFX_TEXTBOX_CHOICE);
        }
        return;
    }
    prevAmount2 = this->unk_81;
    newAmount2 = prevAmount2 + shellDifference;
    if (newAmount2 > gSave.stats.shells) {
        if (gSave.stats.shells != this->unk_81) {
            newAmount2 = gSave.stats.shells;
            shellDifference = (gSave.stats.shells - this->unk_81);
            newAmount = prevAmount + shellDifference;
#ifdef JP
            if (newAmount > 100) {
                newAmount = 100;
                shellDifference = (newAmount - prevAmount);
                newAmount2 = prevAmount2 + shellDifference;
            }
#endif
        } else {
            FigurineDevice_PlayErrorSound(this);
            return;
        }
    }
#ifdef JP
    else if (newAmount > 100) {
#else
    if (newAmount > 100) {
#endif
        if (this->unk_83 == 100) {
            FigurineDevice_PlayErrorSound(this);
            return;
        } else {
            newAmount = 100;
            shellDifference = (newAmount - prevAmount);
            newAmount2 = prevAmount2 + shellDifference;
        }
    }
#endif

    this->unk_83 = newAmount;
    this->unk_81 = newAmount2;
    SoundReq(SFX_TEXTBOX_CHOICE);
}

void FigurineDevice_PlayErrorSound(FigurineDeviceEntity* this) {
    if (super->timer == 0) {
        super->timer = 20;
        SoundReq(SFX_MENU_ERROR);
    }
}

void sub_0808804C(FigurineDeviceEntity* this) {
    u8 result = 0;
    if (gSave.saw_staffroll) {
        result = 9;
    } else if (CheckGlobalFlag(LV5_CLEAR)) {
        result = 7;
    } else if (CheckLocalFlagByBank(FLAG_BANK_9, LV5_31_CAP_0)) {
        result = 6;
    } else if (CheckLocalFlagByBank(FLAG_BANK_3, OUBO_KAKERA)) {
        result = 5;
    } else if (CheckGlobalFlag(LV4_CLEAR)) {
        result = 4;
    } else if (CheckGlobalFlag(LV3_CLEAR)) {
        result = 3;
    } else if (CheckLocalFlagByBank(FLAG_BANK_1, SOUGEN_08_TORITSUKI)) {
        result = 2;
    } else if (CheckGlobalFlag(LV2_CLEAR)) {
        result = 1;
    }
    this->unk_7c = result;
}

void sub_080880D8(FigurineDeviceEntity* this) {
    static const u8 gUnk_08120AAC[] = {
        1, 37, 68, 100, 137, 0, 0, 0,
    };

    const u8* ptr = &gUnk_08120AAC[this->unk_7b];
    u32 it = ptr[0];
    while (it < ptr[1]) {
        if (sub_08088160(this, it)) {
            this->unk_80++;
        }
        it++;
    }
    if (++this->unk_7b == 4) {
        if (gSave.available_figurines != this->unk_80) {
            if (gSave.available_figurines != 0) {
                SetRoomFlag(6);
            }
            gSave.available_figurines = this->unk_80;
        }
        if (CheckLocalFlag(SHOP07_COMPLETE) && (this->unk_80 != gSave.stats.figurineCount)) {
            ClearLocalFlag(SHOP07_COMPLETE);
        }
    }
}

NONMATCH("asm/non_matching/figurineDevice/sub_08088160.inc",
         bool32 sub_08088160(FigurineDeviceEntity* this, s32 param_2)) {
    u8 bVar1;
    bool32 condition;
    u32 uVar3;
    bool32 result;
    const struct_080FC3E4* ptr;
    u32 tmp;

    ptr = &gUnk_080FC3E4[param_2];
    result = FALSE;
    if (this->unk_7c >= ptr->unk_6) {
        result = TRUE;
    } else {
        switch (ptr->unk_6) {
            case 0x8:
            case 0x40:
                if (CheckLocalFlagByBank(ptr->bank, ptr->flag)) {
                    result = TRUE;
                }
                return result;
                break;
            case 0x10:
                if (CheckKinstoneFused(gUnk_080FC3E4[param_2].flag)) {
                    result = TRUE;
                }
                return result;
                break;
            default:
                return result;
            case 0x20:
                switch (gUnk_080FC3E4[param_2].flag) {
                    case 0:
                        if (CheckKinstoneFused(KINSTONE_20) || CheckKinstoneFused(KINSTONE_10) ||
                            CheckKinstoneFused(KINSTONE_19)) {
                            result = TRUE;
                        }
                        break;
                    case 1:
                        if ((u8)this->unk_7c >= 5 && CheckKinstoneFused(KINSTONE_28)) {
                            result = TRUE;
                        }

                        break;
                    case 2:
                        if (CheckKinstoneFused(KINSTONE_54) || CheckKinstoneFused(KINSTONE_56) ||
                            CheckKinstoneFused(KINSTONE_3D)) {
                            result = TRUE;
                        }
                        break;
                    case 3:
                        if (CheckKinstoneFused(KINSTONE_3B) || CheckKinstoneFused(KINSTONE_4A) ||
                            CheckKinstoneFused(KINSTONE_D)) {
                            result = TRUE;
                        }
                        break;
                    case 4:
                        if (CheckKinstoneFused(KINSTONE_49) || CheckKinstoneFused(KINSTONE_55) ||
                            CheckKinstoneFused(KINSTONE_3C)) {
                            result = TRUE;
                        }
                        break;
                    case 5:
                        if (this->unk_7c >= 2 && CheckGlobalFlag(MACHI_MACHIHOKORI)) {
                            result = TRUE;
                        }
                    default:
                        return result;
                }
                break;
        }
    }
    return result;
}
END_NONMATCH

void sub_0808826C(FigurineDeviceEntity* this) {
    s32 tmp = 0x64;
    tmp *= ((this->unk_80 - gSave.stats.figurineCount));
    tmp = tmp / this->unk_80;
    if (tmp == 0 && !CheckLocalFlag(SHOP07_COMPLETE)) {
        tmp = 1;
    }
    this->unk_83 = tmp;
    this->unk_82 = this->unk_83;
}

void sub_080882A8(FigurineDeviceEntity* this) {
#ifdef EU
    static const u8 gUnk_08120AB4[] = {
        206, 79, 3, 2, 0, 208, 0, 6, 0, 13, 0, 2, 0, 0, 0, 0, 128, 240, 104, 56, 2, 0, 1, 0,
    };
    static const u8 gUnk_08120ACC[] = {
        206, 79, 3, 2, 0, 208, 0, 6, 0, 13, 0, 2, 0, 0, 0, 0, 128, 240, 208, 59, 2, 0, 1, 0,
    };
#else
    static const u8 gUnk_08120AB4[] = {
        206, 79, 3, 2, 0, 208, 0, 6, 0, 13, 0, 2, 0, 0, 0, 0, 128, 240, 104, 136, 2, 0, 1, 0,
    };
    static const u8 gUnk_08120ACC[] = {
        206, 79, 3, 2, 0, 208, 0, 6, 0, 13, 0, 2, 0, 0, 0, 0, 128, 240, 208, 139, 2, 0, 1, 0,
    };
#endif
    static const u16 gUnk_08120AE4[] = { TEXT_INDEX(TEXT_CARLOV, 0x18), TEXT_INDEX(TEXT_CARLOV, 0x19) };
    u8* ptr;
    sub_08050384();
    sub_08057044(this->unk_81, gUnk_020227E8, 0x202020);
    sub_08057044(this->unk_83, &gUnk_020227E8[1], 0x202020);
    ptr = (u8*)0x02000000;
    if (ptr[7] == 0) {
        ShowTextBox(gUnk_08120AE4[super->type2], (Font*)&gUnk_08120AB4); // TODO convert data
    } else {
        ShowTextBox(gUnk_08120AE4[super->type2], (Font*)&gUnk_08120ACC); // TODO convert data
    }
    gScreen.bg0.updated = 1;
}

void sub_08088328(FigurineDeviceEntity* this) {
    u32 uVar2;
    u32 uVar3;
    u32 uVar5;
    u32 uVar6;

    do {
        uVar2 = Random();
        uVar2 &= 0x7f;
    } while (uVar2 >= 100);
    ModShells(-this->unk_81);
    uVar3 = Random();
    uVar6 = (uVar3 & 0x7f) + 1;
    uVar5 = uVar6;
    sub_08088424(this);
    if (uVar2 < this->unk_83) {
        uVar2 = FALSE;
        do {
            if (uVar2)
                break;
            if (uVar6 > 0x88) {
                uVar6 = 1;
            }
            if (sub_08088160(this, uVar6) && ReadBit(gSave.figurines, uVar6) == 0) {
                uVar2 = TRUE;
            } else {
                uVar6++;
            }
        } while (uVar5 != uVar6);
    } else {
        uVar2 = TRUE;
        do {
            if (!uVar2)
                break;
            if (uVar6 > 0x88) {
                uVar6 = 1;
            }
            if (sub_08088160(this, uVar6) && ReadBit(gSave.figurines, uVar6) != 0) {
                uVar2 = FALSE;
            } else {
                uVar6++;
            }
        } while (uVar5 != uVar6);
    }
    if (uVar2) {
        gSave.stats.figurineCount++;
        if (gSave.stats.figurineCount != this->unk_80) {
            SetRoomFlag(7);
        } else {
            SetLocalFlag(SHOP07_COMPLETE);
            SetRoomFlag(8);
        }
    }
    this->unk_7d = uVar6;
    ((FigurineDeviceEntity*)super->parent)->unk_7d = uVar6;
}

void sub_08088424(FigurineDeviceEntity* this) {
    if (gSave.stats.figurineCount < 50) {
        if (this->unk_83 < 0x0f) {
            this->unk_83 = 0x0f;
        }
    } else if (gSave.stats.figurineCount < 80) {
        if (this->unk_83 < 0xc) {
            this->unk_83 = 0xc;
        }
    } else if (gSave.stats.figurineCount < 110) {
        if (this->unk_83 < 9) {
            this->unk_83 = 9;
        }
    } else {
        if (this->unk_83 < 6) {
            this->unk_83 = 6;
        }
    }
}

void sub_08088478(void) {
    u32 messageIndex;
    bool32 set0x10 = FALSE;
    if (!CheckRoomFlag(8)) {
        if (!CheckRoomFlag(7)) {
            messageIndex = TEXT_INDEX(TEXT_CARLOV, 0x22);
            set0x10 = TRUE;
        } else {
            messageIndex = TEXT_INDEX(TEXT_CARLOV, 0x25);
        }
    } else {
        switch (gSave.stats.figurineCount) {
            case 136:
                gSave.stats._hasAllFigurines = 0xff;
                messageIndex = TEXT_INDEX(TEXT_CARLOV, 0x29);
                break;
            case 130:
                if (gSave.saw_staffroll) {
                    messageIndex = TEXT_INDEX(TEXT_CARLOV, 0x27);
                } else {
                    messageIndex = TEXT_INDEX(TEXT_CARLOV, 0x28);
                }
                break;
            default:
                messageIndex = TEXT_INDEX(TEXT_CARLOV, 0x27);
        }
    }
    MessageFromTarget(messageIndex);
    gMessage.textWindowPosX = 1;
    gMessage.textWindowPosY = 0xc;
    if (set0x10) {
        gMessage.rupees = 5;
    }
#ifndef EU
    gPlayerEntity.animationState = 6;
#endif
}

void sub_08088504(void) {
    u32 index;
    switch (gSave.stats.figurineCount) {
        case 136:
            index = TEXT_INDEX(TEXT_CARLOV, 0x2f);
            break;
        case 130:
            index = TEXT_INDEX(TEXT_CARLOV, 0x14);
            break;
        default:
            index = TEXT_INDEX(TEXT_CARLOV, 0x11);
            break;
    }
    MessageFromTarget(index);
    gMessage.textWindowPosX = 1;
    gMessage.textWindowPosY = 0xc;
}

void sub_08088544(void) {
    u32 index;
    if (gSave.stats.figurineCount != 130) {
        index = TEXT_INDEX(TEXT_CARLOV, 0xe);
    } else {
        index = TEXT_INDEX(TEXT_CARLOV, 0x15);
    }
    MessageFromTarget(index);
    gMessage.textWindowPosX = 1;
    gMessage.textWindowPosY = 0xc;
}

void sub_08088574(void) {
    u32 index;
#ifdef EU
    if (CheckRoomFlag(0xa)) {
#else
    if (CheckRoomFlag(9)) {
#endif
        if (CheckLocalFlag(SHOP07_COMPLETE)) {
            index = TEXT_INDEX(TEXT_CARLOV, 0x13);
        } else {
            index = TEXT_INDEX(TEXT_CARLOV, 0x0c);
        }
    } else {
        index = TEXT_INDEX(TEXT_CARLOV, 0x0b);
    }
    MessageFromTarget(index);
    gMessage.textWindowPosX = 1;
    gMessage.textWindowPosY = 0xc;
}

void sub_080885B0(void) {
    if (!CheckRoomFlag(8)) {
        if (!CheckRoomFlag(7)) {
            ModRupees(5);
            MessageFromTarget(TEXT_INDEX(TEXT_CARLOV, 0x26));
            gMessage.textWindowPosX = 1;
            gMessage.textWindowPosY = 0xc;
        }
    } else if (gSave.stats._hasAllFigurines != 0) {
        // GOT ALL THEM FIGURINES (:
        gSave.stats.hasAllFigurines = 1;
        CreateItemEntity(ITEM_QST_CARLOV_MEDAL, 0, 0);
        SetGlobalFlag(FIGURE_ALLCOMP);
    }
    ClearRoomFlag(8);
    ClearRoomFlag(7);
}

#ifndef EU
void sub_0808861C(FigurineDeviceEntity* this, ScriptExecutionContext* context) {
    context->condition = CheckPlayerInRegion(0xa8, 0x54, 0xc, 8);
#ifdef JP
    if ((gPlayerEntity.animationState != 0)) {
#else
    if ((gPlayerEntity.animationState != 0) || (gPlayerEntity.z.HALF.HI != 0)) {
#endif
        context->condition = 0;
    }
    gActiveScriptInfo.flags |= 1;
}

#if !defined(JP)
void sub_08088658(FigurineDeviceEntity* this, ScriptExecutionContext* context) {
    context->condition = CheckPlayerInRegion(0x78, 0x78, 0x10, 8);
    if (gPlayerEntity.z.HALF.HI != 0) {
        context->condition = 0;
    }
    gActiveScriptInfo.flags |= 1;
}
#endif

#endif
