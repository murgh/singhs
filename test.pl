#! /usr/bin/perl

use strict;
use warnings;
use Switch;

my $local_repo_path = "";
my $testDir = "";
my $runDir = "";
my $test_case_name = "";
my $testList = "";
my $do = "";

sub parse_options {

	for (my $i = 0 ; $i <= $#ARGV; $i++ ) {
		
		switch($ARGV[$i]) {

			case "-local_repo" {
				$i++;
				$local_repo_path = $ARGV[$i];
				chomp ($local_repo_path);
				if ($local_repo_path =~ /^-/) {
					print "Invalid path\n";
					exit;
				}	 
				$testDir = "$local_repo_path/test";
				chomp($testDir);
				$testList = "$testDir/test.list";
				chomp($testList);
			}
			
			case "-add" { 
				$i++;
				$test_case_name = $ARGV[$i];
				chomp ($test_case_name);
				if ($test_case_name =~ /^-/) {
					print "Invalid test case name\n";
					exit;
				} 
				$do = "add_test_case";
			}

			case "-run_test_case" {
				$i++;
				$test_case_name = $ARGV[$i];
				chomp ($test_case_name);
				if ($test_case_name =~ /^-/ ) {
					print "Invalid test case name\n";
					exit;
				}
				$do = "run_test_case";
	                }

			case "-run" {
				$do = "run";
			}	

		}

	}

}

sub main {

	switch($do) {

		case "add_test_case" {
			add_test_case();
			last;
		}
		
		case "run_test_case" {
			if (`grep $test_case_name $testList | wc -l` == 0) {
				print "Invalid test case name\n";
				exit;
			}
			run("one");
			last;
		}
	
		case "run" {
			run("all");
			last;
		}
	}
}

sub add_test_case {

	if ($local_repo_path eq "") {
		print "Specify the path of your local repository\n";
		exit;
	}
	
	if ($test_case_name eq "") {
		print "Specify the name of test case\n";
		exit;
	}       

	if ( -e $testList) {
		if (`grep $test_case_name $testList | wc -l` != 0) {
			print "\"$test_case_name\" already exists. Use another name.\n";
			exit;
		}
	}
	
	print "$testDir\n";	

	system ("mkdir -p $testDir/$test_case_name");

	open ( my $DESCRIPTION_FILE , ">>" , "$testDir/$test_case_name/description" );
	print $DESCRIPTION_FILE "Test Case Name : $test_case_name\n";
	print $DESCRIPTION_FILE "Description : \n";
	
	open ( my $TCL_FILE , ">>" , "$testDir/$test_case_name/$test_case_name.tcl" );
	print $TCL_FILE "load $local_repo_path/mymodule.so\n";

	open ( my $GOLD_FILE , ">" , "$testDir/$test_case_name/Output_gold.txt" );

	open ( my $TEST_LIST , ">>" , $testList );
	print $TEST_LIST "$test_case_name\n";
}

sub run {

	my $diff_result = "";
	my $i = 1;
	my $runDir = "regressions_".`date '+%d-%m-%y_%H-%M-%S'`;
	my $list = "";

	chomp ($runDir);
	system("$local_repo_path/build");

	if ($_[0] eq "all") {
		$list = $testList;
	}
	elsif ($_[0] eq "one") {
		open ( my $TEMP_LIST , ">" , "$testDir/temp.list");
		print $TEMP_LIST "$test_case_name\n";
		$list = "$testDir/temp.list";
	}  
	my $serial = "S.No.";
	my $name = "Test Case Name";
	my $result = "Result";

	print "--------+----------------------------------------------------+---------\n";
	printf ("| %-2s | %-50s | %-6s |\n",$serial,$name,$result);
	print "--------+----------------------------------------------------+---------\n";	

	open ( my $TEST_LIST , "<" , $list );
	
	while (my $test_case = <$TEST_LIST>) {
		
		chomp($test_case);
		system("mkdir -p $runDir/$test_case");
		my $tcl = "$testDir/$test_case/$test_case.tcl";
		system("tclsh $tcl");
                
		system("cp Output.txt $runDir/$test_case/Output.txt");
		system("rm -rf Output.txt");
		if( (! -e "$runDir/$test_case/Output.txt") || (`diff $runDir/$test_case/Output.txt $testDir/$test_case/Output_gold.txt | wc -l` != 0)) {
			system("diff $runDir/$test_case/Output.txt $testDir/$test_case/Output_gold.txt > $runDir/$test_case/$test_case.diff");
			$diff_result = "FAIL";
		}
		else {
			$diff_result = "PASS";
		}

		printf ("| %-5s | %-50s | %-6s |\n" ,$i, $test_case, $diff_result);
	        print "--------+----------------------------------------------------+---------\n";	                
	
		$i = $i + 1;
	}
        
	system("rm -rf '$testDir/temp.list'");     	

} 


		
parse_options();
main();

1;
