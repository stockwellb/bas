/* bas_arena.h - v0.1 - arena (bump) allocator - (bas)

   A single-file library providing a generic allocator interface and a
   linear "bump" arena that implements it.

   Do this:

       #define BAS_ARENA_IMPLEMENTATION

   before you include this file in *one* C file to create the
   implementation. That file should look like this:

       #include ...
       #include ...
       #define BAS_ARENA_IMPLEMENTATION
       #include "bas_arena.h"

   All other files should just #include "bas_arena.h" without the #define.


   DOCUMENTATION

   Two things share this file:

     - bas_allocator : a small vtable interface (alloc / realloc / free) that
                       callers can accept to stay agnostic of where memory
                       comes from.
     - bas_arena     : a linear allocator that carves allocations out of one
                       up-front buffer by bumping an offset. Individual
                       allocations are never freed; the whole arena is
                       reclaimed at once (see bas_arena_reset / bas_arena_free).

   An arena embeds a bas_allocator as its first member, so a pointer to an
   arena is usable anywhere a bas_allocator* is expected:

       bas_arena arena;
       if (!bas_arena_init(&arena, 1 << 16)) { ... }

       int *xs = bas_arena_alloc(&arena, 256 * sizeof *xs);

       bas_allocator *a = &arena.base;
       void *p = a->alloc(a, 128);

       bas_arena_free(&arena);

   Allocations are aligned to alignof(max_align_t) by default; use
   bas_arena_alloc_aligned for a specific (power-of-two) alignment.


   COMPILE-TIME OPTIONS

   #define BAS_ARENADEF <specifier>

       Controls linkage of every function. Defaults to `extern` for
       declarations and nothing for definitions. Define it (before the first
       include) to e.g. `static` to get an internal-linkage build:

           #define BAS_ARENADEF static
           #define BAS_ARENA_IMPLEMENTATION
           #include "bas_arena.h"


   LICENSE

   MIT License. See the LICENSE file distributed with this project.
*/

#ifndef BAS_INCLUDE_BAS_ARENA_H
#define BAS_INCLUDE_BAS_ARENA_H

#include <stdbool.h>
#include <stddef.h>

#ifndef BAS_ARENADEF
#ifdef BAS_ARENA_STATIC
#define BAS_ARENADEF static
#else
#define BAS_ARENADEF extern
#endif
#endif

/* ========================================================================= */
/* Allocator interface                                                       */
/* ========================================================================= */

/* A generic allocator: a set of function pointers plus whatever state the
 * concrete allocator threads through `self`. Callers that accept a
 * bas_allocator* stay agnostic of where their memory comes from. */
typedef struct bas_allocator {
  void *(*alloc)(struct bas_allocator *self, size_t size);
  void *(*realloc)(struct bas_allocator *self, void *ptr, size_t old_size,
                   size_t new_size);
  void (*free)(struct bas_allocator *self, void *ptr, size_t size);
} bas_allocator;

/* ========================================================================= */
/* Arena                                                                     */
/* ========================================================================= */

/* A linear "bump" allocator over a single owned buffer. `base` must be the
 * first member so a bas_arena* can be passed as a bas_allocator*. */
typedef struct bas_arena {
  bas_allocator base;
  unsigned char *buf;
  size_t offset;
  size_t capacity;
} bas_arena;

/* Initialize *arena with a freshly malloc'd buffer of `capacity` bytes.
 *
 * Returns true on success. Returns false (leaving the arena empty, with a
 * NULL buffer and zero capacity) when capacity is 0 or the allocation fails.
 * The allocator vtable is always installed, so a failed arena still frees and
 * allocates safely (every allocation simply fails). */
BAS_ARENADEF bool bas_arena_init(bas_arena *arena, size_t capacity);

/* Release the arena's buffer and reset it to the empty state. Safe to call on
 * a zero-initialized or already-freed arena. */
BAS_ARENADEF void bas_arena_free(bas_arena *arena);

/* Reset the arena to empty without releasing its buffer, making all previously
 * allocated memory available again. */
BAS_ARENADEF void bas_arena_reset(bas_arena *arena);

/* Allocate `size` bytes aligned to alignof(max_align_t).
 *
 * Returns a pointer into the arena, or NULL if size is 0 or the arena does not
 * have room. The memory is uninitialized. */
BAS_ARENADEF void *bas_arena_alloc(bas_arena *arena, size_t size);

/* Allocate `size` bytes aligned to `align`, which must be a power of two.
 *
 * Returns a pointer into the arena, or NULL if size is 0 or the arena does not
 * have room after aligning. The memory is uninitialized. */
BAS_ARENADEF void *bas_arena_alloc_aligned(bas_arena *arena, size_t size,
                                           size_t align);

#endif /* BAS_INCLUDE_BAS_ARENA_H */


/* ------------------------------------------------------------------------- */
/* IMPLEMENTATION                                                            */
/* ------------------------------------------------------------------------- */

#ifdef BAS_ARENA_IMPLEMENTATION

#include <assert.h>
#include <stdalign.h>
#include <stdlib.h>
#include <string.h>

/* ---- Allocator vtable adapters ------------------------------------------ */

static void *bas_arena__vtable_alloc(bas_allocator *self, size_t size) {
  return bas_arena_alloc((bas_arena *)self, size);
}

static void *bas_arena__vtable_realloc(bas_allocator *self, void *ptr,
                                       size_t old_size, size_t new_size) {
  void *new_ptr = bas_arena_alloc((bas_arena *)self, new_size);
  size_t copy_size = old_size < new_size ? old_size : new_size;

  if (new_ptr != NULL && ptr != NULL) {
    memcpy(new_ptr, ptr, copy_size);
  }

  return new_ptr;
}

static void bas_arena__vtable_free(bas_allocator *self, void *ptr,
                                   size_t size) {
  (void)self;
  (void)ptr;
  (void)size; /* no-op: an arena frees everything at once */
}

/* ---- Arena -------------------------------------------------------------- */

BAS_ARENADEF bool bas_arena_init(bas_arena *arena, size_t capacity) {
  arena->base.alloc = bas_arena__vtable_alloc;
  arena->base.realloc = bas_arena__vtable_realloc;
  arena->base.free = bas_arena__vtable_free;
  arena->buf = NULL;
  arena->offset = 0;
  arena->capacity = 0;

  if (capacity == 0) {
    return false;
  }

  arena->buf = malloc(capacity);

  if (arena->buf == NULL) {
    return false;
  }

  arena->capacity = capacity;

  return true;
}

BAS_ARENADEF void bas_arena_free(bas_arena *arena) {
  free(arena->buf);
  arena->buf = NULL;
  arena->offset = 0;
  arena->capacity = 0;
}

BAS_ARENADEF void bas_arena_reset(bas_arena *arena) { arena->offset = 0; }

BAS_ARENADEF void *bas_arena_alloc_aligned(bas_arena *arena, size_t size,
                                           size_t align) {
  assert(align > 0 && (align & (align - 1)) == 0); /* power of two */

  if (size == 0) {
    return NULL;
  }

  size_t aligned = (arena->offset + align - 1) & ~(align - 1);

  if (aligned > arena->capacity || size > arena->capacity - aligned) {
    return NULL;
  }

  void *ptr = arena->buf + aligned;
  arena->offset = aligned + size;

  return ptr;
}

BAS_ARENADEF void *bas_arena_alloc(bas_arena *arena, size_t size) {
  return bas_arena_alloc_aligned(arena, size, alignof(max_align_t));
}

#endif /* BAS_ARENA_IMPLEMENTATION */
