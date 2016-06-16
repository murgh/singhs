#include <iostream>
#include <algorithm>
#include <map>
#include <utility>
#include "partition.hxx"
#include "../Kernel/graph.hxx"

using namespace std;

int
diganaPartitionMgr::print () {
	std::cout << "Hello I am the partition manager class" << std::endl;
	return 0;
}

class prop : public diganaDynamicGraphProperty {
	int a;
	int b;
};
typedef struct prop prop;

int 
diganaPartitionMgr::test_property_interface () {
  diganaGraphObjectIdentifier id;
  id.setName ("my_graph");
  diganaGraph * graph = diganaGraphMgr::getGraphMgr ().create_graph (id, diganaDirectedGraphS);
  int i = 0;
  while (i < 10) { 
    graph->add_vertex (i); 
    i++;
  }
  //Strike 10 edges
  i = 0;
  while (i < 9) {
    graph->add_edge (i, i+1);
    i++;
  }
  graph->register_vertex_property<int> ("length");
  graph->register_vertex_property<prop> ("cood");
  graph->register_edge_property<double> ("delay");
  i = 0;
  while (i < 9) {
    diganaVertex v(i, graph);
    diganaEdge e(i, i+1, graph);
    v.put_property<int> ("length", i+5);
    e.put_property<double> ("delay", i-10);
   
    i++;
  }
  return 0;}
void 
diganaKL::bipartition ( std::string graph_name) {
     
     (diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second)->register_vertex_property<int> ("partition_id");
     (diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second)->register_vertex_property<bool> ("locked");

     mapNameToGraph::iterator graph_itr_obj = diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name);

 	int vertex_count = diganaGraphMgr::getGraphMgr().getVCount(graph_name);	
	diganaVertex v;
	diganaGraphIterator::vertex_iterator itr, eItr;
   	itr.attach(graph_itr_obj->second);

   	for (; itr != eItr; ++itr) {
    	v = *itr;
	if( v.getVertexId () < vertex_count/2) {
        v.put_property<int>("partition_id", 1);
        v.put_property<bool>("locked", 0);
	}
	else { 
        v.put_property<int>("partition_id", 2);
        v.put_property<bool>("locked", 0);
	}

  	}
	}

void diganaKL::ya(){
std::cout << "hotline bling\n" << std::endl;
}

//get number of adjacent vertices of the given vertex which  are not in the same partition
int diganaKL::get_external_cost(int vertex_id , std::string graph_name){
	diganaGraph* graph = diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second ;
	diganaVertex v ( vertex_id , graph );

	int partition1 = v.get_property<int>("partition_id" );
	int external_cost = 0;
        diganaGraphIterator::adjacency_iterator itr, eItr;
        itr.attach(vertex_id , graph);
        diganaVertex v2;
        for (; itr != eItr; ++itr) {
        v2 = *itr;
//	int vertex_id_2 = v2.getVertexId();
        int partition2 = v2.get_property<int>("partition_id" );
	if(partition1 != partition2)
	external_cost++;
	}

	return external_cost;
}
/*
int diganaKL::get_external_cost(int vertex_id , std::string graph_name){
	diganaGraph* graph = diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second ;
//	diganaVertex v ( vertex_id , graph );
	diganaEdge e ;
	int partition1 = v.get_property<int>("partition_id" );
	int external_cost = 0;
        diganaGraphIterator::in_edge_iterator itr, eItr;
        itr.attach(vertex_id , graph);
        diganaVertex v2;
        for (; itr != eItr; ++itr) {
        v2 = *itr;
	int vertex_id_2 = v2.getVertexId();
        int partition2 = v2.get_property<int>("partition_id" );
	if(partition1 != partition2)
	external_cost++;
	}

	return external_cost;
}
*/
//get number of adjacent vertices in the same partition
int diganaKL::get_internal_cost(int vertex_id , std::string graph_name){
	diganaGraph* graph = diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second ;
	diganaVertex v ( vertex_id , graph );
        int internal_cost = 0;
        
	int partition1 = v.get_property<int>("partition_id" );
        diganaGraphIterator::adjacency_iterator itr, eItr;
        itr.attach(vertex_id , graph);
        diganaVertex v2;
        for (; itr != eItr; ++itr) {
        v2 = *itr;
     //   int vertex_id_2 = v2.getVertexId();
        int partition2 = v2.get_property<int>("partition_id" );
        if(partition1 == partition2)
        internal_cost++;
        }
	return internal_cost;
	}
//benefit of putting the vertex into the other partition
int diganaKL::get_benefit_of_node ( int vertex_id , std::string graph_name ){
	 int external_cost = get_external_cost (vertex_id , graph_name ); 
	 int internal_cost = get_internal_cost (vertex_id , graph_name);
         int benefit = external_cost - internal_cost ;
         return benefit; 
}

//get the gain obtained by swapping two nodes
int diganaKL::gain_value( int vertex_id1 , int vertex_id2 , std::string graph_name ){
	int Da = get_benefit_of_node(vertex_id1 , graph_name );
	int Db = get_benefit_of_node(vertex_id2 , graph_name );
	int Cab = diganaGraphMgr::getGraphMgr().check_if_edge_exists(graph_name , vertex_id1 , vertex_id2 );
 	int gain_value = (Da + Db) - 2*Cab ; 
	return gain_value;
	}

//get max gain by iterating through all unlocked vertex pairs possible
//example 0-0 0-1 0-2 
std::pair<int , int > diganaKL::get_max_gain (std::string graph_name ){

	int max_gain = 0 ,i ,  gain_val ,partition_id_1 , partition_id_2;
	int num_vertices = diganaGraphMgr::getGraphMgr().getVCount(graph_name);
	gain_map vertex_gain_map;
	std::pair<int , int> vertex_pair; 
        std::vector <int>gain_vec;
	diganaGraph* graph = diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second ;
        	
	diganaGraphIterator::vertex_iterator itr, eitr;
        itr.attach(graph);
	diganaVertex v2;
	for( i = 0 ; i < num_vertices ; i++){
	diganaVertex v ( i , graph );
	if( v.get_property<bool>("locked") == 1)
	continue;
	else {

	partition_id_1 = v.get_property<int>("partition_id");
        itr.attach(graph);
        for (; itr != eitr; ++itr) {
        v2 = *itr;
	partition_id_2 = v2.get_property<int>("partition_id");
	if( ( v2.get_property<bool>("locked") == 1 ) ||
		( partition_id_1 == partition_id_2 ) )
	continue;
	else {
	gain_val =  ( gain_value( i , v2.getVertexId() , graph_name) );
	vertex_pair = std::make_pair( i  , v2.getVertexId() );
	std::pair< int , std::pair <int , int > > pair(gain_val , vertex_pair);
	vertex_gain_map.insert(pair);
	gain_vec.push_back(gain_val);	
	}
	}
	}		
	}
	    std::sort(gain_vec.begin(), gain_vec.end());
	    max_gain = gain_vec.back() ;
	  
	    std::map<int , std::pair < int , int > >::iterator gain_map_itr = vertex_gain_map.find(max_gain); 
	    vertex_pair = gain_map_itr->second; 	
	    return vertex_pair;
		}

//self explanatory :p
void diganaKL::swap_node ( int vertex1 , int vertex2 , std::string graph_name ) {

	diganaGraph* graph = diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second ;
	diganaVertex v1 ( vertex1 , graph );
	diganaVertex v2 ( vertex2 , graph );
   	
	int partition1 = v1.get_property<int>("partition_id" );
	int partition2 = v2.get_property<int>("partition_id" );
        v1.put_property<int>("partition_id" , partition2);
        v2.put_property<int>("partition_id" , partition1);
   	v1.put_property<bool>("locked" , 1); 
   	v2.put_property<bool>("locked" , 1); 
	
	

	}

//this is obvious :)					
bool diganaKL::check_all_locked(std::string graph_name) {

	diganaVertex v;
	diganaGraph* graph = diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second ;
	diganaGraphIterator::vertex_iterator itr, eitr;
        itr.attach(graph);
	int num_locked = 0;
	int num_vertices = diganaGraphMgr::getGraphMgr().getVCount(graph_name);
	
        for (; itr != eitr; ++itr) {
        v = *itr;
	if(v.get_property<bool>("locked" ) == true )
	num_locked++;
	}
	if ( num_locked == num_vertices )
	return true;
	if ( (num_locked == (num_vertices-1) ) && (num_vertices%2 != 0 ))
	return true;
	if ( num_locked != num_vertices )
	return false;
// remove this line later 	
return false ;

}

//get no of external edges
int diganaKL::get_cut_set(std::string graph_name) {
	int cut_set = 0;
	diganaGraph* graph = diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second ;
        diganaGraphIterator::edge_iterator itr, eitr;
        itr.attach(graph);
	diganaEdge e;
        for (; itr != eitr; ++itr) {
        e = *itr;
	diganaVertex v1 (e.get_source_id(), graph);
	diganaVertex v2 (e.get_sink_id(), graph);
        if( v1.get_property<int>("partition_id") != 
				v2.get_property<int>("partition_id"))
	cut_set++;
	else 
	continue;
}
       return cut_set;
}


void diganaKL::show_parts (std::string graph_name) {

	diganaGraph* graph = diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second ;
	
	diganaGraphIterator::vertex_iterator itr, eitr;
        itr.attach(graph);
        for (; itr != eitr; ++itr) {
        diganaVertex v2 = *itr;
	cout << "partition id " <<   v2.get_property<int>("partition_id") << " for vertex  " << v2.getVertexId() << "\n" ;
	}
	
}
//get configuration with least cutset
int diganaKL::get_best_config(std::string graph_name) {

	int config = 0 ;
	int cutset;	
	std::vector <int>cut_vec;
	cutset = get_cut_set(graph_name);
	cutset_config_map ccm;
	while(!check_all_locked(graph_name)) {
	std::pair<int , int> vertex_pair = get_max_gain(graph_name);
	swap_node(vertex_pair.first , vertex_pair.second , graph_name);
	cutset = get_cut_set(graph_name);
	if ( cutset == 0 )
	{ return config ;}
	cut_vec.push_back(cutset);
	std::pair<int , int> cut_con = std::make_pair(cutset , config );
//	cout << cutset << " cutset " << config << " config \n" ;
//	std::cout << "cutset for config number " << config << " is " << cutset << "\n" ;
	ccm.insert(cut_con);
	config++;
	}
	    std::sort(cut_vec.begin(), cut_vec.end());
	    std::map<int,int>::iterator it;
	    it = ccm.find(cut_vec.front());
	return  it->second;

}

//get best configuration no , keep swapping until that configuration is achieved
void diganaKL::implement_kl(std::string graph_name) {

	bipartition(graph_name);
	
	diganaGraph* graph = diganaGraphMgr::getGraphMgr().get_graph_through_name(graph_name)->second ;
	diganaVertex v;
	diganaGraphIterator::vertex_iterator itr, eitr;
   	itr.attach(graph);
	int cutset, i ;
	int config_no = get_best_config(graph_name);
 	int vertex_count = diganaGraphMgr::getGraphMgr().getVCount(graph_name);	

   	for (; itr != eitr; ++itr) {
    	v = *itr;
	if( v.getVertexId () < vertex_count/2) {
   	v.put_property<int>( "partition_id" , 1);
   	v.put_property<bool>( "locked" , 0); 
	}
	else { 
        v.put_property<int>("partition_id" , 2);
   	v.put_property<bool>("locked" , 0);
	}
	}

	cutset = get_cut_set(graph_name);
	std::cout << "Initial cutset is   " << cutset << "\n" ;
	if ( cutset == 0 )
	return ;
	for(i = 0 ; i <= config_no ; i++){
	
	std::pair<int , int> vertex_pair = get_max_gain(graph_name);
	swap_node(vertex_pair.first , vertex_pair.second , graph_name);
	cutset = get_cut_set(graph_name);
	} 
//	show_parts(graph_name);
	cutset = get_cut_set(graph_name);
	std::cout << "Final cutset is   " << cutset << "\n" ;
}

// sample implementation 


int main(){

  diganaGraphObjectIdentifier id;
  id.setName ("my_graph");
  diganaGraph * graph = diganaGraphMgr::getGraphMgr ().create_graph (id, diganaUndirectedGraphS);
  int i = 0 , x = 0 ;
  while (i < 500 ) { 
    graph->add_vertex (i); 
    i++;
  }

  while ( x != 250 ) {
    int random_number = std::rand()%250;
    int random_number2 = std::rand()%500;
    graph->add_edge (random_number , random_number2 );
    x++;
}
 diganaKL kl;
 kl.bipartition("my_graph");
 cout <<"Initial cutset  "<< kl.get_cut_set("my_graph") << "\n" ;
 kl.implement_kl ("my_graph");
 cout <<"Final cutset " <<kl.get_cut_set("my_graph") << "\n" ;

return 0;


}						

