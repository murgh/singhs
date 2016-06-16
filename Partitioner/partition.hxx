/***************************************
 * The code for partitioning module of 
 * DiGAna
 *
 * 
 ***************************************/

 #ifndef PARTITION
 #define PARTITION
#include <map>
 class diganaPartitionMgr {
   public :
     int print ();
     int test_property_interface ();
 }; 

 typedef std::map<  int , std::pair<int , int > > gain_map;    
 typedef std::map< int , int > cutset_config_map; 


 class diganaKL {
 public :
 void ya();
 void bipartition(std::string);
 int get_internal_cost(int  vertex_id , std::string);
 int get_external_cost(int  vertex_id , std::string);
 int get_benefit_of_node(int , std::string);
 int gain_value(int , int , std::string);
 std::pair<int , int> get_max_gain (std::string);
 bool check_all_locked(std::string);
 void mark_edges(std::string);
 void swap_node (int , int , std::string);
 int get_cut_set (std::string);
 int get_best_config(std::string);
 void implement_kl(std::string);
 void show_parts ( std::string );
 };
 #endif
