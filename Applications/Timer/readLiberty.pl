
use Liberty::Parser;
use timerDesignInfo;

my $input_lib_file = $ARGV[0];
my $parser = new Liberty::Parser;
my $library_group = $parser->read_file ($input_lib_file);
my @cell_groups = $parser->get_groups_by_type ($library_group, 'cell');

my $library = timerDesignInfo::add_or_get_library ($input_lib_file); 

sub create_cells {
  foreach $cell (@cell_groups) {
    my $current_cell_name = $parser->get_group_name ($cell);
    chomp ($current_cell_name);
    $timerCell = timerDesignInfo::add_or_get_cell ($library, $current_cell_name);
    my @cell_pin_group = $parser->get_groups_by_type ($cell, "pin");
    foreach $pin_of_cell (@cell_pin_group) {
      my $pin_name = $parser->get_group_name ($pin_of_cell);
      my $pin_cap = $parser->get_simple_attr_value ($pin_of_cell, "capacitance");
      my $pin_dir = $parser->get_simple_attr_value ($pin_of_cell, "direction");
      $timerCellPin = timerDesignInfo::add_or_get_pin ($timerCell, $pin_name);      
      timerDesignInfo::add_pin_direction ($timerCellPin, $pin_dir);
      timerDesignInfo::add_pin_cap ($timerCellPin, $pin_cap);
    }
  }
}

sub create_cell_arcs {
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
	print "Cell $current_cell_name  Src $related_pin Sink $pin_name timing_sense $timing_sense timing_type $timing_type\n"; 
        $timing_arc = timerDesignInfo::add_timing_arc ($timerCell, $related_pin, $pin_name);
	if ($timing_sense) { 
	  timerDesignInfo::add_timing_sense ($timing_arc, $timing_sense); 	
	}  
	if ($timing_type) {
	  timerDesignInfo::add_timing_type ($timing_arc, $timing_type); 	
	}
      }
    }
  }
}

create_cells ();
create_cell_arcs ();
