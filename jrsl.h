/* jrsl - v1.0 - public domain skip list implementation
 *                                    no warranty implied; use at your own risk
 *
 * A C/C++ implementation of William Pugh's Skip Lists with width
 *
 * "Skip lists are a data structure that can be used in place of balanced trees.
 * Skip lists use probabilistic balancing rather than strictly enforced
 * balancing and as a result the algorithms for insertion and deletion in skip
 * lists are much simpler and significantly faster than equivalent algorithms
 * for balanced trees."
 *
 *
 * ============================== RESEARCH PAPER ==============================
 * https: *www.epaperpress.com/sortsearch/download/skiplist.pdf
 *
 *
 * ================================== USAGE ===================================
 * Checkout jsrl_demo.c for an example use of this library.
 *
 * =============================== CONTRIBUTORS ===============================
 * Jack Royer (base file)
 *
 *
 * ================================= LICENSE ==================================
 * Public Domain.
 * See end of file for additional license information.
 *
 */

#ifndef JRSL_H
#define JRSL_H

/* =============================== DOCUMENTATION ==============================
 * This is an implementation of Skip Lists with widths.
 *
 * Why use Skip Lists ?
 * --------------------
 * Skip Lists allow insertion, deletion, random access and search in O(log n)
 * on average (and O(n) in worst case).
 * Skip lists are a simple data structure that can be used in place of balanced
 * trees for most applications and are much less daunting.
 *
 * Single File
 * -----------
 * We are using the standard single file trick. If you just include it normally,
 * you just get the header file function definitions. To get the code, you
 * include it from a C/C++ file and define JHR_SKIP_LIST_IMPLEMENTATION first.
 * (see USAGE)
 *
 * TODO FINISH
 */

#include <assert.h> /* for assert() */
#include <math.h>   /* for log() */
#include <stdio.h>  /* for printf() and sprintf()*/
#include <stdlib.h> /* for malloc() and rand()*/
#include <string.h> /* for strlen() */

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned short jrsl_us;

typedef char (*comparator_t)(void *key1, void *key2);
typedef void (*key_destructor_t)(void *key);
typedef void (*node_visitor_t)(void *key, void *data);
typedef void (*label_printer_t)(void *key, void *data);

struct skip_node_t;
struct link {
  size_t width;
  struct skip_node_t *node;
};

typedef struct skip_node_t {
  struct link *forward;

  void *key;
  void *data;
} skip_node_t;

typedef struct skip_list_t {
  /* Maximum level for this skip list */
  jrsl_us max_level;
  /* The probabilty to add a new level.
   * p is probability so we must have 0<= p <= 1 */
  float p;

  /* Maximum length of a `forward` array */
  jrsl_us level;
  size_t width;

  skip_node_t *head;

  comparator_t comparator;
  key_destructor_t key_destructor;
} skip_list_t;

void jrsl_initialize(skip_list_t *skip_list, comparator_t comparator,
                     key_destructor_t key_destructor, float p,
                     jrsl_us max_level);
void jrsl_destroy(skip_list_t *skip_list, node_visitor_t node_visitor);

void *jrsl_search(skip_list_t *skip_list, void *key);
void *jrsl_insert(skip_list_t *skip_list, void *key, void *data);
void *jrsl_remove(skip_list_t *skip_list, void *key);

void jrsl_display_list(skip_list_t *skip_list, label_printer_t label_printor);

jrsl_us jrsl_max_level(jrsl_us N /* Maximum number of elements */, float p);

static jrsl_us jrsl_random_level(skip_list_t *skip_list);
static void jrsl_center_string(char *str, size_t new_length);
static void jrsl_init_head(skip_list_t *skip_list);

static skip_node_t *jrsl_node_at(skip_list_t *skip_list, size_t index);
void *jrsl_data_at(skip_list_t *skip_list, size_t index);
void *jrsl_key_at(skip_list_t *skip_list, size_t index);

#ifdef __cplusplus
}
#endif

#endif /*!JRSL_H*/
#ifdef JRSL_IMPLEMENTATION

/* Initializes the head of the skip list. Helper function used in
 * `jrsl_initialize`.*/
static void jrsl_init_head(skip_list_t *skip_list) {
  skip_node_t *head = (skip_node_t *)malloc(sizeof(skip_node_t));
  if (!head) {
    /* Malloc failure */
    exit(EXIT_FAILURE);
  }

  head->data = NULL;
  head->key = NULL;
  head->forward =
      (struct link *)malloc(skip_list->max_level * sizeof(struct link));
  if (!head->forward) {
    /* Malloc failure */
    exit(EXIT_FAILURE);
  }

  head->forward[0].node = NULL;
  head->forward[0].width = 0;

  skip_list->head = head;
}

/* Initializes a skip list. */
void jrsl_initialize(skip_list_t *skip_list, comparator_t comparator,
                     key_destructor_t key_destructor, float p,
                     jrsl_us max_level) {
  skip_list->level = 1;
  skip_list->width = 0;
  skip_list->max_level = max_level;
  skip_list->p = p;
  skip_list->comparator = comparator;
  skip_list->key_destructor = key_destructor;

  jrsl_init_head(skip_list);
}

/* Destroys a skip list. Node visitor is applied to every node before the node
 * is freed.*/
void jrsl_destroy(skip_list_t *skip_list, node_visitor_t node_visitor) {
  skip_node_t *node = skip_list->head;
  while (node) {
    skip_node_t *next_node = node->forward[0].node;

    node_visitor(node->key, node->data);
    free(node->forward);
    free(node);
    node = next_node;
  }
}

/* Returns the node with index `index`. If `index` is greater than the width of
 * the skip list, returns NULL. This is a helper function used in `jrsl_key_at`
 * and `jrsl_data_at`.*/
static skip_node_t *jrsl_node_at(skip_list_t *skip_list, size_t index) {
  if (index >= skip_list->width)
    return NULL;

  /* The remaining width to travel. `index` is incremented by 1 because of the
   * presence of a head in the skip list. */
  size_t w = index + 1;

  skip_node_t *x = skip_list->head;

  size_t i;
  for (i = skip_list->level - 1; i >= 0; i--) {
    while (x->forward[i].node && x->forward[i].width <= w) {
      w -= x->forward[i].width;
      x = x->forward[i].node;
      if (w == 0)
        return x;
    }
  }
  /* This should never be called beacuse of the initial check.*/
  return NULL;
}

/* Returns the key of the `index`th element of the skip list.*/
void *jrsl_key_at(skip_list_t *skip_list, size_t index) {
  skip_node_t *node = jrsl_node_at(skip_list, index);
  if (node)
    return node->key;
  return NULL;
}

/* Returns the data of the `index`th element of the skip list.*/
void *jrsl_data_at(skip_list_t *skip_list, size_t index) {
  skip_node_t *node = jrsl_node_at(skip_list, index);
  if (node)
    return node->data;
  return NULL;
}

/* Returns the data of the node with the key `key`. If `key` is not in
 * the skip list returns NULL.
 */
void *jrsl_search(skip_list_t *skip_list, void *key) {
  skip_node_t *x;
  x = skip_list->head;

  size_t i;
  for (i = skip_list->level; i > 0; --i) {
    while (x->forward[i - 1].node != NULL &&
           skip_list->comparator(x->forward[i - 1].node->key, key) < 0) {
      x = x->forward[i - 1].node;
    }
  }
  x = x->forward[0].node;

  if (x)
    if (skip_list->comparator(x->key, key) == 0) {
      return x->data;
    }

  return NULL;
}

/* Inserts a new element in the skip list and returns NULL. If an element with
 * that key is already in the list, updates that element and returns the
 * previous data. */
void *jrsl_insert(skip_list_t *skip_list, void *key, void *data) {
  /* Helper array of pointers to elements that will need updating. */
  skip_node_t **update =
      (skip_node_t **)malloc(skip_list->max_level * sizeof(skip_node_t *));

  /* Helper array to update widths. */
  size_t *update_width =
      (size_t *)malloc(skip_list->max_level * sizeof(size_t));

  if (!update || !update_width) {
    /* Malloc Failure */
    exit(EXIT_FAILURE);
  }

  /* Finds the correct spot for the key in the skip list. */
  skip_node_t *x = skip_list->head;
  size_t i;
  for (i = skip_list->level; i > 0; --i) {
    /* The sum of traveled widths. */
    size_t width_sum = 0;

    while (x->forward[i - 1].node != NULL &&
           skip_list->comparator(x->forward[i - 1].node->key, key) < 0) {
      width_sum += x->forward[i - 1].width;
      x = x->forward[i - 1].node;
    }

    update[i - 1] = x;
    update_width[i - 1] = width_sum;
  }

  /* If the node is already in the list, retuns the already existing node. */
  if (x->forward[0].node) {
    if (skip_list->comparator(x->forward[0].node->key, key) == 0) {
      void *old = x->forward[0].node->data;
      x->forward[0].node->data = data;

      free(update);
      free(update_width);
      return old;
    }
  }

  /* The level for the new node. */
  size_t level = jrsl_random_level(skip_list);

  /* sanity check */
  assert(level < skip_list->max_level);

  /*  Completes the helper arrays if the new node is the first node on a (new)
   * level.*/
  if (level > skip_list->level) {
    size_t i;
    for (i = skip_list->level; i < level; ++i) {
      update[i] = skip_list->head;
      update_width[i] = 0;

      /* The width to NULL is always 0 */
      skip_list->head->forward[i].node = NULL;
      skip_list->head->forward[i].width = 0;
    }
    skip_list->level = level;
  }

  skip_node_t *new_node = (skip_node_t *)malloc(sizeof(skip_node_t));

  if (!new_node) {
    /* Malloc Failure */
    exit(EXIT_FAILURE);
  }

  new_node->data = data;
  new_node->key = key;
  new_node->forward = (struct link *)malloc(level * sizeof(struct link));

  if (!new_node->forward) {
    /* Malloc Failure */
    exit(EXIT_FAILURE);
  }

  /* Inserts the new node in the list. */
  for (i = 0; i < level; ++i) {
    /* Update the linked nodes. */
    new_node->forward[i].node = update[i]->forward[i].node;
    update[i]->forward[i].node = new_node;

    /* Updates the widths of the links. */
    if (i > 0) {
      size_t width_before =
          update_width[i - 1] + update[i - 1]->forward[i - 1].width;

      if (update[i]->forward[i].width > 0)
        /* The width is the width of the previous connection, + 1 ( because we
         * are inserting a node ) - whatever is before the new node.
         */
        new_node->forward[i].width =
            update[i]->forward[i].width + 1 - width_before;
      else
        /* The width to NULL is always 0. */
        new_node->forward[i].width = 0;

      update[i]->forward[i].width = width_before;
    } else { /* i == 0 */
      new_node->forward[i].width = update[i]->forward[i].width;
      update[i]->forward[i].width = 1;
    }
  }

  /* Updates the widths of the links above the newly created node. */
  for (i = level; i < skip_list->level; ++i) {
    if (update[i]->forward[i].node)
      ++update[i]->forward[i].width;
    else
      /* The width to NULL is always 0 and does not need updating. All links
       * above a link pointing to NULL will point to NULL. */
      break;
  }

  skip_list->width++;

  free(update);
  free(update_width);
  return NULL;
}

/* Removes an element from the skip list and returns its data. If it's not
 * in the list returns NULL. */
void *jrsl_remove(skip_list_t *skip_list, void *key) {
  /* Helper array of pointers to elements that will need updating. */
  skip_node_t **update =
      (skip_node_t **)malloc(skip_list->level * sizeof(skip_node_t *));

  if (!update) {
    /* Malloc Failure */
    exit(EXIT_FAILURE);
  }

  /* Finds the theoretical location of the key. */
  skip_node_t *x = skip_list->head;
  size_t i;
  for (i = skip_list->level; i > 0; --i) {
    while (x->forward[i - 1].node != NULL &&
           skip_list->comparator(x->forward[i - 1].node->key, key) < 0) {
      x = x->forward[i - 1].node;
    }
    update[i - 1] = x;
  }
  x = x->forward[0].node;

  /* Could not find the key in the skip list. */
  if (skip_list->comparator(x->key, key) != 0) {
    free(update);
    return NULL;
  }

  /* TODO * /
  /* Updates the list and removes the node */
  for (i = 0; i < skip_list->level; ++i) {
    if (update[i]->forward[i].node) {
      if (skip_list->comparator(update[i]->forward[i].node->key, key) == 0) {
        update[i]->forward[i].node = x->forward[i].node;

        if (x->forward[i].width > 0)
          update[i]->forward[i].width += x->forward[i].width - 1;
        else
          update[i]->forward[i].width = 0;
        continue;
      } else {
        --update[i]->forward[i].width;
      }
    }
  }

  free(update);

  void *old = x->data;
  free(x->forward);
  free(x);

  /* Updates the list's max level */
  while (skip_list->level > 1 &&
         !skip_list->head->forward[skip_list->level - 1].node)
    --skip_list->level;

  return old;
}

static jrsl_us jrsl_random_level(skip_list_t *skip_list) {
  /* We don't actually care about initialization */
  float rnd = rand() / (float)RAND_MAX;
  size_t level = 1;
  while (rnd < skip_list->p && level < skip_list->max_level - 1) {
    level++;
    rnd = rand() / (float)RAND_MAX;
  }
  return level;
}

/* Returns the optimal max level based on the probability `p` to add a new
 * level and the estimated maximum number of elements `N`.
 * If `p` is invalid (p > 1 || p < 0) returns 0 */
jrsl_us jrsl_max_level(jrsl_us N, float p) {
  if (!(0 <= p <= 1))
    return 0;
  return (size_t)(log(N) / log(1 / p));
}

/* Centers a string by padding it left and right with spaces.
 */
static void jrsl_center_string(char *str, size_t new_length) {
  size_t pad_l = (new_length - strlen(str)) / 2;
  size_t pad_r = new_length - pad_l;
  printf("%*s%s%*s", pad_l, "", str, pad_r, "");
}

/* Draws a visual representation of the skip list.
 * A link to a node is represented by an arrow (`o-->`) and final elements of
 * a level, that point to a null pointer, are represented by an `x`.
 */
void jrsl_display_list(skip_list_t *skip_list, label_printer_t label_printer) {
  /* Example result, heavily inspired by wikipedia's illustrations on skip
   * lists
   *            4
   * o---------------------> x Level 2
   *   1     1        2
   * o---> o---> o---------> x Level 1
   *   1     1     1     1     1     1
   * o---> o---> o---> o---> o---> o---> Level 0
   *       3     6     7     9     12
   */
  size_t i;
  for (i = skip_list->level; i > 0; --i) {
    skip_node_t *node = skip_list->head;
    while (node) {
      if (node->forward[i - 1].width > 0) {
        char str[10];
        /* TODO insecure */
        sprintf(str, "%d", node->forward[i - 1].width);
        jrsl_center_string(str, node->forward[i - 1].width * 6 - 1);
      }
      node = node->forward[i - 1].node;
    }
    printf("\n");

    /* Draws the arrows */
    node = skip_list->head;
    while (node) {
      if (node->forward[i - 1].width > 0) {
        printf("o%.*s> ", node->forward[i - 1].width * 6 - 3,
               "---------------------------------------------------------------"
               "--------------------------------------------------------------"
               "-");
      } else {
        printf("x ");
      }
      node = node->forward[i - 1].node;
    }
    printf(" Level %i \n", i - 1);
  }

  /* draws the labels */
  if (label_printer) {
    printf("      ");
    skip_node_t *node = skip_list->head->forward[0].node;
    while (node) {
      label_printer(node->key, node->data);
      node = node->forward[0].node;
    }
  }
}

#endif /*JRSL_IMPLEMENTATION*/

/* ============================================================================
 * This software is available under 2 licenses-- choose whichever you prefer
 * ============================================================================
 * ALTERNATIVE A - MIT License
 *
 * Copyright(c) 2021 Jack Royer
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files(the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and /
 * or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions: The above
 * copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 ============================================================================
 *  ALTERNATIVE B - Public Domain(www.unlicense.org)
 * This is free and unencumbered software released into the public domain.
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute
 * this software, either in source code form or as a compiled binary, for any
 * purpose, commercial or non-commercial, and by any means. In jurisdictions
 * that recognize copyright laws, the author or authors of this software
 * dedicate any and all copyright interest in the software to the public
 * domain. We make this dedication for the benefit of the public at large and
 * to the detriment of our heirs and successors. We intend this dedication to
 * be an overt act of relinquishment in perpetuity of all present and future
 * rights to this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 * ============================================================================
 */
