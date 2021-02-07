#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "SDL_FontCache.h"
#include "behaviour_tree.h"
#include "global.h"
#include "path_finder.h"
#include "toolbox/toolbox.h"

typedef enum
{
  TRANSFORM,
  VISUAL,
  MOTION,
  ANIMATOR,
  CONTROLLER,
  EQUIPMENT,
  HEALTH,
  HITBOX,
  HEAL_BAR,
  LIFE_SPAN,
  DESTROYED_TAG,
  WEAPON_SKILL_SWING,
  WEAPON_SKILL_CHARGE,
  WEAPON_SKILL_THUST,
  WEAPON_SKILL_THUNDER_STORM,
  DAMAGE_OUTPUT,
  WEAPON_ATTRIBUTES,
  DROP,
  PICKUPABLE_ATTRIBUTES,
  INVULNERABLE,
  INPUT_BLOCKER,
  ENEMY_TAG,
  PLAYER_TAG,
  CAMERA_TARGET_TAG,
  ENDABLE_TILE_COLLISION_TAG,
  CHARACTER_ANIMATOR_TAG,
  PROJECTILE_ATTRIBUTES,
  DOOR_TAG,
  BRAIN,
  DESTINATION,
  PATH,
  FOLLOWING_TARGET,
  SPOT,
  LEVEL_SWITCHER,
  NAME,
  INTERACTABLE,
  TEXT,
  DIALOGUE,
  MERCHANT,
  CHEST,
  ATTUNEMENT_SLOT,
  CASTABLE,
  MANA_POOL,
  DOOR_ATTRIBUTES,
  HOLDER,
  ATTACK_MASK,
  REMOVE_IF_OFFSCREEN,
  ATTACKER,
  SELF_DESTRUCTION,
  NUM_COMPONENTS
} ComponentId;

typedef struct
{
  Vec2  vel;
  Vec2  acc;
  float max_speed;
  float max_force;
  float friction;
  float vz;
  float gravity_scale;
  float bounching;
} Motion;

typedef struct
{
  Vec2   prev_position;
  Vec2   position;
  double rotation;
  float  z;
  int    hdir;
  BOOL   lock_hdir;
} Transform;

typedef struct
{
  SDL_Point        anchor;
  SDL_RendererFlip flip;
  Sprite           sprite;
  COLOR            color;
  u8               opacity;
} Visual;

typedef struct
{
  Vec2  position;
  float radius;
} Spot;

typedef struct
{
  Vec2 force;
  u16  action;
  BOOL in_action;
} Controller;

typedef struct
{
  ecs_entity_t weapon;
  Vec2         weapon_anchor;
  Vec2         d;
} Equipment;

typedef struct
{
  s32 atk;
  u16 type_id;
  u16 mask;
} WeaponAttributes;

/*Weapon skills*/
typedef struct
{
  CharacterAction on_action;
  u16             timer;
  u16             state;
  u32             range;
} wpskl_Swing;

typedef struct
{
  u16  on_action;
  u16  timer;
  BOOL is_active;
} wpskl_Charge;

typedef struct
{
  u16 on_action;
  u16 timer;
  u8  state;
} wpskl_Thust;

typedef struct
{
  u16 on_action;
  u16 remaining;
  u16 interval;
  u16 total;
  u16 timer;
} wpskl_ThunderStorm;

typedef struct
{
  int  atk;
  int  type;
  BOOL impact;
  Vec2 force;
} DamageOutput;

typedef struct
{
  int w;
  int h;
  int cx;
  int cy;
} DamageArea;

/*entity tags*/
typedef int PlayerTag;

typedef int EnemyTag;

typedef int WeaponTag;

typedef int CameraTargetTag;

typedef int EnableTileCollisionTag;

typedef int NpcTag;

typedef int DestroyedTag;

typedef int CharacterAnimatorTag;

typedef struct ProjectileAttributes
{
  int   damage;
  u16   effect;
  u16   damage_type;
  BOOL  destroy_when_hit;
  BOOL  impact;
  Vec2  impact_force;
  float impact_force_z;
  u16   impact_time;
  u16   sfx;
  u16   shooter;
} ProjectileAttributes;

typedef enum DoorState
{
  DOOR_STATE_CLOSE,
  DOOR_STATE_OPEN,
} DoorState;

#define DOOR_NO_REQUIRED_KEY -1
#define DOOR_REQUIRED_SPECIAL_CONDITION -2
typedef struct DoorInfo
{
  s16 required_key;
  u16 state;
} DoorAttributes;

typedef struct PickupableAttributes
{
  u16 id;
  u16 sfx;
  u16 coins;
} PickupableAttributes;
typedef struct
{
  s32 hit_points;
  s32 max_hit_points;
} Health;

typedef struct
{
  u32        current_anim;
  u32        elapsed;
  u32        cnt;
  Animation* anims;
} Animator;

typedef struct
{
  Vec2 size;
  Vec2 anchor;
  int  proxy_id;
  u16  mask_bits;
  u16  category;
} HitBox;

typedef struct
{
  int       len; // in pixel
  SDL_Color color;
  SDL_Color border;
  SDL_Point anchor;
} HealthBar;

typedef struct LifeSpan
{
  s32 remaining;
} LifeSpan;

typedef struct
{
  u16 item1;
  u16 item2;
  u8  change1;
  u8  change2;
} Drop;

typedef struct
{
  int remaining;
} InputBlocker;

typedef struct
{
  int remaining;
} Invulnerable;

typedef struct
{
  BTNode* root;
} Brain;

typedef Vec2 Destination;

#define PATH_MAX_NUM_NODES 100
typedef struct Path
{
  Vec2i nodes[PATH_MAX_NUM_NODES];
  int   num_nodes;
  int   curr;
} Path;

typedef struct
{
  ecs_entity_t entity;
  float        radius;
} FollowingTarget;

#define LEVEL_SWITCHER_MAX_LEVEL_NAME_LEN 255
#define LEVEL_SWITCHER_MAX_DEST_LEN 255
typedef struct
{
  char      level[LEVEL_SWITCHER_MAX_LEVEL_NAME_LEN + 1];
  char      dest[LEVEL_SWITCHER_MAX_DEST_LEN + 1];
  Direction exit_direction;
} LevelSwitcher;

#define NAME_MAX_LEN 15
typedef struct
{
  char value[NAME_MAX_LEN + 1];
} Name;

#define TEXT_MAX_LEN 63
typedef struct
{
  char      value[TEXT_MAX_LEN + 1];
  FC_Effect effect;
  FONT*     font;
} Text;

#define INTERACTABLE_MAX_COMMANDS 5
typedef struct Interatcable
{
  const char* commands[INTERACTABLE_MAX_COMMANDS];
  int         num_commands;
} Interactable;

typedef struct
{
  int conversation_id;
} Dialogue;

#define MERCHANT_MAX_PAYLOADS 100
#define MERCHANT_INIFINTE -1
typedef struct Merchant
{
  ItemPayload payloads[MERCHANT_MAX_PAYLOADS];
  u32         num_payloads;
} Merchant;

#define CHEST_MAX_ITEMS 5
typedef struct
{
  Item items[CHEST_MAX_ITEMS];
  u8   num_items;
} Chest;

typedef struct
{
  u16 mana_points;
  u16 max_mana_points;
} ManaPool;

typedef struct
{
  u16 spell_id;
} AttunementSlot;

typedef struct
{
  int timer;
} Castable;

typedef int RemoveIfOffScreen;

typedef struct Holder
{
  ecs_entity_t value;
} Holder;

typedef struct AttackMask
{
  u16 value;
} AttackMask;

typedef struct Attacker
{
  ecs_entity_t value;
} Attacker;

typedef struct SelfDestruction
{
  u16          range;
  ecs_entity_t target;
  u16          emiting_timer;
  u16          emiting_interval;
} SelfDestruction;

typedef struct Flags 
{
  u32 value;
} Flags;

Animation*
animation_init(Animation* anim, SDL_Texture* tex, u32 x, u32 y, u32 row, u32 col, u32 sw, u32 sh);

void animation_fini(Animation* anim);

Animator* animator_init(Animator* animator, const Animation* anims, u32 cnt);
void      animator_fini(Animator* animator);

void visual_set_anchor_to_center(Visual* v);
void brain_fini(Brain* ai_agent);
void hitbox_init(HitBox* h);
void level_switcher_init(LevelSwitcher* sw, const char* level, const char* dest);
void name_init(Name* name, const char* value);
void text_init(Text* text, const char* value, FONT* font, COLOR color);
void visual_init(Visual* v);

#endif // COMPONENTS_H
