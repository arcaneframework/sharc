#!/usr/bin/env perl
use warnings;
use strict;
use Getopt::Long;

#-----------------------------------------------------------
# Predeclarations

sub usage();

#-----------------------------------------------------------
# Main program

my %options = ();
my $verbose = 0;
my $help = 0;
my $comment = 0;
my $out_file;
GetOptions("output:s" => \$out_file,
	   "help"     => \$help,
	   "verbose"  => \$verbose,
     "comment"  => \$comment);

our $progname = $0;
$progname =~ s|^(.*)/||;

usage() if $help;

my $in_file = shift or die usage();
die "Can't find $in_file" unless -f $in_file;

if(!$out_file) {
    $out_file = $in_file;
    $out_file =~ s/.arc$/\_merged.arc/g;
}

open(IN, "<$in_file") or die "Could not open $in_file\n";
open(OUT, ">$out_file") or die "Could not open $out_file\n";
my @content = <IN>;
close IN;

if ($comment) 
{   # header informations after the first line
    my $AUTHOR = "$ENV{USER} at ".localtime(time());
    my $line = shift @content;
    print OUT $line;
    print OUT <<EOF;
<!-- Generated file from $in_file -->
<!-- by $AUTHOR -->
EOF
}

foreach my $line (@content) {
    if($line =~ m/^(.*)<!--\s*INCLUDE{(.*?)}\s*-->(.*)$/) {
        my $prefix = $1;
        my $part_file = $2;
        my $suffix = $3;
        #$part_file =~ /$.xml/ or $part_file .= '.xml';
        print OUT $prefix;
        open(PART, "<$part_file") or die "Could not open '$part_file' to include\n";
        print OUT "<!-- begin inclusion of '$part_file' -->\n" if ($comment);
        print OUT while (<PART>);
        print OUT "<!-- end inclusion of '$part_file' -->\n" if ($comment);
        close PART;
        print OUT $suffix;
        print "-- Merging $part_file\n" if $verbose;
    } else {
        print OUT $line;
    }
}

print "Done\n" if $verbose;

#-----------------------------------------------------------
# Subroutines 

sub usage() {
    print <<EOF;
Usage: $progname SOURCE[--output OUTPUT] [--verbose] [--help]
Create OUTPUT from file SOURCE.
  --help                       display this help message
  --verbose                    verbose
  --comment                    add information by comments in OUTPUT file
  --output                     output name (if SOURCE is source.arc, default is source_merged.arc)
Inclusion syntax is
   <!-- INCLUDE{my_file_to_include} -->
EOF
exit;
}
