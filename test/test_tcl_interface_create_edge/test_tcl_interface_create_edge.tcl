set prop [list color weight]

set graph "-graph g1"
set source "-source n1"
set sink "-sink n2"

#Order Testing
puts "create_edge -graph g -source n -sink n2 -properties $prop"
create_edge -graph g -source n -sink n2 -properties $prop
create_edge -graph g -source n -properties $prop -sink n2
create_edge -graph g -sink n2 -source n -properties $prop
create_edge -graph g -sink n2 -properties $prop -source n
create_edge -graph g -properties $prop -source n -sink n2
create_edge -graph g -properties $prop -sink n2 -source n
create_edge -source n -graph g -sink n2 -properties $prop
create_edge -source n -graph g -properties $prop -sink n2
create_edge -source n -sink n2 -graph g -properties $prop
create_edge -source n -sink n2 -properties $prop -graph g
create_edge -source n -properties $prop -sink n2 -graph g
create_edge -source n -properties $prop -graph g -sink n2
create_edge -sink n2 -source n -graph g -properties $prop
create_edge -sink n2 -source n -properties $prop -graph g
create_edge -sink n2 -graph g -source n -properties $prop
create_edge -sink n2 -graph g -properties $prop -source n
create_edge -sink n2 -properties $prop -source n -graph g
create_edge -sink n2 -properties $prop -graph g -source n
create_edge -properties $prop -graph g -source n -sink n2
create_edge -properties $prop -graph g -sink n2 -source n
create_edge -properties $prop -source n -graph g -sink n2
create_edge -properties $prop -source n -sink n2 -graph g
create_edge -properties $prop -sink n2 -source n -graph g

#Typo Testing
create_edge -grah g -source n1 -sink n2 -properties $prop
create_edge -graph g -source n1 -snk n2 -properties $prop
create_edge -graph g -souce n1 -sink n2 -properties $prop
create_edge -graph g -source n1 -sink n2 -propeties $prop


