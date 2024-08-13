#include "../ecs.h"

typedef enum {
    CMP_ID
} cmp_id;

static inline void system(ecs_t* ecs, ent_t ent, void* context) {
    int *component = (int*)get_cmp(ecs, ent, CMP_ID);
    int *data      = (int*)context;
    *component += *data;
}

int main(void) {
    ecs_t ecs = {0};
    ent_t ent = create_ent(&ecs);

    int cmp_data = 42;
    add_cmp(&ecs, ent, CMP_ID, &cmp_data, sizeof(cmp_data));

    int data = 5;
    run(system, &data, &ecs, cmps(CMP_ID));

    int *cmp = (int*)get_cmp(&ecs, ent, CMP_ID);
    printf("Entity %d, Component %d: %d\n", ent, CMP_ID, *cmp);

    destroy_ent(&ecs, ent);
    return 0;
}
