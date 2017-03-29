
use Verilog::Netlist;
use timerDesignInfo;

sub create_nodes_of_graph {
  my $top_mod = shift;
  my $circuit = shift;
  my $circuit_node_count = shift;
  my %node_name_to_id_hash = shift;
  my %net_sink_hash = shift;
  my %net_source_hash = shift;
  my @nets = shift;

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
      create_node_cmd ($circuit, 
		       $node_name, 
		       $top_port->direction, 
		       $circuit_node_count,
		       \%node_name_to_id_hash);
      #KNOWN ISSUE HERE : Can be a problem because 
      #port name and net name are
      #assumed to be the same which may or may not be the case.
      pair_net_node ($is_source, 
		     $node_name, 
		     $node_name, 
		     \%net_sink_hash,
		     \%net_source_hash,
		     \@nets);
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
		         $node_name, 
		         $top_port->direction, 
		         $circuit_node_count,
		         \%node_name_to_id_hash);
        pair_net_node ($is_source, 
		       $node_name, 
		       $node_name, 
		       \%net_sink_hash,
		       \%net_source_hash,
		       \@nets);
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
      create_node_cmd ($circuit, 
		       $node_name, 
		       $port_dir,
		       $circuit_node_count,
		       \%node_name_to_id_hash);
      pair_net_node ($is_source, 
		     $net, 
		     $node_name, 
		     \%net_sink_hash,
		     \%net_source_hash,
		     \@nets);
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
  my %net_sink_hash = shift;
  my %net_source_hash = shift;
  my @nets = shift;
   
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
  my $circuit = shift;
  my $node_name_to_id_hash = shift;
  my %net_sink_hash = shift;
  my %net_source_hash = shift;
  my @nets = shift;
  foreach my $net (@nets) {
    foreach my $source (@{$net_source_hash{$net}}) {
      foreach my $sink (@{$net_sink_hash{$net}}) {
        create_edge_cmd ($circuit, $source, $sink, $node_name_to_id_hash);
      }
    }
  }	  
}

sub get_cell_arcs {
   print "The inputs are - @_[0] @_[1] @_[2]\n";
}

sub create_timing_arcs {
  my  $top_mod = shift;	
  my  $circuit = shift;	
  foreach my $modcell ($top_mod->cells) {
    my $node_name = "";
    my $is_source;
    foreach my $cell_pin ($modcell->pins) {
      get_cell_arcs ($modcell->name, $modcell->submodname, $cell_pin->name);
    }
  }
}

sub create_edges_of_graph {
  my  $top_mod = shift;	
  my $circuit = shift;
  my %node_name_to_id_hash = shift;
  my %net_sink_hash = shift;
  my %net_source_hash = shift;
  my @nets = shift;
  create_interconnect_arcs ($circuit,
		  	    \%node_name_to_id_hash,
		    	    \%net_sink_hash,
			    \%net_source_hash,
			    \@nets);
  create_timing_arcs ($top_mod, $circuit);
}

sub create_node_cmd {
	my $circuit = shift;
	my $node_name = shift;
        my $direction = shift;
        my $circuit_node_count = shift;	
        my $node_name_to_id_hash = shift;	
	print "Create node -> $node_name dir --> $direction\n";
	my $id = timerDesignInfo::add_pin ($circuit, $node_name, $circuit_node_count);
 	timerDesignInfo::add_pin_direction ($circuit, $id, $direction);
	$node_name_to_id_hash{$node_name} = $id;
	$circuit_node_count = $circuit_node_count + 1;
}

sub create_edge_cmd {
	my $circuit = shift;
	my $source = shift;
	my $sink = shift;
	my $node_name_to_id_hash = shift;
	print "Create Edge $source --> $sink\n";
	my $src = $node_name_to_id_hash{$source};
	my $snk = $node_name_to_id_hash{$sink};
 	timerDesignInfo::add_timing_arc ($circuit, $src, $snk);
}

sub read_verilog_netlist {
  my $verilog_file = shift;
  my $nl = new Verilog::Netlist;

  foreach my $file ($verilog_file) {
    $nl->read_file (filename => $file);
  }
  $nl->link ();
  $nl->lint ();
  $nl->exit_if_error ();

  my $top_mod = $nl->find_module ("top");

  my $circuit = timerDesignInfo::get_or_create_circuit ($top_mod->name); 

  my $circuit_node_count = 0;

  my %node_name_to_id_hash = ();
  my %net_sink_hash = ();
  my %net_source_hash = ();
  my @nets = (); 

  create_nodes_of_graph ($top_mod, 
		  	 $circuit, 
			 $circuit_node_count,
			 \%node_name_to_id_hash,
			 \%net_sink_hash,
			 \%net_source_hash,
			 \@nets);

  create_edges_of_graph ($top_mod,
		         $circuit,
			 \%node_name_to_id_hash,
			 \%net_sink_hash,
			 \%net_source_hash,
			 \@nets);
}

read_verilog_netlist ( $ARGV[0] );

