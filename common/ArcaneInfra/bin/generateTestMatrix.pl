#!/usr/bin/env perl

#------------------------------------------------------------
# Predeclarations
#------------------------------------------------------------

sub usage();

#------------------------------------------------------------
# Run a matrix of tests 
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
use Getopt::Long;

my $benchmark_file_name = shift or usage();

my $part_path;
my $out_path;
my $separate;
my $help;
my $verbose;
GetOptions("part-dir:s"   => \$part_path,
	   "output-dir:s" => \$out_path,
           "separate"     => \$separate,
	   "help"         => \$help,
	   "verbose"      => \$verbose);
usage() if $help;
$part_path = 'xml' unless $part_path;
$out_path = 'arc' unless $out_path;

my $xml_parser = new XML::DOM::Parser;
my $benchmark = $xml_parser->parsefile("$benchmark_file_name") or die "Failed to open $benchmark_file_name";



my @test_matrices = $benchmark->getElementsByTagName('test-matrix');

my %replacements = ();

foreach my $test_matrix (@test_matrices) {
    my $test_matrix_name = $test_matrix->getAttribute('name');
    my $template_file_name = $test_matrix->getAttribute('template');

    my @chemical_systems = $test_matrix->getElementsByTagName('chemical-system');
    my @dispersions = $test_matrix->getElementsByTagName('dispersion');
    my @meshes = $test_matrix->getElementsByTagName('mesh');

    foreach my $chemical_system (@chemical_systems) {
        my $chemical_system_name = $chemical_system->getAttribute('file-name');
        my @compositions = $chemical_system->getElementsByTagName('composition');
        foreach my $composition (@compositions) {
            my $composition_name = $composition->getLastChild()->getNodeValue();
            foreach my $dispersion (@dispersions) {
                my $dispersion_name = $dispersion->getLastChild()->getNodeValue();
                foreach my $mesh (@meshes) {
                    my $mesh_name = $mesh->getLastChild()->getNodeValue();
                    print "$test_matrix_name($chemical_system_name; $composition_name; $dispersion_name; $mesh_name)\n" if $verbose;

                    $replacements{chemical_system} = $chemical_system_name;
                    $replacements{composition} = $composition_name;
                    $replacements{dispersion} = $dispersion_name;
                    $replacements{mesh} = $mesh_name;

                    my $out_file_name = "$test_matrix_name\_$chemical_system_name\_$composition_name\_$dispersion_name\_$mesh_name";
                    $out_file_name =~ s/\.xml|\.arc//g;
                    $out_file_name =~ s/\//\__/g;                    
                    my $out_dir = $out_path;
                    $out_dir = "$out_path/$out_file_name" if($separate);
                    $out_file_name = "$out_file_name.arc";

                    if($separate) {
                        mkdir($out_dir) or die "Failed to create $out_dir";
                    }

                    open(IN, $template_file_name) or die "Failed to open $template_file_name";
                    open(OUT, ">$out_dir/$out_file_name") or die "Failed to create $out_file_name";
                    while (<IN>) {
                        my $line = $_;
                        $line =~ s/\$\{(\w+)\}/$replacements{$1}/g;
                        if($line =~ m/<!--\s*INCLUDE\{(.*?)\}\s*-->/) {
                            my $part = $1;
                            print "-- Merging $part\n" if $verbose;
                            open(PART, "$part_path/$part") or die "Failed to open $part_path/$part";
                            while(<PART>) {
                                print OUT;
                            }
                        } else {
                            print OUT $line;
                        }
                    }
                    close(IN);
                    close(OUT);
                }
            }
        }
    }
}
print "Done\n" if $verbose;

#------------------------------------------------------------
# Subroutines
#------------------------------------------------------------

sub usage() {
    print STDERR <<EOF;
Usage: generateTestMatrix.pl TESTS [--part-dir DIR] [--output-dir DIR] [--help] [--verbose]
Generate the test matrix described by TESTS

  --part-dir    : the part files directory
  --output-dir  : the output directory
  --verbose     : verbose execution
  --help        : print this message

EOF
    exit(1);
}
