/**
 * @file fireballGuy.c
 * @ingroup Enemies
 *
 * @brief Fireball Guy enemy
 */

#include "enemy.h"
#include "functions.h"

typedef struct {
    s8 h, v;
} PACKED PosOffset;

extern void sub_08045524(Entity*);
void sub_08045178(Entity*, Entity*, int, int);

extern void (*const FireballGuy_Functions[])(Entity*);
extern void (*const gUnk_080D1800[])(Entity*);
extern u8 gUnk_080D180C[4]; // Entity count per form
extern PosOffset gUnk_080D1810[4];

void FireballGuy(Entity* this) {
    EnemyFunctionHandler(this, FireballGuy_Functions);
}

void FireballGuy_OnTick(Entity* this) {
    gUnk_080D1800[this->action](this);
}

void FireballGuy_OnCollision(Entity* this) {
    if (this->health && this->cutsceneBeh.HALF.LO != this->health) {
        this->action = 2;
    } else {
        EnemyFunctionHandlerAfterCollision(this, FireballGuy_Functions);
    }
}

void FireballGuy_OnGrabbed(void) {
}

void sub_080453E8(Entity* this) {
    this->action = 1;
    this->timer = 0;
    this->spriteSettings.draw = 1;
    this->speed = 0x80;
    this->cutsceneBeh.HALF.LO = this->health;
    this->collisionFlags |= 0x10;
    sub_0804A720(this);
    InitializeAnimation(this, 0);
    sub_08045524(this);
}

void sub_08045430(Entity* this) {
    ProcessMovement2(this);
    GetNextFrame(this);
    if (GravityUpdate(this, Q_8_8(24.0)) == 0)
        sub_08045524(this);
}

/* Split FireballGuy into new ones */
void sub_08045454(Entity* this) {
    Entity* entities[4];
    Entity* ent;
    s32 count, i;
    PosOffset* off;
    u32 tmp;

    /* Can we create enough new entities? */
    count = gUnk_080D180C[this->type];
    if (MAX_ENTITIES + 1 - count <= gEntCount)
        return;

    /* Create 2-5 new MiniFireballGuy */
    for (i = 0; i < count; i++)
        entities[i] = CreateEnemy(MINI_FIREBALL_GUY, this->type);

    off = gUnk_080D1810;
    for (i = 0; i < count; i++) {
        ent = entities[i];
        ent->child = entities[(i + 1) % count];
        ent->parent = entities[(i + count - 1) % count];
        tmp = 0;
        ent->type2 = 1;
        ent->z.HALF.HI = tmp;
        ent->iframes = -0x10;

        /* Set MiniFireballGuy offset relative to killed slime. */
        sub_08045178(this, ent, off->h, off->v);
        off++;
    }

    ent = CreateFx(this, FX_DEATH, 0);
    if (ent != NULL)
        CopyPosition(this, ent);

    DeleteEntity(this);
}

u32 sub_0804A024(Entity*, u32, u32);

void sub_08045524(Entity* this) {
    u32 tmp, tmp1, tmp2;

    this->zVelocity = Q_16_16(1.75);
    tmp = sub_0804A024(this, 1, 8);
    if (tmp != 0xff && (Random() & 3) == 0) {
        this->timer = Random() & 3;
        this->direction = DirectionRound(tmp);
    } else {
        if (this->timer) {
            this->timer--;
            return;
        }
        this->timer = Random() & 3;
        if (sub_08049FA0(this) == 0 && (Random() & 3)) {
            tmp1 = sub_08049EE4(this);
            tmp2 = Random() & 8;
            tmp2 += 0xfc;
            this->direction = DirectionRound(tmp1 + tmp2);
        } else {
            this->direction = DirectionRound(Random());
        }
    }
}
