#define JRSL_IMPLEMENTATION
#include "jrsl.h"

/* The probability to add a new level to the skip list*/
#define P 0.5f

/* A helper function used during the skip list's destruction */
void free_data(void *key, void *data) { free(data); }

void label_printer(void *key, void *data) {
  printf("%s%*s", (char *)key, 6 - strlen((char *)key), "");
}

int main() {

  skip_list_t skip_list;

  char *data[] = {"a", "e", "w", "d", "q", "u", "y", "b", "n",
                  "c", "t", "m", "f", "z", "g", "o", "s", "h",
                  "v", "i", "j", "p", "k", "r", "x", "l"};

  size_t i; /* used in for loops */

  /* Let's initialize the skip list using jrsl_initialize, we eill use a
   * probability of 0.5. We can use jrsl_max_level to determine the optimum
   * amount of levels knowing the maximum size of our list.*/
  jrsl_initialize(&skip_list, (comparator_t)strcmp, free, P,
                  jrsl_max_level(26, P));

  printf("\n\nEmpty skip list\n");
  jrsl_display_list(&skip_list, label_printer);

  /* Inserting elements into the skip list to fill it up. We won't be using any
   * useful data, we will just malloc for every letter. */
  printf("\n\nInserting elements\n");
  for (i = 0; i < 26; i++) {
    jrsl_insert(&skip_list, data[i], malloc(sizeof(int)));
  }
  jrsl_display_list(&skip_list, label_printer);

  /* Inserting an element which is already in the list */
  printf("\n\nInserting an element which is already in the list\n");
  free(jrsl_insert(&skip_list, data[0], malloc(sizeof(int))));
  jrsl_display_list(&skip_list, label_printer);

  /* Let's get rid of some things. */
  /* jrsl_remove will call free() on the key (since we specified it as a key
   * destructor) but it won't touch the data, it'll just return what it found at
   * that key, if anything, or NULL otherwise.*/

  printf("\n\nRemoving all the vowels\n");
  free(jrsl_remove(&skip_list, "a"));
  free(jrsl_remove(&skip_list, "e"));
  free(jrsl_remove(&skip_list, "i"));
  free(jrsl_remove(&skip_list, "o"));
  free(jrsl_remove(&skip_list, "u"));
  free(jrsl_remove(&skip_list, "skip_list"));
  jrsl_display_list(&skip_list, label_printer);

  /* Let's search for items inside of the skip list. If the key matches a key in
   * the skip list then jrsl_search will return the data associated with that
   * key, else it will return NULL*/
  printf("\n\nSearching for elements\n");
  printf("Is `a` in the skip list ? %i\n",
         jrsl_search(&skip_list, "a") != NULL);
  printf("Is `f` in the skip list ? %i\n",
         jrsl_search(&skip_list, "f") != NULL);

  /* Skip lists can also be used for random access. We can use jrsl_key_at to
   * retrieve the key of a given node. If the index is greater than the total
   * width of the skip list, jrsl_key_at will return null. Note, their is also a
   * jrsl_data_at function which returns the data of the node.*/
  printf("\n\nRandom Access \n");
  printf("The 5th element of the skip list is %s\n",
         (char *)jrsl_key_at(&skip_list, 5));
  printf("The 10th element of the skip list is %s\n",
         (char *)jrsl_key_at(&skip_list, 10));
  printf("The 15th element of the skip list is %s\n",
         (char *)jrsl_key_at(&skip_list, 15));
  printf("The 50th element of the skip list is %s\n",
         (char *)jrsl_key_at(&skip_list, 50));

  /* Now it's time to get rid of the entire skip list. The second argument
   * is a function to be called on each node in order to clean up. */
  jrsl_destroy(&skip_list, free_data);

  return 0;
}
