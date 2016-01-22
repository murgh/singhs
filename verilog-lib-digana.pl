use Verilog::Netlist;
use Liberty::Parser;

my $delimiter = " ";

#Node creation commands
my $node_create_cmd = "create_node";
my $node_name_option = "-name";

#Common graph
my $graph_name = "myGraph";
my $graph_option = "-graph";

#Edge creation commands
my $edge_create_cmd = "create_edge";
my $edge_source = "-source";
my $edge_sink = "-sink";

my %net_sink_hash;  
my %net_source_hash; 
my @nets;


#my$input_file = $ARGV[0];
my $input_lib_file = $ARGV[0];
my $parser = new Liberty::Parser;
my $library_group = $parser->read_file ($input_lib_file);
my @cell_groups = $parser->get_groups_by_type ($library_group, 'cell');

#Get the arcs from the library file
sub get_cell_arcs {
  my $module_name = shift;
  my $cell_name = shift;
  my $pin_name = shift;
  foreach $cell (@cell_groups) {
    my $current_cell_name = $parser->get_group_name ($cell);
    chomp ($current_cell_name);
    if ($current_cell_name eq $cell_name) {
      my @cell_pin_group = $parser->get_groups_by_type ($cell, "pin");
      foreach $pin_of_cell (@cell_pin_group) {
	my $current_pin = $parser->get_group_name ($pin_of_cell);
	if ($current_pin eq $pin_name) {
	  my @timing_groups = $parser->get_groups_by_type ($pin_of_cell, "timing");
	  foreach $timing_grp ( @timing_groups ) {
            foreach $other_pin (@cell_pin_group) {
              my $other_pin_name = $parser->get_group_name ($other_pin);
	      if ( $parser->check_related_pin ($timing_grp, $other_pin_name) ) {
                create_edge_cmd ($module_name . "/" . $other_pin_name, 
				 $module_name . "/" . $current_pin);
	      }
	    }
	  }
	}
      }
    }
  }
}

sub create_edge_cmd {
  my $source = shift;
  my $sink = shift;

  my $cmd = $edge_create_cmd .
	    $delimiter .
	    $graph_option .
	    $delimiter .
	    $graph_name .
	    $delimiter .
            $edge_source .
	    $delimiter .
	    $source .
	    $delimiter .
	    $edge_sink .
	    $delimiter .
	    $sink . "\n";

 print $cmd;
}

sub create_node_cmd {
  my $node_name = shift;
 
  my $outfile = "out.digana";
  my $cmd = $node_create_cmd . 
	    $delimiter .
	    $graph_option .
	    $delimiter .
	    $graph_name .
	    $delimiter .
	    $node_name_option .
	    $delimiter .
	    $node_name . "\n";
  print $cmd;
}

my $nl = new Verilog::Netlist;

foreach my $file ($ARGV[1]) {
  $nl->read_file (filename => $file);
}
 $nl->link ();
 $nl->lint ();
 $nl->exit_if_error ();

my $top_mod = $nl->find_module ("test");

sub create_nodes_of_graph {
  #Go on each of ports of top module and print all the ports
  foreach my $top_port ($top_mod->ports) {
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
      create_node_cmd ($node_name);
      #KNOWN ISSUE HERE : Can be a problem because 
      #port name and net name are
      #assumed to be the same which may or may not be the case.
      pair_net_node ($is_source, $node_name, $node_name);
    } else {
      my $tot = $lsb + $msb;
      if ($lsb < $msb) {
        $len = $lsb;
      } else {
        $len = $msb;
      }
      while ($len <= $tot) {
        my $node_name = $top_port->name . "[" . $len . "]";
        create_node_cmd ($node_name);
        pair_net_node ($is_source, $node_name, $node_name);
        $len = $len + 1;
      }
    }
  }

  foreach my $modcell ($top_mod->cells) {
    my $node_name = "";
    my $is_source;
    foreach my $cell_pin ($modcell->pins) {
      my $net = $cell_pin->netname;
      my $port_name = $cell_pin->portname;
      my $port_dir = find_port_dir ($modcell->submod, $port_name);
      if ($port_dir eq "in") {
	      $is_source = 0;
      } else {
	      $is_source = 1;
      }
      my $node_name = $modcell->name . "/" . $port_name;
      create_node_cmd ($node_name);
      pair_net_node ($is_source, $net, $node_name);
    }
  }	
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
	    push $net_source_hash{$net_name}, $node_name;
	  } else {
            $net_source_hash{$net_name} = [$node_name];
	  }
  } else {
	  if ($net_sink_hash{$net_name}) {
	    push $net_sink_hash{$net_name}, $node_name;
	  } else {
            $net_sink_hash{$net_name} = [$node_name];
	  }
  }
}

sub create_interconnect_arcs {
  foreach my $net (@nets) {
    foreach my $source (@{$net_source_hash{$net}}) {
      foreach my $sink (@{$net_sink_hash{$net}}) {
        create_edge_cmd ($source, $sink);
      }
    }
  }	  
}

sub create_timing_arcs {
  foreach my $modcell ($top_mod->cells) {
    my $node_name = "";
    my $is_source;
    foreach my $cell_pin ($modcell->pins) {
      get_cell_arcs ($modcell->name, $modcell->submodname, $cell_pin->name);
    }
  }
}

sub create_edges_of_graph {
  create_interconnect_arcs ();
  create_timing_arcs ();
}

create_nodes_of_graph ();
create_edges_of_graph ();
