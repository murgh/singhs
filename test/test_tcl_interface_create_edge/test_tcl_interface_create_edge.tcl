

#Testing order independence
create_edge -graph g -source n -sink n2 
create_edge -graph g -sink n2 -source n 
create_edge -source n -graph g -sink n2 
create_edge -source n -sink n2 -graph g 
create_edge -sink n2 -source n -graph g 
create_edge -sink n2 -graph g -source n 

#Testing typos
create_edge -grah g -source n1 -sink n2 
create_edge -graph g -source n1 -snk n2 
create_edge -graph g -souce n1 -sink n2 


