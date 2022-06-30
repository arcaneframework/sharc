#!/usr/bin/env perl

use strict;
use Cwd;
use Getopt::Long;

my $executable;
my $num_proc;
my $parallel = "Seq"; # ou Mpi ou Embedded
my $codename = "ArcGeoSim" ;
my $options;
my $quiet = 0;
GetOptions("executable:s" => \$executable,
           "num-proc:i"   => \$num_proc,
           "parallel:s"   => \$parallel,
           "codename:s"   => \$codename,
           "options:s"    => \$options,
           "quiet!"       => \$quiet);
$num_proc = 1 if(!$num_proc);
die "Executable not provided" unless ($executable);
my $arc_file = shift or die "An argument must be provided";
my @options = @ARGV;
die "Optional argument not implemented : @options" if (@options);

print "-- Executable: $executable\n" unless $quiet;
print "-- Working directory: $ENV{PWD}\n" unless $quiet;
print "-- Lookup config directory: $ENV{STDENV_PATH_SHR}\n" unless $quiet;

# if num_proc > 1, check if a parallel service has been defined
my $parallel_service = $ENV{ARCANE_PARALLEL_SERVICE};

if ($parallel ne 'Embedded') {
    print "-- Parallel service: $parallel_service\n" if($parallel_service && !($quiet));
    die "Cannot run on $num_proc processors: ARCANE_PARALLEL_SERVICE not defined" if($num_proc > 1 and !$parallel_service);
}

# define replacements in .arc file
my %replacements = ();
$replacements{codename} = $codename ;
print "-- Codename : $replacements{codename}\n" unless $quiet;

my $arcfile_content = read_file($arc_file); # die is failed
my $has_changed = ($arcfile_content =~ s/\$\{(\w+)\}/$replacements{$1}/g);
if ($has_changed) {
    $arc_file =~ s/\.arc$/\_test\.arc/;
    write_file($arc_file, $arcfile_content); # die if failed
}

my $command;
if ( $parallel eq "Seq") {
    $command = "$executable $options $arc_file";
} elsif( $parallel eq "Mpi" ) {
    if( $ENV{"OS"} eq "Windows_NT" ) {
		my $environment;
		#foreach my $var ('ARCANE_PARALLEL_SERVICE', 'STDENV_PATH_SHR') {
		#	if (exists $ENV{$var}) {
		#		$environment .= "-env $var \"$ENV{$var}\" ";
		#		delete $ENV{$var};
		#	}
		#}
        $command = "mpiexec -delegate $environment -n $num_proc $executable $options $arc_file" if($parallel_service);
    }
    else {
        $command = "mpirun -np $num_proc $executable $options $arc_file" if($parallel_service);
    }
} elsif ($parallel eq "Embedded") {
    $command = "$executable -n $num_proc $options $arc_file";
}

die "Undefined command" if (!defined $command);

print "-- Executing: $command\n" unless $quiet;

my @args = split /[ \t]+/, $command;
exec(@args) or die "Could not run $command";

sub read_file {
    my $filename = shift;
    local $/;
    open F, $filename or die "Cannot read file '$filename' : $!";
    my $content = <F>;
    close F;
    return $content;
}

sub write_file {
    my ($filename,$content) = @_;
    open F, ">$filename" or die "Cannot write file '$filename' : $!";
    print F $content;
    close F;
}

