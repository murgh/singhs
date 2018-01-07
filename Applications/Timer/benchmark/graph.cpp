#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../../Kernel/graph.hxx"

#define MIN_PER_RANK 20 /* Nodes/Rank: How 'fat' the DAG should be.  */
#define MAX_PER_RANK 30
#define MIN_RANKS 20    /* Ranks: How 'tall' the DAG should be.  */
#define MAX_RANKS 30
#define PERCENT 3     /* Chance of having an Edge.  */
#define FTP_ThP 100
#define DEGREE 20


class node_p_t2 {
public:
	int master;
	int source;
	std::set<int> theCont;
	void init () {
	  master = source = -1;
	  theCont.clear ();
	}
};	

class node_p_t1 {
public:
	int cone_colour; //-1 no color, 1 red, 2 green, 3 blue
	std::list<int> input_list;
	void init () {
	  cone_colour = -1;
	  input_list.clear ();
	}
};

class node_p_t {
public:
	bool is_end_point;
	bool is_start_point;
	node_p_t1 * prop_algo1;
	node_p_t2 * prop_algo2;
	void init () {
	  is_end_point = false;
  	  is_start_point = false;
	  prop_algo1 = new node_p_t1;
          prop_algo2 = new node_p_t2;	  
	  prop_algo1->init ();
	  prop_algo2->init ();
	}
};

class node_p : public diganaDynamicGraphProperty {
	public:
		node_p () :
			theP (NULL) { }
		node_p (node_p_t * pinInfo) :
			theP (pinInfo) { }
		node_p operator = (const node_p & other)
		{
			theP = other.theP;
			return *this;
		}
		node_p_t * getP () const { return theP; }

	private:
		node_p_t * theP;
};

#define get_property(G, V, P) P = (diganaVertex (V, G)).get_property<node_p> ("node_prop");

#define put_property(G, V, P) (diganaVertex (V, G)).put_property<node_p> ("node_prop", node_p (P));

void create_ftp_list (std::list<std::pair<int, int> > & ftp_list, std::vector<int> & start_l, std::vector<int> & end_l) {
  int start_size = start_l.size ();
  int end_size = end_l.size ();

  for (int i = 0; i < FTP_ThP; i++) {
    int start_id = rand () % start_size;
    int end_id = rand () % end_size;
    ftp_list.push_back (std::pair<int, int> (start_l[start_id], end_l[end_id]));
    //printf ("%d %d\n", start_l[start_id], end_l[end_id]);
  }
}

void create_thp_list (std::list<int> & thp_list, int node_count) {
  for (int i = 0; i < FTP_ThP; i++) {
	  thp_list.push_back ( (rand () % node_count) );
  }
}

void forward_mark_cone (diganaGraph * graph, int node, std::list<int> & end_point_list) 
{
  node_p P;
  std::list<int> Q;
  Q.push_back (node);
  while (Q.size () != 0) {
    int cNode = Q.front ();
    Q.pop_front ();
    get_property(graph, cNode, P);
    P.getP ()->prop_algo1->cone_colour += 1;//Red 
    if (P.getP ()->is_end_point)
      end_point_list.push_back (cNode);
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (cNode, graph);
    for (; ai != aietr; ++ai) {
      diganaVertex sink = *ai;
      node_p S;
      //printf ("Looking at %d -> %d\n", cNode, sink.getVertexId ());
      get_property(graph, sink.getVertexId (), S);
      if (S.getP ()->prop_algo1->cone_colour != 0) {
        Q.push_back (sink.getVertexId ());
      } else {
	//printf ("Seems like a loop\n");
      }
    }
  }
}

void backward_mark_cone (diganaGraph * graph, int node, std::list<int> & start_point_list)
{
  node_p P;
  std::list<int> Q;
   Q.push_back (node);
    while (Q.size () != 0) {
      int cNode = Q.front ();
      Q.pop_front ();
      get_property(graph, cNode, P);
      P.getP ()->prop_algo1->cone_colour += 2;//Green 
      if (P.getP ()->is_end_point)
	start_point_list.push_back (cNode);

      std::list<int>::iterator ai = P.getP ()->prop_algo1->input_list.begin (), 
	      	       aietr = P.getP ()->prop_algo1->input_list.end ();	
      for (; ai != aietr; ++ai) {
        Q.push_back (*ai);
      }
  }

}

void clean_marking (diganaGraph * graph)
{
  node_p P;
  diganaGraphIterator::vertex_iterator vitr, eVitr;
  vitr.attach (graph);
  for (; vitr != eVitr; ++vitr) {
    get_property(graph, (*vitr).getVertexId (), P);
    P.getP ()->prop_algo1->cone_colour = -1;
  }
}

void print_path (std::list<int> & paths) 
{
  std::list<int>::iterator itr = paths.begin ();
  for (; itr != paths.end (); ++itr) {
     printf ("%d --> ", *itr);
  }
  printf ("END\n");

}

void PerformDFS (diganaGraph * graph, int node, std::list<int> & paths)
{
  node_p P;
  get_property(graph, node, P); 
  if (P.getP ()->is_end_point) {
    print_path (paths);
    return;
  }
  diganaGraphIterator::adjacency_iterator ai , aietr;
  ai.attach (node, graph);
  for (; ai != aietr; ++ai) {
    get_property(graph, (*ai).getVertexId (), P);
    if (P.getP ()->prop_algo1->cone_colour == 2) {
      paths.push_back ((*ai).getVertexId ());
      PerformDFS (graph, (*ai).getVertexId (), paths);
    }      
  } 
  if (paths.size () != 0)
    paths.pop_back ();
}

void
algo1_FTP (diganaGraph * graph, std::list<std::pair<int, int> > & FTP)
{
  std::list<int> dummy, paths;
  std::list<std::pair<int, int> >::iterator itr;
  for (itr = FTP.begin (); itr != FTP.end (); ++itr) {
    printf ("Iter ..\n");
    std::pair<int, int> pairp = *itr;
    clean_marking (graph);
    //printf ("Forward Mark\n");
    forward_mark_cone (graph, pairp.first, dummy);
    //printf ("Backward Mark\n");
    backward_mark_cone (graph, pairp.second, dummy);
    PerformDFS (graph, pairp.first, paths);
    paths.clear ();
    dummy.clear (); 
  }  
}


int main (void)
{
	diganaGraphObjectIdentifier id;
	id.setName ("Graph");
	std::list<std::pair<int, int> > edges;
	int i, j, k,nodes = 0;
	srand (time (NULL));

	int ranks = MIN_RANKS
		+ (rand () % (MAX_RANKS - MIN_RANKS + 1));
	printf ("Starting with %d ranks\n", ranks);

	for (i = 0; i < ranks; i++)
	{
		/* New nodes of 'higher' rank than all nodes generated till now.  */
		int new_nodes = MIN_PER_RANK
			+ (rand () % (MAX_PER_RANK - MIN_PER_RANK + 1));

		/* Edges from old nodes ('nodes') to new ones ('new_nodes').  */
		for (j = 0; j < nodes; j++)
			for (k = 0; k < new_nodes; k++)
				if ( (rand () % 100) < PERCENT) { 
					edges.push_back (std::pair<int, int> (j, k + nodes));
				}
		nodes += new_nodes; /* Accumulate into old node set.  */
	}
	printf ("Creating graph with %d nodes and %lu edges\n", nodes, edges.size ());
	bool * start = new bool [nodes];
	bool * end = new bool [nodes];
	int * degree = new int [nodes];
	memset (start, true, nodes);
	memset (end, true, nodes);
	memset (degree, 0, nodes); 
	diganaGraph * graph = diganaGraphMgr::getGraphMgr ().create_graph (id, diganaDirectedGraphS, nodes); 
	std::vector<int> start_l, end_l;
	graph->register_vertex_property <node_p> ("node_prop");
        diganaGraphIterator::vertex_iterator vitr, eVitr;
        vitr.attach (graph);
        for (; vitr != eVitr; ++vitr) {
	  node_p_t * P = new node_p_t;
	  P->init ();
          put_property(graph, (*vitr).getVertexId (), P);
        }

	std::list<std::pair<int, int> >::iterator itr = edges.begin ();
	for (itr = edges.begin (); itr != edges.end (); ++itr) {
		std::pair<int, int> p = *itr;
		//if (degree[p.first] == DEGREE) 
		  //continue;
		graph->add_edge (p.first, p.second);
		start[p.second] = false;
		end[p.first] = false;
		node_p sinkP;
		get_property (graph, p.second, sinkP);
		node_p_t * P = sinkP.getP ();
		P->prop_algo1->input_list.push_back (p.first);
		degree[p.first]++;
	}

        for (i = 0; i < nodes; i++) {
		node_p P;
		get_property (graph, i, P);
		if (start[i]) {
		  start_l.push_back (i);
		  P.getP ()->is_start_point = true;
                }

		if (end[i]) {
		  end_l.push_back (i);
		  P.getP ()->is_end_point = false;
		}
	}	

	std::list<std::pair<int, int> > FTP;
	std::list<int> ThP;
	create_ftp_list (FTP, start_l, end_l);
	create_thp_list (ThP, nodes);
	printf ("Done with graph creation start_points %lu end_points %lu\n", start_l.size (), end_l.size ());
	algo1_FTP (graph, FTP);
	//algo_2 (graph, start_l, end_l, node);
	return 0;
}

