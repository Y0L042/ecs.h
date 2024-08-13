# Performance

*The following tests were performed with simple code avalible in test.c, adding and removing simple features.*

## Current performance

```
Average time taken to create 1024 entities: 894.56 nanoseconds
Average time taken to add 32 components to each of 1024 entities: 16816.18 nanoseconds
Average time taken to save ecs_t state: 249.47 nanoseconds
Average time taken to load ecs_t state: 248.63 nanoseconds
Average time taken to iterate over 1024 entities and iterate 32 components through callback: 246.99 nanoseconds
Average time taken to destroy 1024 entities: 250.03 nanoseconds
```
Because of the scale of this engine, as long as you use 1-val components, and atomic `~1-3op` system design, you will get trivial system operation.

If you can do it determinsiticly with few ops, do it, your cpu will thank you.

## Direct access comparison

At one point I was exploring weather I could try utilizing direct access to eliminate the idea of entities altogether, so I did a simple test itterating through the arrays directly. Too my suprize, giving up the fine-grained control of the ECS system, in the best case scenero only beat the ECS by *10ns*. You can find the original file [here](https://github.com/173duprot/ecs.h/blob/e1a355d85da10a84ec4a6f4e48b9a1ed71abe685/test.c).

Anyways, here were my results:

```
Average time taken to create 1024 entities: 839.00 nanoseconds
Average time taken to add 32 components to each of 1024 entities: 369337.80 nanoseconds
Average time taken to save ECS state: 649924.80 nanoseconds
Average time taken to load ECS state: 121865.80 nanoseconds
Average time taken to iterate over 1024 entities and iterate components through callback: 275.40 nanoseconds
Average time taken to iterate over 1024 entities and iterate components indescriminantly through direct access: 264.60 nanoseconds
Average time taken to destroy 1024 entities: 452.50 nanoseconds
```


1024 entities, 32 components:
- Callback: 275.40 nanoseconds
- Direct: 264.60 nanoseconds

In this run it created a *~11ns* overhead in the best possible case.

----

For comparison with [this chart](https://github.com/abeimler/ecs_benchmark?tab=readme-ov-file#create-entities) I ran the performance test. It is over ~6x times faster than pico_ecs, and over ~12x faster than ENTT.

```
Average time taken to create 64 entities and add 2 components to each: 450.90 nanoseconds
Average time taken to save ecs_t state: 73516.60 nanoseconds
Average time taken to load ecs_t state: 11133.50 nanoseconds
Average time taken to iterate over 64 entities and iterate 2 components through callback: 295.50 nanoseconds
Average time taken to destroy 64 entities: 296.90 nanoseconds
```

I would like to draw your attention to the fact that it takes the same amount of time to update 128 components, as it takes to update 32768, this is because we are running into the hardware limits. We are being constrained by memory access, not CPU.

You can only achive this result by utilizing single-value components, with 1-op callbacks.

# Static analysis

The code is trivial, determinsitic, and introduced a total `~15op` overhead worst-case `-O0` in example.c.

At higher optomizations, it is almost indistinguishable from direct memory access, around `~5op` is the worst we have seen based on loose guestimations. 

(Analyzing code on `-O3` is a pain in the ass.)
