
set prop [list color weight]
# Order testing
create_node -name node1 -graph graph1 -properties $prop

create_node -properties $prop -name node1 -graph graph1

create_node -graph graph1 -properties $prop -name node1

create_node -graph graph1 -name node1 -properties $prop

create_node -properties $prop -graph graph1 -name node1

create_node -name node1 -properties $prop -graph graph1

#Typo testing
create_node -nam node1 -properties $prop -graph graph1

create_node -name node1 -propertes $prop -graph graph1

create_node -grph graph1 -name node1 -proerties $prop

#Properties Testing
