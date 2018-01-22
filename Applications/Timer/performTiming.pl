#!/usr/bin/perl
use Liberty::Parser;
use Verilog::Netlist;
use timerDesignInfo;

my %node_name_to_id_hash = ();
my %net_sink_hash = ();
my %net_source_hash = ();
my @nets = ();
my $timer_test_case = "";
my $make_file = 0;
my $circuit_node_count = 0;
my @input_ports = ();
my @output_ports = ();
my $verbose = 0;

sub my_print {
  my $string = shift;
  if ($verbose == 0) { 
    return;
  }  
  print "$string\n";
}

sub file_print {
  my $string = shift;
  if ($make_file == 0) {
    return;
  }
  print $timer_test_case $string;
}

sub get_node_id {
  my $node_name = shift;
  my_print $node_name;
  if (exists($node_name_to_id_hash{$node_name})) {
    $node_id = $node_name_to_id_hash{$node_name};
    return $node_id;
  } 
  return -1;
}

sub createClock {
  my $circuit = shift;
  my $constr = shift;

  my @constr_string = split /\s+/, $constr;

  my $clock_name = $constr_string[1];
  my $clock_period = $constr_string[2];
  my $vars = scalar @constr_string;
  if ($vars == 4) {
    my $clock_source = $constr_string[3];
    my $node = get_node_id ($clock_source);
    my_print "Adding clock $clock_name $clock_source $node\n";
    timerDesignInfo::add_clock ($circuit, $clock_name, $clock_period, $node, 0);
  } else {
    my_print "Adding virtual clock $clock_name\n";
    timerDesignInfo::add_clock ($circuit, $clock_name, $clock_period, $node, 1);
  }
}

sub inputDelay {
  my $circuit = shift;
  my $constr = shift;

  my @constr_string = split /\s+/, $constr;

  my $value = $constr_string[1];
  my $source = $constr_string[2];

  my $node = get_node_id ($source);
  my_print "Adding input $node $source\n";
  timerDesignInfo::add_IO_delay ($circuit, $value, $node, 1);
}

sub outputDelay {
  my $circuit = shift;
  my $constr = shift;

  my @constr_string = split /\s+/, $constr;

  my $value = $constr_string[1];
  my $source = $constr_string[2];

  my $node = get_node_id ($source);
  my_print "Adding output $node $source\n";
  timerDesignInfo::add_IO_delay ($circuit, $value, $node, 0);
}

sub read_constr_data {
  my $circuit_name = shift;
  my $constr = shift;
  open my $info, $constr or die "ERROR : Could not open $constr: $!"; 
  
  my $circuit = timerDesignInfo::get_circuit ($circuit_name);
  while (my $line = <$info>) {
    my @constr_string = split /\s+/, $line;
    if ($constr_string[0] eq "createClock") { createClock ($circuit, $line) }
    elsif ($constr_string[0] eq "inputDelay") { inputDelay ($circuit, $line) }
    elsif ($constr_string[0] eq "outputDelay") { outputDelay ($circuit, $line) }
    else { print "Cannot identify the constraint $line\n"}
  }
}
sub create_cells {
  my $parser = shift;
  my $library = shift;
  my $library_group = shift; 
  open ($inter_ver, ">inter.v");
  my @cell_groups = $parser->get_groups_by_type ($library_group, 'cell');
  foreach $cell (@cell_groups) {
    my $current_cell_name = $parser->get_group_name ($cell);
    chomp ($current_cell_name);
    print $inter_ver "module $current_cell_name ();\n";
    $timerCell = timerDesignInfo::add_or_get_cell ($library, $current_cell_name);
    my @cell_pin_group = $parser->get_groups_by_type ($cell, "pin");
    foreach $pin_of_cell (@cell_pin_group) {
      my $pin_name = $parser->get_group_name ($pin_of_cell);
      my $pin_cap_pair = $parser->get_attr_with_value ($pin_of_cell, "capacitance");
      my $pin_dir = $parser->get_simple_attr_value ($pin_of_cell, "direction");
      my $clock = $parser->get_attr_with_value ($pin_of_cell, "clock");
      $timerCellPin = timerDesignInfo::add_or_get_pin ($timerCell, $pin_name);      
      if ($clock =~ /clock/ && $clock =~ /true/) {
        timerDesignInfo::mark_clock ($timerCellPin);
      }	
      $pin_cap = get_capacitance ($pin_cap_pair); 
      my_print $pin_cap;
      timerDesignInfo::add_pin_direction ($timerCellPin, $pin_dir);
      timerDesignInfo::add_pin_cap ($timerCellPin, $pin_cap);
      if ($pin_dir ne "internal") {
        print $inter_ver "  $pin_dir $pin_name;\n"
      }
    }
    print $inter_ver "endmodule\n\n";
  }
  close ($inter_ver);
}

sub get_capacitance {
  my $cap_pin_pair = shift;
  return .4;
}

sub create_cell_arcs {
  my $parser = shift;
  my $library = shift;
  my $library_group = shift; 
  my @cell_groups = $parser->get_groups_by_type ($library_group, 'cell');
  foreach $cell (@cell_groups) {
    my $current_cell_name = $parser->get_group_name ($cell);
    chomp ($current_cell_name);
    $timerCell = timerDesignInfo::add_or_get_cell ($library, $current_cell_name);
    my @cell_pin_group = $parser->get_groups_by_type ($cell, "pin");
    foreach $pin_of_cell (@cell_pin_group) {
      my $pin_name = $parser->get_group_name ($pin_of_cell);
      my @timing_groups = $parser->get_groups_by_type ($pin_of_cell, "timing");
      foreach $timing_group (@timing_groups) {
        $related_pin = $parser->get_simple_attr_value ($timing_group, "related_pin");
        $timing_sense = $parser->get_simple_attr_value ($timing_group, "timing_sense");
	$timing_type = $parser->get_simple_attr_value ($timing_group, "timing_type");

        my_print "Cell $current_cell_name  Src $related_pin Sink $pin_name timing_sense $timing_sense timing_type $timing_type"; 
        $timing_arc = timerDesignInfo::add_timing_arc ($timerCell, $related_pin, $pin_name);
	if ($timing_sense) { 
	  timerDesignInfo::add_timing_sense ($timing_arc, $timing_sense); 	
	}  
	if ($timing_type) {
	  timerDesignInfo::add_timing_type ($timing_arc, $timing_type); 	
	}
	my_print "\n"
      }
    }
  }
}

sub read_liberty {
  my $input_lib_file = shift;
  my $parser = new Liberty::Parser;

  my $library = timerDesignInfo::add_or_get_library ($input_lib_file); 

  my $library_group = $parser->read_file ($input_lib_file);

  create_cells ($parser, $library, $library_group);
  create_cell_arcs ($parser, $library, $library_group);
  return $library;
}

sub compute_node_count {
  my $top_mod = shift;

  my $count = 0;

  foreach my $modcell ($top_mod->cells) {
    foreach my $cell_pin ($modcell->pins) {
      $count++;	  
    }
  }

  foreach my $top_port ($top_mod->ports) {
    my $msb = $top_mod->find_net($top_port->name)->msb;
    my $lsb = $top_mod->find_net($top_port->name)->lsb;
    if ($msb == $lsb) {
	    $count++;
    } else {
        my $tot = $lsb + $msb;
        if ($lsb < $msb) {
          $len = $lsb;
        } else {
          $len = $msb;
        }
        while ($len <= $tot) {
	  $count++;
          $len = $len + 1;
        }
    }
  }

  return $count;
}

#read_liberty ($ARGV[0]);
sub create_nodes_of_graph {
  my $top_mod = shift;
  my $circuit = shift;
  my $liberty = shift;

  my $is_IO = 0;
  my $pin_type = "timerIOPort";
  
  #Go on each of ports of top module and print all the ports
  foreach my $top_port ($top_mod->ports) {
    $is_IO = 1;
    my $msb = $top_mod->find_net($top_port->name)->msb;
    my $lsb = $top_mod->find_net($top_port->name)->lsb;
    my $net = $top_mod->find_net($top_port->name);
    if ($top_port->direction eq "in") {
      $is_source = 1;
    } elsif ($top_port->direction eq "out") {
      $is_source = 0;
    } 
    if ($msb == $lsb) {
      my $node_name = $top_port->name;
      create_node_cmd ($circuit, 
		       $is_IO,
		       $pin_type,
		       $node_name, 
		       $top_port->direction); 
      #KNOWN ISSUE HERE : Can be a problem because 
      #port name and net name are
      #assumed to be the same which may or may not be the case.
      pair_net_node ($is_source, 
		     $net->name, 
		     $node_name); 
    } else {
      my $tot = $lsb + $msb;
      if ($lsb < $msb) {
        $len = $lsb;
      } else {
        $len = $msb;
      }
      while ($len <= $tot) {
        my $node_name = $top_port->name . "[" . $len . "]";
        create_node_cmd ($circuit, 
		         $is_IO,
		         $pin_type,
		         $node_name, 
		         $top_port->direction); 

        pair_net_node ($is_source, 
		       $node_name, 
		       $node_name); 
        $len = $len + 1;
      }
    }
  }

  foreach my $modcell ($top_mod->cells) {
    $is_IO = 0;
    my $node_name = "";
    my $is_source;
    foreach my $cell_pin ($modcell->pins) {
      foreach my $netk ($cell_pin->pinselects) {
        my $net = $netk->netname;
        my $port_name = $cell_pin->portname;
        my $port_dir = find_port_dir ($modcell->submod, $port_name);
        $pin_type = find_pin_type ($liberty, $modcell->submod, $port_name);
        if ($port_dir eq "in") {
  	      $is_source = 0;
        } else {
	      $is_source = 1;
        }
        my $node_name = $modcell->name . "/" . $port_name;
        create_node_cmd ($circuit, 
	  	         $is_IO,
		         $pin_type,
		         $node_name, 
		         $port_dir);

        pair_net_node ($is_source, 
		       $net, 
		       $node_name);
      }
    }
  }	
}

#iterate on all the timing arcs of the cell
#if there is an incoming sequential arc to this pin and its dir is output it is Q pin
#if there is an incoming check arc and its dir is input it is D Pin and if the pin has is clock
#its latch clk pin
#other pins are combinational pins.
sub find_pin_type {
  my $library = shift;
  my $module = shift;
  my $port_name = shift;

  my $timerCell = timerDesignInfo::add_or_get_cell ($library, $module->name);
  return timerDesignInfo::get_pin_type ($timerCell, $port_name);
}

sub find_port_dir {
  my $module = shift;
  my $port_name = shift;

  foreach my $port ($module->ports) {
    if ($port->name eq $port_name) {
      return $port->direction;
    }
  }
}

sub pair_net_node {
  my $is_source = shift;
  my $net_name = shift;
  my $node_name = shift; 
   
  my_print "Pairing Net -> $net_name Pin -> $node_name\n";
  my $found = 0;
  foreach $net (@nets) {
   if ($net eq $net_name) {
     $found = 1;
     last;
   }
  }
  if ($found == 0) {
    push @nets, $net_name;
  }
  if ($is_source == 1) {
	  if ($net_source_hash{$net_name}) {
	    push @{ $net_source_hash{$net_name} }, $node_name;
	  } else {
            $net_source_hash{$net_name} = [$node_name];
	  }
  } else {
	  if ($net_sink_hash{$net_name}) {
	    push @ { $net_sink_hash{$net_name} }, $node_name;
	  } else {
            $net_sink_hash{$net_name} = [$node_name];
	  }
  }
}

sub create_interconnect_arcs {
  my $circuit = shift;

  foreach my $net (@nets) {
    my_print "Net - $net\n";
    foreach my $source (@{$net_source_hash{$net}}) {
      foreach my $sink (@{$net_sink_hash{$net}}) {
        create_edge_cmd ($circuit, $source, $sink, 0, 1);
      }
    }
  }
}

sub get_cell_arcs {
   my $circuit = shift;
   my $modcell = shift;
   my $liberty = shift;
   my $lib_cell_name = $modcell->submodname;
   $timerCell = timerDesignInfo::add_or_get_cell ($liberty, $lib_cell_name);
   foreach my $possible_sink ($modcell->pins) {
     foreach my $possible_source ($modcell->pins) {
	my $modcell_name = $modcell->name;
	my $source = $possible_source->name;
	my $sink = $possible_sink->name;
	$timerArc = timerDesignInfo::get_timing_arc ($timerCell, $source, $sink); 
	if ($timerArc) {
#print "Creating cell arc for $lib_cell_name, $modcell_name/$source, $modcell_name/$sink\n"; 
	   my $source_pin = timerDesignInfo::add_or_get_pin ($timerCell, $source);
           create_edge_cmd ($circuit, "$modcell_name/$source", "$modcell_name/$sink", $timerArc, 0);
	}
     }
   }
}

sub create_timing_arcs {
  my $top_mod = shift;	
  my $circuit = shift;	
  my $liberty = shift;
  foreach my $modcell ($top_mod->cells) {
#my $node_name = "";
#my $is_source;
#foreach my $cell_pin ($modcell->pins) {
#get_cell_arcs ($modcell->name, $modcell->submodname, $cell_pin->name, $liberty);
    get_cell_arcs ($circuit, $modcell, $liberty);
#    }
  }
}

sub create_edges_of_graph {
  my $top_mod = shift;	
  my $circuit = shift;
  my $liberty = shift;

  create_interconnect_arcs ($circuit);
  create_timing_arcs ($top_mod,
		      $circuit,
		      $liberty);
}

sub create_node_cmd {
  my $circuit = shift;
  my $is_IO = shift;
  my $pin_type = shift;
  my $node_name = shift;
  my $direction = shift;

  if ($is_IO == 1) {
   if ($direction eq "in") {
     push @input_nodes, $node_name; 
   } else {
     push @output_nodes, $node_name; 
   } 
  }

  my_print "Create node -> $node_name $circuit_node_count dir --> $direction\n";
  $node_name_to_id_hash{$node_name} = $circuit_node_count;
  if ($make_file == 0) {
    timerDesignInfo::add_pin ($circuit, $node_name, $circuit_node_count);
    timerDesignInfo::add_pin_direction_io ($circuit, $circuit_node_count, $direction, 
		    							$is_IO, $pin_type);
  } else {
    my $clk = ($pin_type eq "timerLatchClock") ? "true" : "false"; 
    my $data = ($clk eq "true") ? "false" : "true";
    my $dir = ($direction eq "in") ? "timerInput" : "timerOutput";
    file_print " prop = new timerPinInfo (\"$node_name\", $clk, $data, $pin_type, $dir);\n";
    file_print " ADD_GRAPH_NODE_AND_PROPERTY (graph, $circuit_node_count, prop);\n";
  }
  $circuit_node_count = $circuit_node_count + 1;
}

sub create_edge_cmd {
  my $circuit = shift;
  my $source = shift;
  my $sink = shift;
  my $timerArc = shift;
  my $is_net = shift;

  my $src = get_node_id ($source);
  my $snk = get_node_id ($sink);
  my_print "Create Edge $source $src --> $sink $snk\n";
  #No self Edge will be created
  if ($src == $snk) {
    my_print "Did not create self edge\n";
    return;
  }
  if ($make_file == 0) {
    if ($is_net) {
      timerDesignInfo::add_timing_arc ($circuit, $src, $snk);
    } else {
      timerDesignInfo::add_timing_arc ($circuit, $src, $snk, $timerArc);
    }
  } else {
    file_print " graph->add_edge ($src, $snk);\n"	  
  }
}

sub pre_process_netlist_file {
  my $verilog_file = shift;

  my $verilog_file_out = "local.v";
  open (my $in_fh, '<', $verilog_file);
  open (my $out_fh, '>', $verilog_file_out);
  print $out_fh "`include <inter.v>\n\n";
  while (my $row = <$in_fh>) {
    if ($row =~ m/module/ && $row !~ m/endmodule/) {
       print $out_fh "module top;\n";	
       next;
    }
    print $out_fh $row;
  }
  close ($in_fh); 
  close ($out_fh); 
  return $verilog_file_out;
}

sub read_verilog_netlist {
  my $liberty = shift;
  my $verilog_file = shift;
  my $nl = new Verilog::Netlist (use_pinselects=>$false);

  my $file = pre_process_netlist_file ($verilog_file);

  #foreach my $file ($verilog_file) {
  $nl->read_file (filename => $file);
  #}
  $nl->link ();
  $nl->lint ();
  $nl->exit_if_error ();

  my $top_mod = $nl->find_module ("top");

  my $node_count = compute_node_count ($top_mod); 

  print "Creating graph with $node_count nodes\n";

  my $circuit = timerDesignInfo::create_circuit ($top_mod->name, $node_count); 

  create_nodes_of_graph ($top_mod, 
		  	 $circuit, 
			 $liberty);

  create_edges_of_graph ($top_mod,
		         $circuit,
			 $liberty);
  return $top_mod->name;
}

sub read_timing_data {
  my $liberty = shift;
  my $netlist = shift;
  my $constr = shift;
  print "Reading Liberty File $liberty .... \n";
  my $liberty_ptr = read_liberty ( $liberty );
  print "Reading Verilog File $netlist .... \n";
  my $circuit = read_verilog_netlist ( $liberty_ptr, $netlist);
  print "Reading Constr File $constr .... \n";
  read_constr_data ($circuit, $constr);
}

sub write_timer_testcase {
  my $liberty = shift;
  my $netlist = shift;
  my $constr = shift;
  if ($make_file == 0) { 
    return;
  }
  file_print "#include \"timerPin.hxx\"\n";
  file_print "#include \"timerEdge.hxx\"\n";
  file_print "#include \"timerUtils.hxx\"\n";
  file_print "#define ADD_GRAPH_NODE_AND_PROPERTY(G, I, P) \\\n";
  file_print "	G->add_vertex (I); \\\n";
  file_print "	V = diganaVertex (I, G); \\\n";
  file_print " 	V.put_property<timerPinProperty> (\"Pin_Property\", timerPinProperty (P));\n\n";
  file_print "extern void perform_timing_analysis (diganaGraph *);\n\n";
  file_print "diganaGraph * create_timing_graph ()\n";
  file_print "{\n";
  file_print " diganaGraphObjectIdentifier id;\n";
  file_print " id.setName (\"Circuit\");;\n";
  file_print " diganaGraph * graph = \n";
  file_print " 	diganaGraphMgr::getGraphMgr ().create_graph (id, diganaDirectedGraphS);\n";
  file_print " graph->register_vertex_property<timerPinProperty> (\"Pin_Property\");\n";
  file_print " graph->register_edge_property<timerArcProperty> (\"Arc_Property\");\n";
  file_print " diganaVertex V;\n";
  file_print " timerPinInfo * prop;\n"; 
  read_timing_data ($liberty, $netlist, $constr);
  file_print " prop = new timerPinInfo (\"V_IN\", false, false, timerPinVirtualNode, timerInput),\n";
  file_print " ADD_GRAPH_NODE_AND_PROPERTY (graph, $circuit_node_count, prop);\n";
  file_print " prop = new timerPinInfo (\"V_OUT\", false, false, timerPinVirtualNode, timerOutput);\n";
  $circuit_node_count = $circuit_node_count + 1;
  file_print " ADD_GRAPH_NODE_AND_PROPERTY (graph, $circuit_node_count, prop);\n";
  file_print " return graph;\n";
  file_print "}\n\n";
  file_print "int main ()\n";
  file_print "{\n diganaGraph * graph = create_timing_graph ();\n";
  file_print " perform_timing_analysis (graph);\n return 0;\n}"; 
}

sub read_report {
  
  my $report = shift;
  if ($report ne "") {
     open (my $fh, '<', $report) 
	     or die "Could not open file '$report'";
     while (my $row = <$fh>) {
	@tokens = split(/\s+/, $row);
	my $from = -1;
	my $through = -1;
	my $to = -1;
	foreach my $token (@tokens) {
	  if ($token eq "From") {
	    $from = -2;
	    next;
	  }
	  if ($token eq "Through") {
	    $through = -2;
	    next;
	  }
	  if ($token eq "To") {
	    $to = -2;
	    next;
	  }
	  if ($from == -2) {
	    $from = get_node_id($token); 
	    next;
          }
	  if ($through == -2) {
	    $through = get_node_id($token); 
	    next;
          }
	  if ($to == -2) {
	    $to = get_node_id($token); 
	    next;
          }
	}		
	timerDesignInfo::addReportObject ($from, $through, $to);
     }
     close $fh;
  }
}

sub perform_timing {
   	
  my $liberty = shift;
  my $netlist = shift;
  my $constr = shift;
  my $report = shift;
  my $algo = shift;
  my $part = shift;
  if ($make_file == 0) { 
	read_timing_data ($liberty, $netlist, $constr);
	read_report ($report); 
	timerDesignInfo::perform_timing_analysis ("top", $algo, $part);
  } else {
	open ($timer_test_case, ">timer_test.cxx"); 
	write_timer_testcase ($liberty, $netlist, $constr);
	close ($timer_test_case);
	system ("g++ ../../Kernel/graph.cpp timerAlgorithms.cxx timer_test.cxx -o timer");
	system ("./timer");
  }
}

sub parse_options {
  my $option_count = 6;
  my $liberty_index = -1;
  my $netlist_index = -1;
  my $constr_index = -1;
  my $report_index = -1;
  my $algorithm_index = -1;
  my $part_index = -1;
  foreach my $i (0 .. $#ARGV) {
    if ($ARGV[$i] eq "-lib") {
      $liberty_index = $i + 1;	
    }    
    if ($ARGV[$i] eq "-netlist") {
      $netlist_index = $i + 1;	
    }
    if ($ARGV[$i] eq "-constr") {
      $constr_index = $i + 1;	
    }
    if ($ARGV[$i] eq "-report") {
      $report_index = $i + 1;	
    }
    if ($ARGV[$i] eq "-algo") {
      $algorithm_index = $i + 1;	
    }
    if ($ARGV[$i] eq "-part") {
      $part_index = $i + 1;	
    }
  }
  if ($liberty_index == -1 || $netlist_index == -1 ||
      $#ARGV < $option_count - 1) {
    print "ERROR : performTiming usage :\n";
    print "performTiming \n-lib <Liberty FIle needed for lib>\n-netlist <Verilog Netlist containing netlist connectivity of the design>\n-constr <Constraint File containing the clock and input delay information>\n-report <file containing commands\n-algo 1(generic)or2(tag_propagation)
    >";    
    return;
  }
  my $netlist = $ARGV[$netlist_index];
  my $liberty = $ARGV[$liberty_index];
  my $constr = $ARGV[$constr_index]; 
  my $report = "";
  if ($report_index != -1) {
    $report = $ARGV[$report_index];
  }  
  my $algo = 2;
  if ($algorithm_index != -1) {
    $algo = $ARGV[$algorithm_index];
  }
  my $part = -1;
  if ($part_index != -1) {
    $part = $ARGV[$part_index];
  }
  perform_timing ($liberty, $netlist, $constr, $report, $algo, $part);
            	
}

parse_options ();
