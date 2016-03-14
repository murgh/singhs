//Sushobhit Singh
//First - Cut No path compression, uses recursion,
//not very efficient.
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <utility>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define N 100000000
#define E 20000000000

using namespace std;
#define undef_id -1

typedef std::pair<std::pair<int, int>, std::pair<int, int> > level_info;
#define make_level_info(L1, I1, L2, I2) \
	std::make_pair (std::make_pair (L1, I1), std::make_pair (L2, I2));

class Deck {
 private:
  std::vector<int *> level_array; 
  std::vector<std::pair<int, int> > flag_tag_array;
  int levels;
  int vertex_count;

 public:

  #define ALLOCATE_LEVEL(L, LC, N)\
     L.resize (1+LC);\
     std::vector<int *>::iterator it = L.begin ();\
     int * k = new int [N];\
     for (int i = N; i--;) k[i] = undef_id;\
     L.insert (it + LC, k);\
     k[0] = 0;

  Deck (int vc) {
    vertex_count = vc;
    levels = -1;
    flag_tag_array.resize (vertex_count + 1); 
    for (int i = 0; i <= vertex_count; i++) {
       flag_tag_array[i].first = undef_id;
       flag_tag_array[i].second = undef_id; 
    }
    std::cout << "Created a DECK of size " << vertex_count << std::endl;
  }

  ~Deck() {
    flag_tag_array.clear ();
    //for (int i = 0; i < levels; i++) delete level_array[i];
    level_array.clear ();
    //std::cout << "Deleted the deck !! " << vertex_count << std::endl;
  }

  /*Add a new level to the Deck*/
  void add_new_level () {
    int level_size = (int) vertex_count / pow (2, ++levels);  
    ALLOCATE_LEVEL (level_array, levels, level_size + 2);
  }

  /* Deck-Union has the following steps : 
     //Bad implementation actually due to recursion, do a non-recursive
     //implementation
   */

  level_info Deck_Union (int ID1, int ID2, int check_level = 0) {
    if (check_level > levels)
      add_new_level (); 
    if (check_level <= levels) {
      int * level_arr = level_array[check_level];
      //Both levels are equal
      if (level_arr[ID1] == level_arr[ID2]) {
	//If both are undefined id then give them a new id.
	//Else keep the same ID, nothing is to be changed
        if (level_arr[ID1] == undef_id) {
          level_arr[ID1] = level_arr[ID2] = ++(level_arr[0]);
	}
	return make_level_info (check_level, level_arr[ID1], check_level, level_arr[ID2]);
      } else {//If both the levels are different
        //If any one of them is undefined ID then copy the
	//defined ID into it
	if (level_arr[ID1] == undef_id) {
	  level_arr[ID1] = level_arr[ID2];
	  return make_level_info (check_level, level_arr[ID1], check_level, level_arr[ID2]);
	}
	if (level_arr[ID2] == undef_id) {
	  level_arr[ID2] = level_arr[ID1];
	  return make_level_info (check_level, level_arr[ID1], check_level, level_arr[ID2]);
	}
	//The condition where none is undefined
	//the costly union case.
	//Go to the next level, and try for Union
	return Deck_Union (level_arr[ID1], level_arr[ID2], check_level+1);
      }
    }
  }

  void Perform_DECK_Union (int ID1, int ID2) {
    level_info l_info = Deck_Union (ID1, ID2); 
    //std::cout << " Union between " << ID1 << " and " << ID2 << std::endl;
    flag_tag_array [ID1] = l_info.first;
    flag_tag_array [ID2] = l_info.second;
  }
  void print_tag_info () {
   for (int i = 1; i <= vertex_count; i++) {
     std::cout << "ID : " << i << 
	          " Level = " << flag_tag_array [i].first << 
	          "Tag = " << flag_tag_array [i].second << std::endl; 
   }
  }
};

int main () {

 Deck d (N);
 int ID1, ID2, i = 1;
 clock_t start,end;
 double duration;
 start=clock();
 while (i < E) {
  ID1 = rand () % N + 1;
  ID2 = rand () % N + 1;
  d.Perform_DECK_Union (ID1, ID2);
  i++;
 }
 end=clock();
 duration = (double)(end - start) / CLOCKS_PER_SEC;
 printf("TIME = %f\n",duration);
 //d.print_tag_info ();
 return 0;
}
