
use Verilog::Netlist;

my $nl = new Verilog::Netlist;

foreach my $file ('smallverilog.v') {
  $nl->read_file (filename => $file);
}
 $nl->link ();
 $nl->lint ();
 $nl->exit_if_error ();

my $top_mod = $nl->find_module ("top");

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
  #create_timing_arcs ();
}

sub create_node_cmd {
	print "Create node -> @_[0]\n";
}

sub create_edge_cmd {
	print "Create Edge @_[0] --> @_[1]\n";
}

create_nodes_of_graph ();
create_edges_of_graph ();
