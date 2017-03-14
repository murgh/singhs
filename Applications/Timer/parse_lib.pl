use Liberty::Parser;
use Inline CPP => config => cc => '/usr/bin/g++';
#use Inline CPP => config => namespace => 'TimeLib';
use Inline CPP;

#my$input_lib_file = $ARGV[0];
my $input_lib_file = "NangateOpenCellLibrary_fast.lib";
my $parser = new Liberty::Parser;
my $library_group = $parser->read_file ($input_lib_file);
my @cell_groups = $parser->get_groups_by_type ($library_group, 'cell');
sub get_cell_arcs {
  my $cell_name = shift; 
  printf "Cell : %s\n", $cell_name;
  my $cell_pointer = TimeLib::test (4);
  foreach $cell (@cell_groups) {
    my $current_cell_name = $parser->get_group_name ($cell);
    chomp ($current_cell_name);
    if ($current_cell_name eq $cell_name) {
     my @cell_pin_group = $parser->get_groups_by_type ($cell, "pin");
     foreach $pin_of_cell (@cell_pin_group) {
       my @timing_groups = $parser->get_groups_by_type ($pin_of_cell, "timing");
       foreach $timing_grp ( @timing_groups ) {
         foreach $other_pin (@cell_pin_group) {
           my $other_pin_name = $parser->get_group_name ($other_pin);
           if ( $parser->check_related_pin ($timing_grp, $other_pin_name) ) {
           }
         }
       }
     }
   }
  }
}

get_cell_arcs ("CLKGATETST_X1"); 

__END__
__CPP__

#include "/home/singhs/work/DiGAna_Development/Dec25Latest/Applications/Timer/timerLibInfo.hxx"

namespace TimeLib {
	int test (int a) { return a*a; }

CellInfo * getOrCreateLibCell (std::string cellName) {
  CellInfo * info = NULL; 
  std::map<std::string, CellInfo*>::iterator itr;
  itr = libraryCells.find (cellName);
  if (itr != libraryCells.end ())
    return libraryCells.find (cellName)->second;  

  info = new CellInfo (cellName);
  libraryCells[cellName] = info;
  return info;
}

PinInfo * getOrCreateLibCellPin (
		       std::string cellName,
		       std::string pinName, 
		       PinDirection direction,
		       float * capacitance,
                       float max_fanout, float min_fanout,
                       float fanout_load,
                       float min_transition, float max_transition,
                       float min_capacitance, float max_capacitance) {
   CellInfo * cell = getOrCreateLibCell (cellName); 	
   PinInfo * pin;
   if ((pin = cell->cellPin (pinName)))
     return pin;
   	
   pin = new PinInfo (cell, direction, pinName, capacitance,
		      max_fanout, min_fanout, fanout_load,
		      min_transition, max_transition, 
		      min_capacitance, max_capacitance);  
   cell->insertPin (pin);
   return pin;
}

TimingArc * getOrCreateTimeArc (PinInfo * start, PinInfo * end,
			 TimingArcType arcType) {
  CellInfo * cell = start->getCell ();	
  TimingArc * arc = cell->getTimeArc (start, end, arcType);
  if (arc)
    return arc;
  arc = new TimingArc (arcType, start, end); 
  cell->insertTimingArc (arc);
  return arc;
}
}
