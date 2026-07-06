#include "bas_arena.h"
#include "bas_test.h"

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

/* init */

static void bas_arena_init_sets_buffer_capacity_and_offset(void) {
  bas_arena arena;

  CHECK_TRUE(bas_arena_init(&arena, 1024));
  CHECK_NOT_NULL(arena.buf);
  CHECK(arena.capacity == 1024);
  CHECK(arena.offset == 0);

  bas_arena_free(&arena);
}

static void bas_arena_init_with_zero_capacity_yields_empty_arena(void) {
  bas_arena arena;

  CHECK_FALSE(bas_arena_init(&arena, 0));
  CHECK_NULL(arena.buf);
  CHECK(arena.capacity == 0);

  bas_arena_free(&arena);
}

/* alloc */

static void bas_arena_alloc_returns_aligned_pointer_and_bumps_offset(void) {
  bas_arena arena;
  bas_arena_init(&arena, 1024);

  void *p = bas_arena_alloc(&arena, 64);

  CHECK_NOT_NULL(p);
  CHECK(arena.offset == 64);
  CHECK((uintptr_t)p % alignof(max_align_t) == 0);

  bas_arena_free(&arena);
}

static void bas_arena_alloc_zero_returns_null(void) {
  bas_arena arena;
  bas_arena_init(&arena, 1024);

  CHECK_NULL(bas_arena_alloc(&arena, 0));

  bas_arena_free(&arena);
}

static void bas_arena_alloc_fills_whole_buffer(void) {
  bas_arena arena;
  bas_arena_init(&arena, 1024);

  void *p = bas_arena_alloc(&arena, 1024);

  CHECK_NOT_NULL(p);
  CHECK(arena.offset == 1024);

  bas_arena_free(&arena);
}

static void bas_arena_alloc_beyond_capacity_returns_null(void) {
  bas_arena arena;
  bas_arena_init(&arena, 1024);

  CHECK_NULL(bas_arena_alloc(&arena, 100000));

  bas_arena_free(&arena);
}

static void bas_arena_alloc_one_byte_past_capacity_returns_null(void) {
  bas_arena arena;
  bas_arena_init(&arena, 1024);

  CHECK_NULL(bas_arena_alloc(&arena, 1025));

  bas_arena_free(&arena);
}

static void bas_arena_alloc_sequentially_bumps_with_alignment(void) {
  bas_arena arena;
  bas_arena_init(&arena, 1024);

  void *p1 = bas_arena_alloc(&arena, 2);
  CHECK_NOT_NULL(p1);
  CHECK(arena.offset == 2);

  void *p2 = bas_arena_alloc(&arena, 2);
  size_t a = alignof(max_align_t);
  size_t expected = ((2 + a - 1) & ~(a - 1)) + 2;
  CHECK_NOT_NULL(p2);
  CHECK(arena.offset == expected);
  CHECK(p1 != p2);

  bas_arena_free(&arena);
}

/* alloc_aligned */

static void bas_arena_alloc_aligned_honors_requested_alignment(void) {
  bas_arena arena;
  bas_arena_init(&arena, 1024);

  (void)bas_arena_alloc_aligned(&arena, 10, 64);
  void *p = bas_arena_alloc_aligned(&arena, 10, 64);

  CHECK_NOT_NULL(p);
  CHECK(arena.offset == 64 + 10);
  CHECK((uintptr_t)p % 64 == 0);

  bas_arena_free(&arena);
}

/* reset */

static void bas_arena_reset_reclaims_buffer_without_freeing(void) {
  bas_arena arena;
  bas_arena_init(&arena, 1024);

  void *first = bas_arena_alloc(&arena, 128);
  CHECK_NOT_NULL(first);
  CHECK(arena.offset == 128);

  bas_arena_reset(&arena);
  CHECK(arena.offset == 0);
  CHECK_NOT_NULL(arena.buf);

  void *again = bas_arena_alloc(&arena, 128);
  CHECK(again == first);

  bas_arena_free(&arena);
}

/* free */

static void bas_arena_free_resets_to_empty_state(void) {
  bas_arena arena;
  bas_arena_init(&arena, 1024);

  bas_arena_free(&arena);

  CHECK_NULL(arena.buf);
  CHECK(arena.capacity == 0);
  CHECK(arena.offset == 0);
}

int main(void) {
  /* init */
  RUN_TEST(bas_arena_init_sets_buffer_capacity_and_offset);
  RUN_TEST(bas_arena_init_with_zero_capacity_yields_empty_arena);

  /* alloc */
  RUN_TEST(bas_arena_alloc_returns_aligned_pointer_and_bumps_offset);
  RUN_TEST(bas_arena_alloc_zero_returns_null);
  RUN_TEST(bas_arena_alloc_fills_whole_buffer);
  RUN_TEST(bas_arena_alloc_beyond_capacity_returns_null);
  RUN_TEST(bas_arena_alloc_one_byte_past_capacity_returns_null);
  RUN_TEST(bas_arena_alloc_sequentially_bumps_with_alignment);

  /* alloc_aligned */
  RUN_TEST(bas_arena_alloc_aligned_honors_requested_alignment);

  /* reset */
  RUN_TEST(bas_arena_reset_reclaims_buffer_without_freeing);

  /* free */
  RUN_TEST(bas_arena_free_resets_to_empty_state);

  TEST_SUMMARY();

  return bas_tests_failed > 0;
}
