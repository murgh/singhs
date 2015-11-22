
set prop [list color weight]

#Testing order independence
create_node -name node1 -graph graph1 
create_node -graph graph1 -name node1 

#Testing typos
create_node -nam node1 -graph graph1
create_node -name node1 -grap graph1

