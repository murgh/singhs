#---- Straight forward testing----
create_graph -name g -type directional
create_node -name a -graph g 
create_node -name b -graph g
create_edge -graph g -source 0 -sink 1

# ----Negative Testing----

# two graphs at the same instance
create_graph -name x -type undirectional 
# creating same graph again
create_graph -name g -type undirectional
# creating same node again
create_node -name a -graph g
# creating node in a graph which doesn't exist
create_node -name c -graph n
# creating edge in a graph which doesn't exist
create_edge -graph n -source 0 -sink 1
# creating same edge again
create_edge -graph g -source 0 -sink 1
# creating edge between vertices which don't exist
create_edge -graph g -source 2 -sink 3

