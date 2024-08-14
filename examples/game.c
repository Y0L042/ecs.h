// Simple Quake-Like in 400 LOC

#include "raylib.h"
#include "ecs.h"

// Define component structures
typedef struct {
    float x, y, z, scale;
} cmp_transform_t;

typedef struct {
    float dx, dy, dz;
} cmp_velocity_t;

typedef struct {
    int health;
} cmp_health_t;

typedef struct {
    BoundingBox bounds; // 3D bounding box
} cmp_static_t;

typedef struct {
    BoundingBox bounds; // 3D bounding box
} cmp_collision_t;

typedef struct {
    Model model;
    BoundingBox bounding_box; // 3D bounding box for occlusion culling
} cmp_renderable_t;

typedef struct {
    Color color;
    float intensity;
    float range;
} cmp_light_t;

void log_error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
}

// Quadtree/Octree node structure
typedef struct OctreeNode {
    BoundingBox bounds;
    struct OctreeNode* children[8];
    int entity_count;
    ent_t entities[MAX_ENTS];
} OctreeNode;

// Quadtree/Octree for spatial partitioning
OctreeNode* CreateOctreeNode(BoundingBox bounds) {
    OctreeNode* node = (OctreeNode*)MemAlloc(sizeof(OctreeNode));
    node->bounds = bounds;
    memset(node->children, 0, sizeof(node->children));
    node->entity_count = 0;
    return node;
}

void InsertEntityInOctree(OctreeNode* node, ent_t entity, BoundingBox bounds) {
    if (node->entity_count < MAX_ENTS) {
        node->entities[node->entity_count++] = entity;
    } else {
        // Split node or insert into children (simplified logic)
    }
}

void GetEntitiesInFrustum(OctreeNode* node, Camera camera, ent_t* visible_entities, int* visible_count) {
    if (IsBoundingBoxInFrustum(node->bounds, camera)) {
        for (int i = 0; i < node->entity_count; ++i) {
            visible_entities[(*visible_count)++] = node->entities[i];
        }
        for (int i = 0; i < 8; ++i) {
            if (node->children[i]) {
                GetEntitiesInFrustum(node->children[i], camera, visible_entities, visible_count);
            }
        }
    }
}

// Frustum culling
bool IsBoundingBoxInFrustum(BoundingBox box, Camera camera) {
    // Perform frustum culling based on the camera's view frustum
    // Simplified placeholder implementation
    return CheckCollisionBoxFrustum(box, GetCameraFrustum(camera));
}

// Systems
void system_move(ecs_t* ecs, ent_t ent, void* data) {
    cmp_transform_t* trans = (cmp_transform_t*)get_cmp(ecs, ent, 0);
    cmp_velocity_t* vel = (cmp_velocity_t*)get_cmp(ecs, ent, 1);

    trans->x += vel->dx;
    trans->y += vel->dy;
    trans->z += vel->dz;
}

void system_world_collision(ecs_t* ecs, ent_t ent, void* data) {
    cmp_collision_t* col = (cmp_collision_t*)get_cmp(ecs, ent, 2);
    cmp_transform_t* trans = (cmp_transform_t*)get_cmp(ecs, ent, 0);
    cmp_velocity_t* vel = (cmp_velocity_t*)get_cmp(ecs, ent, 1);

    // Simplified collision logic: check against other entities
    for (size_t i = 0; i < ecs->active_count; ++i) {
        ent_t other = ecs->active_list[i];
        if (other == ent) continue;

        if (CHECK_BIT(ecs->ent_cmps[other], 2)) {
            cmp_collision_t* other_col = (cmp_collision_t*)get_cmp(ecs, other, 2);

            // Perform collision check (bounding box, etc.)
            if (CheckCollisionBoxes(col->bounds, other_col->bounds)) {
                // Handle collision (e.g., stop movement, reduce health, etc.)
                trans->x -= vel->dx; // Simple collision response
                trans->y -= vel->dy;
                trans->z -= vel->dz;
            }
        }
    }
}

void system_input(ecs_t* ecs, ent_t ent, void* data) {
    cmp_velocity_t* vel = (cmp_velocity_t*)get_cmp(ecs, ent, 1);

    if (IsKeyDown(KEY_W)) vel->dz -= 1.0f;
    if (IsKeyDown(KEY_S)) vel->dz += 1.0f;
    if (IsKeyDown(KEY_A)) vel->dx -= 1.0f;
    if (IsKeyDown(KEY_D)) vel->dx += 1.0f;
}

void system_visibility_check(ecs_t* ecs, ent_t* visible_entities, int* visible_count, Camera* camera) {
    OctreeNode* octree = (OctreeNode*)ecs; // Assuming the octree is stored in ecs for simplicity
    GetEntitiesInFrustum(octree, *camera, visible_entities, visible_count);
}

void system_world_render(ecs_t* ecs, ent_t* visible_entities, int visible_count, Camera* camera) {
    for (int i = 0; i < visible_count; ++i) {
        ent_t ent = visible_entities[i];

        cmp_transform_t* trans = (cmp_transform_t*)get_cmp(ecs, ent, 0);
        cmp_renderable_t* render = (cmp_renderable_t*)get_cmp(ecs, ent, 3);

        // Render the model
        DrawModel(render->model, (Vector3){ trans->x, trans->y, trans->z }, trans->scale, WHITE);
    }
}

void system_lighting(ecs_t* ecs, ent_t ent, void* data) {
    cmp_light_t* light = (cmp_light_t*)get_cmp(ecs, ent, 4);

    // Update lighting in the world, possibly affecting other entities
    // Apply lighting effects based on light properties
}

void system_physics(ecs_t* ecs, ent_t ent, void* data) {
    cmp_velocity_t* vel = (cmp_velocity_t*)get_cmp(ecs, ent, 1);

    // Apply gravity
    vel->dy -= 9.8f * GetFrameTime(); // Gravity acceleration
}

void system_camera_control(ecs_t* ecs, ent_t player, Camera* camera) {
    cmp_transform_t* trans = (cmp_transform_t*)get_cmp(ecs, player, 0);

    camera->position = (Vector3){ trans->x, trans->y + 1.8f, trans->z + 10.0f };
    camera->target = (Vector3){ trans->x, trans->y + 1.8f, trans->z };
}

void system_ai(ecs_t* ecs, ent_t ent, void* data) {
    cmp_transform_t* trans = (cmp_transform_t*)get_cmp(ecs, ent, 0);
    cmp_velocity_t* vel = (cmp_velocity_t*)get_cmp(ecs, ent, 1);

    cmp_transform_t* player_trans = (cmp_transform_t*)get_cmp(ecs, *((ent_t*)data), 0);

    // Simple chase AI
    vel->dx = player_trans->x > trans->x ? 1.0f : -1.0f;
    vel->dz = player_trans->z > trans->z ? 1.0f : -1.0f;
}

void system_sound(ecs_t* ecs, ent_t ent, void* data) {
    if (IsKeyPressed(KEY_SPACE)) {
        PlaySound(*(Sound*)data);
    }
}


void init_world(ecs_t* ecs, OctreeNode* octree) {
    // Create static entities like walls, floors, etc.
    ent_t wall = create_ent(ecs);

    cmp_transform_t wall_transform = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .scale = 1.0f };
    cmp_static_t wall_static = { /* Any specific data for static objects */ };
    cmp_collision_t wall_collision = { /* Collision data */ };
    cmp_renderable_t wall_renderable = { /* Load model or texture */ };

    add_cmp(ecs, wall, 0, &wall_transform, sizeof(wall_transform));
    add_cmp(ecs, wall, 1, &wall_static, sizeof(wall_static));
    add_cmp(ecs, wall, 2, &wall_collision, sizeof(wall_collision));
    add_cmp(ecs, wall, 3, &wall_renderable, sizeof(wall_renderable));

    InsertEntityInOctree(octree, wall, wall_static.bounds);

    // Repeat for other static and dynamic objects
}

typedef struct {
    Model models[MAX_MODELS];
    Texture2D textures[MAX_TEXTURES];
} resource_manager_t;

resource_manager_t resource_manager;

void load_resources() {
    // Load models, textures, etc.
    resource_manager.models[0] = LoadModel("resources/model.obj");
    resource_manager.textures[0] = LoadTexture("resources/texture.png");
}

void unload_resources() {
    // Unload resources
    for (int i = 0; i < MAX_MODELS; ++i) {
        UnloadModel(resource_manager.models[i]);
    }
    for (int i = 0; i < MAX_TEXTURES; ++i) {
        UnloadTexture(resource_manager.textures[i]);
    }
}

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSED,
    GAME_STATE_GAMEOVER
} game_state_t;

game_state_t game_state = GAME_STATE_MENU;

void update_game_state() {
    switch (game_state) {
        case GAME_STATE_MENU:
            // Handle menu logic
            break;
        case GAME_STATE_PLAYING:
            // Handle gameplay logic
            break;
        case GAME_STATE_PAUSED:
            // Handle pause logic
            break;
        case GAME_STATE_GAMEOVER:
            // Handle game over logic
            break;
    }
}

void load_level(const char* level_path, ecs_t* ecs) {
    // Simplified level loading logic: parse the level file and create entities
    FILE* file = fopen(level_path, "r");
    if (!file) {
        log_error("Failed to load level");
        return;
    }

    // Example: load entities from level file
    // fclose(file);
}


int main() {
    InitWindow(800, 600, "Minimalistic FPS Shooter");
    SetTargetFPS(60);

    ecs_t ecs = {0}; // Initialize ECS
    BoundingBox worldBounds = (BoundingBox){{-100.0f, -100.0f, -100.0f}, {100.0f, 100.0f, 100.0f}};
    OctreeNode* octree = CreateOctreeNode(worldBounds); // Define world bounds here
    init_world(&ecs, octree); // Initialize the world state

    // Create a player entity
    ent_t player = create_ent(&ecs);

    // Add components
    cmp_transform_t player_pos = {0.0f, 0.0f, 0.0f, 1.0f};
    cmp_velocity_t player_vel = {0.0f, 0.0f, 0.0f};
    cmp_health_t player_health = {100};

    add_cmp(&ecs, player, 0, &player_pos, sizeof(player_pos));
    add_cmp(&ecs, player, 1, &player_vel, sizeof(player_vel));
    add_cmp(&ecs, player, 2, &player_health, sizeof(player_health));

    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 45.0f;
    camera.type = CAMERA_PERSPECTIVE;

    Sound shootSound = LoadSound("resources/shoot.wav");

    ent_t visible_entities[MAX_ENTS];
    int visible_count = 0;

    while (!WindowShouldClose()) {
        // Update systems
        run(system_input, NULL, &ecs, cmps(2, 0, 1));
        run(system_physics, NULL, &ecs, cmps(2, 0, 1));
        run(system_move, NULL, &ecs, cmps(2, 0, 1));
        run(system_world_collision, NULL, &ecs, cmps(3, 0, 1, 2));
        run(system_ai, &player, &ecs, cmps(2, 0, 1));
        run(system_sound, &shootSound, &ecs, cmps(2, 0, 1));
        visible_count = 0;
        run(system_visibility_check, &camera, &ecs, cmps(2, 0, 3));
        run(system_camera_control, player, &camera);

        // Render
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

        run(system_world_render, visible_entities, &ecs, cmps(2, 0, 3));
        run(system_lighting, NULL, &ecs, cmps(2, 0, 4));

        EndMode3D();
        EndDrawing();
    }

    UnloadSound(shootSound);
    unload_resources();
    CloseWindow(); // De-initialize the window
    return 0;
}

