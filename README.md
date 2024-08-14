<img align="left" style="width:260px" src="./misc/ecs.h.gif" width="288px">

**ecs.h** is a simple, fun, static, ECS library with no overhead.

*"Built for your old ThinkPad."*

No C++, no queries, no tags, no graphs, no relationships. Just deterministic static array access. Almost indistinguishable from direct array access, meaning there is essentially 0 overhead.

This is for people who like developer-focused, high-performance, easy to use code, who doesn't mind editing the header file to meet their constraints.

---

<br>

## Features

- **No external dependencies**
- **No overhead**: indistinguishable[*](https://github.com/173duprot/ecs.h/blob/main/PERFORMANCE.md#static-analysis) from direct iteration.
- **Low Complexity**: only 121 lines of C11
- **Easy to use**: only 7 core, and 2 helper functions
- **Serialization**: save/load the entire game state in milliseconds

## Performance

On a 2020 MacBook, it takes *[~1.2 nanoseconds](https://github.com/173duprot/ecs.h/blob/main/PERFORMANCE.md)* to create an entity.

A trivial update system, iterating over 67072 single-value components, takes *[~285 nanoseconds](https://github.com)*, that's *0.004 ns* per component.

It achieves this by making iterations indistinguishable[*](https://github.com/173duprot/ecs.h/blob/main/PERFORMANCE.md#static-analysis) from direct memory access through smart, static, design patterns.

This makes it [orders of magnitude faster than the next fastest ECS](https://github.com/abeimler/ecs_benchmark?tab=readme-ov-file) in almost every metric.

This is a student research project. I have spent over a month whittling the code down to where it is right now.

### No overhead?

According to performance tests? yes[*](https://github.com/173duprot/ecs.h/blob/main/PERFORMANCE.md)

According to static analyses? yes[*](https://github.com/173duprot/ecs.h/blob/main/PERFORMANCE.md#static-analysis)

### Downsides

Buckle in.

- Max 2048 entities
- Max 32 components
- Less memory efficient than dynamic.

### Who is this for?

People who don't need features or scale, who aren't space-constrained, who are targeting low-performance computing devices.

- This just so happens to describe most Raylib developers!

People who weren’t going to use most ECS features or scale anyway, why not reap the benefit?

- Does your 2D platformer really need ENTT?

Anyone who has avoided the ECS design pattern because of unnecessary complexity and headache.

- This ECS is a joy to use, and it makes things easier, not harder.

## Basic Example

```c
#include "../ecs.h"

typedef enum {
    CMP_ID
} cmp_id;

static inline void example_system(ecs_t* ecs, ent_t ent, void* context) {
    int *component = (int*)get_cmp(ecs, ent, CMP_ID);
    int *data      = (int*)context;
    *component += *data;
}

int main(void) {
    ecs_t* ecs = &(ecs_t){0}; // Static init (for small ecs's)
    //ecs_t* ecs = malloc_ecs(); // Malloc init (for large ecs's)
    ent_t ent = create_ent(ecs);

    int cmp_data = 42;
    add_cmp(ecs, ent, CMP_ID, &cmp_data, sizeof(cmp_data));

    int data = 5;
    run(example_system, &data, ecs, cmps(CMP_ID));

    int *cmp = (int*)get_cmp(ecs, ent, CMP_ID);
    printf("Entity %d, Component %d: %d\n", ent, CMP_ID, *cmp);

    destroy_ent(ecs, ent);
    //free_ecs(ecs);
    return 0;
}
```

### Want more?

If you're looking for more functionality, then you should pair this with our ***lock-free multithreading, real-time, event system [\[evs.h\]](https://github.com/173duprot/evs.h)*** which is powered by the same algorithms that run the Frostbite engine.

## Installation

Clone the repository and include `ECS.H` in your project. No additional setup is required.

```bash
git clone https://github.com/yourusername/ECS.H.git
```

## License

ECS.H is licensed under the MIT License. See [LICENSE](LICENSE) for details.

---
