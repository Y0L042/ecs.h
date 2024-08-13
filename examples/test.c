#include "../ecs.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

#define SAVE_FILE "save.dat"
#define NUM_ITERATIONS 100000

static inline void component_iter_callback(ecs_t *ecs, ent_t ent, void* data) {
    int* cmp = get_cmp(ecs, ent, 0);
    *cmp = (*cmp)+1;
}

double get_time_in_nanoseconds(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
}

int main() {
    ecs_t ecs = {0};
    struct timespec start, end;
    double total_create_duration = 0;
    double total_add_duration = 0;
    double total_save_duration = 0;
    double total_load_duration = 0;
    double total_iterate_duration = 0;
    double total_destroy_duration = 0;

    for (size_t iter = 0; iter < NUM_ITERATIONS; ++iter) {
        memset(&ecs, 0, sizeof(ecs));

        // Creating entities
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        for (size_t i = 0; i < MAX_ENTS; ++i) {
            create_ent(&ecs);
        }
        // Add this back for more granular satistics
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_create_duration += get_time_in_nanoseconds(start, end);

        // Adding components to entities
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        int data[MAX_CMP_SIZE / sizeof(int)];
        for (size_t i = 0; i < MAX_CMP_SIZE / sizeof(int); ++i) {
            data[i] = i;
        }
        for (size_t i = 0; i < MAX_ENTS; ++i) {
            for (size_t j = 0; j < MAX_CMPS; ++j) {
                add_cmp(&ecs, i, j, data, sizeof(data));
            }
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_add_duration += get_time_in_nanoseconds(start, end);

        // Saving ecs_t state
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
//        ecs_save(&ecs, SAVE_FILE);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_save_duration += get_time_in_nanoseconds(start, end);

        // Loading ecs_t state
        ecs_t loaded_ecs = {0};
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
//        ecs_load(&loaded_ecs, SAVE_FILE);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_load_duration += get_time_in_nanoseconds(start, end);

        // Iterating over entities with callback
        //int sum = 0;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
                                                    // Over every component
        run(component_iter_callback, NULL, &loaded_ecs, 0xFFFFFFFFFFFFFFFF);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_iterate_duration += get_time_in_nanoseconds(start, end);

        // Destroying entities
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        for (size_t i = 0; i < MAX_ENTS; ++i) {
            destroy_ent(&loaded_ecs, i);
        }
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        total_destroy_duration += get_time_in_nanoseconds(start, end);
    }

    printf("Average time taken to create %d entities: %.2f nanoseconds\n", MAX_ENTS, total_create_duration / NUM_ITERATIONS);
    printf("Average time taken to add %d components to each of %d entities: %.2f nanoseconds\n", MAX_CMPS, MAX_ENTS, total_add_duration / NUM_ITERATIONS);
    //printf("Average time taken to create %d entities and add %d components to each: %.2f nanoseconds\n", MAX_ENTS, MAX_CMPS, total_add_duration / NUM_ITERATIONS);
    printf("Average time taken to save ecs_t state: %.2f nanoseconds\n", total_save_duration / NUM_ITERATIONS);
    printf("Average time taken to load ecs_t state: %.2f nanoseconds\n", total_load_duration / NUM_ITERATIONS);
    printf("Average time taken to iterate over %d entities and iterate %d components through callback: %.2f nanoseconds\n", MAX_ENTS, MAX_CMPS, total_iterate_duration / NUM_ITERATIONS);
    printf("Average time taken to destroy %d entities: %.2f nanoseconds\n", MAX_ENTS, total_destroy_duration / NUM_ITERATIONS);

    return 0;
}
