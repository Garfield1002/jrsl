# C\C++ Skip List Implementation

This is C89 implementation of the data type described in [William Pugh's paper](https:www.epaperpress.com/sortsearch/download/skiplist.pdf) with widths.

> Skip lists are a data structure that can be used in place of balanced trees.
> Skip lists use probabilistic balancing rather than strictly enforced balancing
> and as a result the algorithms for insertion and deletion in skip lists are
> much simpler and significantly faster than equivalent algorithms for
> balanced trees.

Using widths allows for efficient random access.

Although this library will work with C and C++, if you perfer fancy C++ objects you can check out [my first implementation of skip lists](https://github.com/Garfield1002/jhr_skip_list)

## 💡 Why use Skip Lists ?

Skip Lists allow insertion, deletion, random access and search in O(log n) on average (and O(n) in worst case).
Skip lists are a simple data structure that can be used in place of balanced trees for most applications and are much less daunting.

## 📚 Usage

**This library is written in standard C89 for portability.**

The idea behind single-header file libraries is that they're easy to distribute and deploy because all the code is contained in a single file.
The .h file acts as its own header files, i.e. it declares the functions and classes contained in the file but no code is getting compiled.

So in addition, you should select _exactly_ one C\C++ source file that actually instantiates the code, preferably a file you're not editing frequently.
This file should define `JRSL_IMPLEMENTATION` to actually enable the function definitions.

You can check out [example.c](https://github.com/Garfield1002/jrsl/blob/master/example/example.c) for some sample code.

### Skip List Methods

---

<table>
  <tr>
    <th>Function</th>
    <th>Effect</th>
  </tr>
  <tr>
    <td colspan="2">
        <b>Creation and deletion</b>
    </td>
  </tr>
  <tr>
    <td>jrsl_initialize()</td>
    <td>Initializes a skip list</td>
  </tr>
  <tr></tr>
  <tr>
    <td>jrsl_destroy()</td>
    <td>Cleans up a skip list</td>
  </tr>
  <tr>
    <td colspan="2">
        <b>Size and capacity</b>
    </td>
  </tr>
  <tr>
    <td>jrsl_max_level()</td>
    <td>Calculates the optimal maximum for the amount of levels</td>
  </tr>
  <tr>
    <td colspan="2">
        <b>Element access</b>
    </td>
  </tr>
  <tr>
    <td>jrsl_data_at()</td>
    <td>Returns the data at a particular index</td>
  </tr>
  <tr></tr>
    <tr>
    <td>jrsl_key_at()</td>
    <td>Returns the key at a particular index</td>
  </tr>
  <tr>
    <td colspan="2">
        <b>Modification</b>
    </td>
  </tr>
  <tr>
    <td>jrsl_insert()</td>
    <td>Inserts an element (a key and some data) in the skip list and returns `NULL`</td>
  </tr>
  <tr></tr>
  <tr>
    <td>jrsl_remove()</td>
    <td>Removes an element from the skip list, deletes the key and returns it's data</td>
  </tr>
  <tr>
    <td colspan="2">
        <b>Searching</b>
    </td>
  </tr>
  <tr>
    <td>jrsl_search()</td>
    <td>Returns the data of the node with a given key</td>
  </tr>
  <tr>
    <td colspan="2">
        <b>Visualization</b>
    </td>
  </tr>
  <tr>
    <td>jrsl_display_list()</td>
    <td>Prints a visual representation of the skip list</td>
  </tr>
</table>

## ⭐ Contribution

All contributions are welcome!

## ⚖ License

This library is in the public domain. You can do anything you want with it. You have no legal obligation to do anything else, although I appreciate attribution.

It is also licensed under the MIT open source license, if you have lawyers who are unhappy with public domain. The source file includes an explicit dual-license for you to choose from.
