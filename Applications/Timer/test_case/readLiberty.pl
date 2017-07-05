use Liberty::Parser;

my $lib_cell_count = 0;

sub is_sequential {
  my $parser = shift;
  my $cell = shift;
  my @cell_pin_group = $parser->get_groups_by_type ($cell, "pin");
  foreach $pin_of_cell (@cell_pin_group) {
    my @timing_group = $parser->get_groups_by_type ($pin_of_cell, "timing");	  
    foreach $timing (@timing_group) {
      my $timing_type = $parser->get_simple_attr_value ($timing, 'timing_type');
      if ($timing_type =~ m/setup/ ||
          $timing_type =~ m/hold/) {
	return 1;
      }
    }
  }
  return 0;
}

sub create_cells {
  my $parser = shift;
  my $library_group = shift; 
  my $gnl_file_handle;
  open ($gnl_file_handle, ">master.gnl");
  open ($ver_file_handle, ">master.v");
  print $gnl_file_handle "[library]\nname=lib\n";
  my @cell_groups = $parser->get_groups_by_type ($library_group, 'cell');
  foreach $cell (@cell_groups) {
    my $current_cell_name = $parser->get_group_name ($cell);
    print $ver_file_handle "module $current_cell_name;\n";
    chomp ($current_cell_name);
    my @cell_pin_group = $parser->get_groups_by_type ($cell, "pin");
    my $input_count = 0;
    my $output_count = 0;
    my @input_array = {};
    my @output_array = {};
    foreach $pin_of_cell (@cell_pin_group) {
      my $pin_name = $parser->get_group_name ($pin_of_cell);
      my $pin_direction = $parser->get_simple_attr_value ($pin_of_cell, 'direction');
      if ($pin_direction eq "input") { 
	$input_count = $input_count + 1;
        print $ver_file_handle "input $pin_name;\n";
      } else {
	$output_count = $output_count + 1;
        print $ver_file_handle "output $pin_name;\n";
      }
    }
    print $ver_file_handle "endmodule\n";
    if ($input_count != 0 && $output_count != 0) {
      $lib_cell_count = $lib_cell_count + 1;	    
      if (is_sequential ($parser, $cell)) {
        print $gnl_file_handle "latch=$current_cell_name $input_count $output_count\n"
      } else {
        print $gnl_file_handle "gate=$current_cell_name $input_count $output_count\n"
      }
    }
  }
  close ($gnl_file_handle);
  close ($ver_file_handle);
  return "master.gnl";
}

sub read_liberty {
  my $input_lib_file = shift;

  my $parser = new Liberty::Parser;

  my $library_group = $parser->read_file ($input_lib_file);

  return create_cells ($parser, $library_group);
}

sub create_sub_circuit {
  my $sub_circuit_count = shift;
  my $sub_circuit_size = shift;
  my $gnl_file;
  open ($gnl_file, ">file_$sub_circuit_count.gnl");
  open ($gnl_master, "<master.gnl");
  while (my $row = <$gnl_master>) {
    print $gnl_file $row; 
  }
  close ($gnl_master); 
  print $gnl_file "[circuit]\nname=block_$sub_circuit_count\nlibraries=lib\n";
  print $gnl_file "distribution=";
  my $distribution = int ($sub_circuit_size / $lib_cell_count);
  my $final_total = 0;
  for (my $i = 0; $i < $lib_cell_count; $i = $i + 1) {
    print $gnl_file " $distribution"; 
    $final_total = $final_total + $distribution;
  }  
  my $input = int (.2 * $final_total);
  my $output = int (.1 * $final_total);
  print $gnl_file "\nsize=$final_total\nI=$input\nO=$output\n";
  close ($gnl_file);
  system ("gnl file_$sub_circuit_count.gnl");
}

sub create_circuits {
  my $input_lib_file = shift;
  my $total_inst_count = shift;
  my $sub_circuit_count = shift;

#my $sub_circuit_size = int ($total_inst_count / $sub_circuit_count);
  my $master_gnl = read_liberty ($input_lib_file);
#  for (my $i = 0; $i < $sub_circuit_count; $i = $i + 1) {
#    create_sub_circuit ($i, $sub_circuit_size); 
#  }
}

create_circuits ($ARGV[0], $ARGV[1], $ARGV[2]);
