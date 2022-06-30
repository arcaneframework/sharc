#!/usr/bin/env perl

#------------------------------------------------------------
# Run a convergence test case and store the result in the 
# standard output format for ArcGeoSim tests
#------------------------------------------------------------

BEGIN {
  my $base = $ENV{'PERLBASE'};
  @INC = (@INC,$base);
  for my $f1 (glob("$base/*")) {
    if (-d $f1) {
      for my $f2 (glob("$f1/*")) {
	@INC = (@INC,$f2) if (-d $f2);
      }
    }
  }
}

use warnings;
use strict;
use XML::DOM;
use IO::Handle;
use IO::Pipe;
#use Template;
#use Template::Stash::XS;
use Getopt::Long;

#------------------------------------------------------------
# Predeclarations
#------------------------------------------------------------

# Print test information
sub print_test_information();

# Retrieve indicators from test output
sub retrieve_indicators($$);
# Compute convergence ratios
sub compute_ratios($$$$);
# Compute convergence orders
sub compute_orders($$$$);

# Write Arcane problem descriptor
sub arc_writer($$$$$);
# Write results to xml format
sub xml_writer($$);
# Write LaTeX table's headline
sub write_latex_headline($$$$);
# Write one line of LaTeX table
sub write_latex_line($$$$$$);
# Write LaTeX table's footer
sub write_latex_footer($);

# Describe usage
sub usage();

#------------------------------------------------------------
# Global variables
#------------------------------------------------------------

my $separator = "------------------------------";

#------------------------------------------------------------
# Main program
#------------------------------------------------------------

# Read arguments
my $directory_file = shift or usage();
my $test_file      = shift or usage();
my $project_dir;
my $output_dir;
my $executable;
my $num_proc;
my $codename = "ArcGeoSim" ;
#my $config = $ENV{ARCANE_USER_CONFIG_FILE_NAME};
GetOptions("project-dir:s" => \$project_dir,
           "output-dir:s"  => \$output_dir,
           "executable:s"  => \$executable,
           "num-proc:i"    => \$num_proc,
           "codename:s"    => \$codename);
$num_proc = 1 if (!$num_proc);

die "Parallel tests cannot be run without mpi" if($num_proc > 1 && !$ENV{'ARCANE_PARALLEL_SERVICE'});

die "Cannot open $test_file"      unless -f "$test_file";
die "Cannot open $directory_file" unless -f "$directory_file";

# Parse directory and test file
my $parser      = new XML::DOM::Parser;
my $tests       = $parser->parsefile("$test_file") or die "XML::DOM::Parser::ERROR\n";
my $directories = $parser->parsefile("$directory_file") or die "XML::DOM::Parser::ERROR\n";

# Read directories
my @directories = $directories->getElementsByTagName("directories");

my $arc_template_dir = $directories[0]->getAttributeNode('arc_template')->getValue();
my $out_template_dir = $directories[0]->getAttributeNode('out_template')->getValue();
my $meshes_dir       = $directories[0]->getAttributeNode('meshes')->getValue();
my $tex_output_dir   = $directories[0]->getAttributeNode('tex_output')->getValue();
$tex_output_dir      = "$output_dir/$tex_output_dir";
my $xml_output_dir   = $directories[0]->getAttributeNode('xml_output')->getValue();
$xml_output_dir      = "$output_dir/$xml_output_dir";
my $arc_output_dir   = $directories[0]->getAttributeNode('arc_output')->getValue();
$arc_output_dir      = "$output_dir/$arc_output_dir";

# Read test information
my @test_nodes = $tests->getElementsByTagName("test");
scalar(@test_nodes) or die "No test node defined in $test_file\n";
scalar(@test_nodes) == 1 or die "Multiple tests defined in $test_file\n";

# Read the name of the test
my $node = ($test_nodes[0]->getElementsByTagName("description"))[0];
$node    = ($node->getElementsByTagName("name"))[0]->getLastChild();
my $name = $node->getNodeValue();

# Read the mesh list
my $parameters = ($test_nodes[0]->getElementsByTagName("parameters"))[0];
$node = ($parameters->getElementsByTagName("meshes"))[0]->getLastChild();
my @meshes = split /\s+/, $node->getNodeValue();

# Read Arcane template file name
$node = ($parameters->getElementsByTagName("arcane-template"))[0]->getLastChild();
my $arcane_template = $node->getNodeValue();

# Check if perturbations have to be added and read radii if necessary
$node = ($parameters->getElementsByTagName("add-random-perturbation"))[0];
my $add_random_perturbation = $node->getAttributeNode("value")->getValue();
$node = ($parameters->getElementsByTagName("radii"))[0]->getLastChild();
my @perturbation_radii = split /\s+/, $node->getNodeValue();

# Read indicators, ratios and orders
$node = ($parameters->getElementsByTagName("indicators"))[0]->getLastChild();
my @indicators;
@indicators = split /\s+/, $node->getNodeValue() if($node);

$node = ($parameters->getElementsByTagName("ratios"))[0]->getLastChild();
my @ratios;
@ratios = split /\s+/, $node->getNodeValue() if($node);

$node = ($parameters->getElementsByTagName("orders"))[0]->getLastChild();
my @orders;
@orders = split /\s+/, $node->getNodeValue() if($node);

my %mesh2perturbation_radius;
for(my $i = 0; $i < scalar(@meshes); $i++) {
  $mesh2perturbation_radius{$meshes[$i]} = $perturbation_radii[$i];
}

die "Number of perturbation radii differs from number of meshes" unless scalar(@meshes) == scalar(@perturbation_radii);
die "Perturbation required but movemesh section is not present in $arcane_template" if ($add_random_perturbation =~ m/yes|true/ && !`grep "movemesh" $arc_template_dir/$arcane_template`);
die "Perturbation not required but movemesh section is present in $arcane_template" if ($add_random_perturbation =~ m/no|false/ &&  `grep "movemesh" $arc_template_dir/$arcane_template`);

# Read output file name
$node = ($parameters->getElementsByTagName("output-template"))[0]->getLastChild();
my $output_template = $node->getNodeValue();

# Read TeX output flag
$node = ($parameters->getElementsByTagName("tex-output"))[0]->getLastChild();
my $tex_output_flag = $node->getNodeValue();

my $xml = $name."\.xml";
my $tex = "$name\.tex";

# Print test information
print_test_information();

# Open TeX file if required
my $tex_handle = new IO::File;
if($tex_output_flag =~ m/yes|true/) {
  $tex_handle->open(">$tex_output_dir\/$tex") or die "Failed to create $tex_output_dir\/$tex";
  write_latex_headline($tex_handle, \@indicators, \@orders, \@ratios);
}

# Execute the test
print "$separator\nExecuting test\n";
my $nmesh = 0;
my %old_values;
foreach my $mesh (@meshes) {
  print "$separator\non       : $mesh\n$separator\n";

  my $arcane_filename = $arcane_template;
  $arcane_filename =~ s/template/$mesh/g;
  $arcane_filename =~ s/\.vt2//g;
  arc_writer($meshes_dir,
             $mesh,
             $mesh2perturbation_radius{$mesh},
             "$arc_template_dir/$arcane_template",
             "$arc_output_dir/$arcane_filename");

  my $command = "$project_dir/common/ArcaneInfra/bin/run.pl $arc_output_dir/$arcane_filename --executable=$executable --num-proc=$num_proc --parallel=Mpi --codename=$codename --quiet";
  open(LOG, "$command |") or die "Could not run $command";
  my @log_content = <LOG>;
  print foreach @log_content;

  my %values = retrieve_indicators(\@log_content, \@indicators);
  foreach $_ (@indicators) {
    if (!defined($values{$_})) {
#       print foreach @log_content;
#       die "Indicator $_ could not be found\n";
    }
  }

  my %ratio_values;
  $ratio_values{$_} = '--' foreach @ratios;
  my %order_values;
  $order_values{$_} = '--' foreach @orders;

  if ($nmesh > 0) {
    compute_orders(\@orders, \%values, \%old_values, \%order_values);
    compute_ratios(\@ratios, \%values, \%old_values, \%ratio_values);
  }

  if($tex_output_flag =~ m/yes|true/) {
    write_latex_line($tex_handle, $mesh, \@indicators, \%values, \%order_values, \%ratio_values);
  }

  $nmesh++;
  %old_values = %values;
}

# Write output to xml file
# xml_writer();

# Write output to LaTeX file if required
if($tex_output_flag =~ m/yes|true/) {
  write_latex_footer($tex_handle);
  $tex_handle->close();
}
print "Done\n";

#------------------------------------------------------------
# Subroutines
#------------------------------------------------------------

sub print_test_information() {
  print "$separator\nDirectories\n$separator\n";
  print "arc      : $arc_template_dir\n";
  print "out      : $out_template_dir\n";
  print "meshes   : $meshes_dir\n";
  print "tex      : $tex_output_dir\n";
  print "xml      : $xml_output_dir\n";
  print "exe      : $executable\n";

  print "$separator\nTest information\n$separator\n";
  print "name     : $name\n";
  print "meshes   : ", join(' ', @meshes), "\n";
  print "rand pert: $add_random_perturbation\n";
  print "template : $arcane_template\n";
  print "tex out  : $tex_output_flag\n";
  print "num proc : $num_proc\n";
}

#------------------------------------------------------------

sub retrieve_indicators($$) {
  my ($log_content, $indicators) = @_;
  my %values;
  my $number = '[+-]?\ *(\d+(\.\d*)?|\.\d+)([eE][+-]?\d+)?';
  foreach (@{$log_content}) {
    my $line = $_;
    foreach (@{$indicators}) {
      if ($line =~ m/$_\s*\:\s*($number|nan)/) {
	$values{$_} = $1;
      } 
    }
  }
  return %values;
}

#------------------------------------------------------------

sub compute_orders($$$$) {
    my ($orders, $values, $old_values, $order_values) = @_;
    foreach(@{$orders}) {
        $order_values->{$_} = log($old_values->{$_}/$values->{$_}) / log($old_values->{"h"}/$values->{"h"});
    }
}

#------------------------------------------------------------

sub compute_ratios($$$$) {
  my ($ratios, $values, $old_values, $ratio_values) = @_;
  foreach (@{$ratios}) {
    $ratio_values->{$_} = 2 * log($old_values->{$_}/$values->{$_}) / log($values->{"nunkw"}/$old_values->{"nunkw"});
  }
}

#------------------------------------------------------------

sub arc_writer($$$$$) {
    my($meshes_dir, $mesh, $perturbation_radius, $template_filename, $output_filename) = @_;
    my %replacements = ();
    $replacements{mesh} = "$meshes_dir\/$mesh";
    $replacements{perturbation_radius} = $perturbation_radius;
    open(IN, $template_filename) or die "Failed to open $template_filename";
    open(OUT, ">$output_filename") or die "Failed to create $output_filename";
    while(<IN>) {
	s/\[\%\s*(\w+)\s*\%\]/$replacements{$1}/g; # if $replacements{1};
	print OUT;
    }
    close(IN);
    close(OUT);
}

#------------------------------------------------------------

sub xml_writer($$) {
}

#------------------------------------------------------------

sub write_latex_headline($$$$) {
    my ($fh, $indicators, $orders, $ratios) = @_;
    $fh->print("\\begin{center} \n\\begin{tabular}\{|c|");
    foreach(@{$indicators}) {
        my $indicator = $_;
        $fh->print("c");
        $fh->print("c") if grep{$_ eq $indicator} @{$orders};
        $fh->print("c") if grep{$_ eq $indicator} @{$ratios};
    }    
    $fh->print("|}\n");
    $fh->print("i");
    foreach(@{$indicators}) {
        my $indicator = $_;
        $fh->print(' & ', $indicator);
        $fh->print(' & ocv', $_) if grep{$_ eq $indicator} @{$orders};
        $fh->print(' & ratio', $_) if grep{$_ eq $indicator} @{$ratios};
    }
    $fh->print(" \\\\ \n\\hline\n");
}

#------------------------------------------------------------

sub write_latex_line($$$$$$) {
    my ($fh, $mesh, $indicators, $values, $order_values, $ratio_values) = @_;
    $mesh =~ s/\_/\\\_/g;
    $mesh =~ s/.typ1//g;
    $fh->print("$mesh");
    foreach(@{$indicators}) {
        my $indicator = $_;
        if($values->{$indicator} =~ /\./) {         # Floating point
            $fh->print(' & $', sprintf("%.2e", $values->{$indicator}), '$');
        } elsif($values->{$indicator} eq 'nan') { # Not a number
            $fh->print(' & -- ');
        } else {                                    # Integer
            $fh->print(' & $', sprintf("%d", $values->{$indicator}), '$');
        }

        if(defined($order_values->{$indicator})) {
            my $order_value = $order_values->{$indicator} eq '--' ? '--' : '$'.sprintf("%.2e", $order_values->{$indicator}).'$';
            $fh->print(' & ', $order_value);
        }
        if(defined($ratio_values->{$indicator})) {
            my $ratio_value = $ratio_values->{$indicator} eq '--' ? '--' : '$'.sprintf("%.2e", $ratio_values->{$indicator}).'$';
            $fh->print(' & ', $ratio_value);
        }
    }
    $fh->print(" \\\\ \n");
}

#------------------------------------------------------------

sub write_latex_footer($) {
    my ($fh) = @_;
    $fh->print("\\hline\n\\end{tabular}\n\\end{center}\n");
}

#------------------------------------------------------------

sub usage() {
    print STDERR <<EOF;
Usage: convergence.pl DIRECTORIES DESCRIPTOR [--project-dir DIR] [--output-dir DIR] [--executable EXE] [--num-proc NUM_PROC]
Run the convergence test defined by DIRECTORIES and DESCRIPTOR

  --project-dir : project directory
  --output-dir  : output directory
  --executable  : executable
  --num-proc    : the number of processes

EOF
    exit(1);
}
