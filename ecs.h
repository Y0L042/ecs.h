#ifndef ECS_H
#define ECS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define inline inline __attribute__((always_inline)) // Force inlines

// Settings are tuned for performance.
#define MAX_ENTS 1024 /* MAX 2048 */
#define MAX_CMPS 32    /* MAX 64 */
#define MAX_CMP_SIZE 8 /* MAX 64 */

typedef uint32_t ent_t;
typedef uint16_t cmp_t;
typedef uint64_t cmps_t; // Component bitmask

// Structure of Arrays for performance
typedef struct { 
    size_t free_count; 
    ent_t free_list[MAX_ENTS]; // List of free entity slots

    size_t active_count;      
    ent_t active_list[MAX_ENTS]; // List of active entities

    size_t ent_count; 
    cmps_t ent_cmps[MAX_ENTS]; // List of entities (groups of components)

    uint8_t data[MAX_CMPS][MAX_ENTS * MAX_CMP_SIZE]; // Raw Storage
} ecs_t;

// System type
typedef void (*system_t)(ecs_t*, ent_t, void*);

// Bitmask Macros
#define   SET_BIT(mask, bit)  ((mask) |=  (1ULL << (bit)))
#define CLEAR_BIT(mask, bit)  ((mask) &= ~(1ULL << (bit)))
#define CHECK_BIT(mask, bit) (((mask) &   (1ULL << (bit))) != 0)


/* Helper functions */

// Easy component access for systems
static inline void* get_cmp(ecs_t* ecs, ent_t ent, cmp_t cmp_id) {
    return &ecs->data[cmp_id][ent * MAX_CMP_SIZE];
}

// Component mask generator
static inline cmps_t cmps(int count, ...) {
    uint64_t bitmask = 0;
    va_list args;
    va_start(args, count);
    
    for (int i = 0; i < count; i++) {
        int bit = va_arg(args, int);
        bitmask |= (1ULL << bit);
    }
    
    va_end(args);
    return bitmask;
}


/* Core functions */

// Entity management
//// Create entity
static inline ent_t create_ent(ecs_t* ecs) {
    ent_t ent = (ecs->free_count > 0)
                ? ecs->free_list[--ecs->free_count]
                : (ecs->ent_count < MAX_ENTS)
                  ? ecs->ent_count++
                  : (ent_t)-1;
    if (ent != (ent_t)-1) {
        ecs->active_list[ecs->active_count++] = ent;
    }
    return ent;
}
//// Destroy entity
static inline void destroy_ent(ecs_t* ecs, ent_t ent) {
    ecs->ent_cmps[ent] = 0;
    ecs->free_list[ecs->free_count++] = ent;
    for (size_t i = 0; i < ecs->active_count; ++i) {
        if (ecs->active_list[i] == ent) {
            ecs->active_list[i] = ecs->active_list[--ecs->active_count];
            break;
        }
    }
}

// Component management
//// Add component
static inline int add_cmp(ecs_t* ecs, ent_t ent, cmp_t cmp_id, const void* data, size_t size) {
    if (CHECK_BIT(ecs->ent_cmps[ent], cmp_id)) return -1; // Ensure the spot is open
    if (size > MAX_CMP_SIZE || ent >= MAX_ENTS) return -1; // Ensure size does not exceed the limit and entity index is active

    SET_BIT(ecs->ent_cmps[ent], cmp_id); 
    memcpy(&ecs->data[cmp_id][ent * MAX_CMP_SIZE], data, size);
    return 0;
}
//// Delete component
static inline int del_cmp(ecs_t* ecs, ent_t ent, cmp_t cmp_id) {
    if (!CHECK_BIT(ecs->ent_cmps[ent], cmp_id)) return -1; // Ensure the spot is full
         CLEAR_BIT(ecs->ent_cmps[ent], cmp_id); return 0;
         // Check bit, clear bit.
}

// System
//// Run a system 
static inline void run(system_t system, void* data, ecs_t* ecs, cmps_t cmps) {
    for (size_t i = 0; i < ecs->active_count; ++i) { 
        ent_t ent = ecs->active_list[i];
        if ((ecs->ent_cmps[ent] & cmps) == cmps) {
            system(ecs, ent, data);
        }
    } 
}

// Serialization
//// Save
static inline int save_ecs(ecs_t* ecs, const char* filename) {
    if (!ecs || !filename) return -1;

    FILE* file = fopen(filename, "wb");
    if (!file) return -1;

    fwrite(ecs, sizeof(ecs_t), 1, file);
    fclose(file);
    return 0;
}
//// Load
static inline int load_ecs(ecs_t* ecs, const char* filename) {
    if (!ecs || !filename) return -1;

    FILE* file = fopen(filename, "rb");
    if (!file) return -1;

    fread(ecs, sizeof(ecs_t), 1, file);
    fclose(file);
    return 0;
}


#endif // ECS_H
