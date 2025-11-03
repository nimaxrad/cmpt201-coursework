#define _DEFAULT_SOURCE
#define _ISOC99_SOURCE
#include "alloc.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
static struct header *free_head = NULL;
static enum algs current_algo = FIRST_FIT;
static int heap_limit = 0;
uint64_t heap_used = 0;
static void *initial_break = NULL;
static void coalesce_all(void);

static struct header *grow_heap(void) {
  if (heap_limit > 0 && heap_used + INCREMENT > (uint64_t)heap_limit)
    return NULL;
  void *base = sbrk(INCREMENT);
  if (base == (void *)-1)
    return NULL;
  heap_used += INCREMENT;
  struct header *h = (struct header *)base;
  h->size = (uint64_t)INCREMENT;
  h->next = free_head;
  free_head = h;
  return h;
}
static struct header *find_block(uint64_t total_needed,
                                 struct header **prev_out) {
  coalesce_all();
  struct header *best = NULL, *best_prev = NULL;
  struct header *prev = NULL, *cur = free_head;
  while (cur) {
    if (cur->size >= total_needed) {
      if (current_algo == FIRST_FIT) {
        *prev_out = prev;
        return cur;
      }
      if (!best) {
        best = cur;
        best_prev = prev;
      } else if (current_algo == BEST_FIT && cur->size < best->size) {
        best = cur;
        best_prev = prev;
      } else if (current_algo == WORST_FIT && cur->size > best->size) {
        best = cur;
        best_prev = prev;
      }
    }
    prev = cur;
    cur = cur->next;
  }
  *prev_out = best_prev;
  return best;
}
static void remove_from_free(struct header *prev, struct header *blk) {
  if (prev)
    prev->next = blk->next;
  else
    free_head = blk->next;
  blk->next = NULL;
}
static void coalesce_all(void) {
  int merged;
  do {
    merged = 0;
    struct header *prev_a = NULL, *a = free_head;
    while (a) {
      struct header *prev_b = NULL, *b = free_head;
      while (b) {
        if (b == a) {
          prev_b = b;
          b = b->next;
          continue;
        }
        char *a_start = (char *)a;
        char *b_start = (char *)b;
        char *a_end = a_start + a->size;
        char *b_end = b_start + b->size;
        if (a_end == b_start) {
          a->size += b->size;
          if (prev_b)
            prev_b->next = b->next;
          else
            free_head = b->next;
          b = (prev_b ? prev_b->next : free_head);
          merged = 1;
          continue;
        }
        if (b_end == a_start) {
          b->size += a->size;
          if (prev_a)
            prev_a->next = a->next;
          else
            free_head = a->next;
          a = (prev_a ? prev_a->next : free_head);
          merged = 1;
          goto next_a;
        }
        prev_b = b;
        b = b->next;
      }
      prev_a = a;
      a = a->next;
    next_a:;
    }
  } while (merged);
}
void *alloc(int size) {
  if (size <= 0)
    return NULL;
  uint64_t total_needed = (uint64_t)sizeof(struct header) + (uint64_t)size;
  if (!initial_break)
    initial_break = sbrk(0);
  if (!free_head && !grow_heap())
    return NULL;
  struct header *prev = NULL;
  struct header *blk = NULL;
  while (1) {
    blk = find_block(total_needed, &prev);
    if (blk)
      break;
    if (!grow_heap())
      return NULL;
  }
  remove_from_free(prev, blk);
  uint64_t remainder = blk->size - total_needed;
  if (remainder > (uint64_t)sizeof(struct header)) {
    struct header *rem = (struct header *)((char *)blk + total_needed);
    rem->size = remainder;
    rem->next = free_head;
    free_head = rem;
    blk->size = total_needed;
  }
  return (char *)blk + sizeof(struct header);
}
void dealloc(void *ptr) {
  if (!ptr)
    return;
  struct header *h = (struct header *)((char *)ptr - sizeof(struct header));
  h->next = free_head;
  free_head = h;
  coalesce_all();
}
void allocopt(enum algs algo, int limit) {
  if (!initial_break)
    initial_break = sbrk(0);
  brk(initial_break);
  free_head = NULL;
  heap_used = 0;
  current_algo = algo;
  heap_limit = limit;
}
struct allocinfo allocinfo(void) {
  struct allocinfo info = (struct allocinfo){0, 0, 0, 0};
  for (struct header *p = free_head; p; p = p->next) {
    uint64_t payload = (p->size > (uint64_t)sizeof(struct header))
                           ? (p->size - (uint64_t)sizeof(struct header))
                           : 0;
    info.free_size += payload;
    info.free_chunks++;
    if (payload > info.largest_free_chunk_size)
      info.largest_free_chunk_size = payload;
    if (info.smallest_free_chunk_size == 0 ||
        payload < info.smallest_free_chunk_size)
      info.smallest_free_chunk_size = payload;
  }
  return info;
}
