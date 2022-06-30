#!/usr/bin/perl

use strict;

my $rootdir = shift @ARGV or die "Rootdir must be specified";
my $testname = shift @ARGV or die "Testname must be specified";

$testname =~ s/_(\d+proc)$//;
my $suffix = $1;

# check max test name length
exit 2 if (length $testname > 50);

my $optional_arcgeophy_author_filename = "$rootdir/common/ArcGeoPhy/test/Authors.txt";
my $optional_sharedutils_author_filename = "$rootdir/common/SharedUtils/test/Authors.txt";

my @files = ();

if (-e $optional_arcgeophy_author_filename) {
    if (-e $optional_sharedutils_author_filename) {
    @files = ("$rootdir/test/Authors.txt","$rootdir/common/ArcGeoSim/test/Authors.txt",$optional_arcgeophy_author_filename, $optional_sharedutils_author_filename);
   }else{
    @files = ("$rootdir/test/Authors.txt","$rootdir/common/ArcGeoSim/test/Authors.txt",$optional_arcgeophy_author_filename);
    }
}
else {
if (-e $optional_sharedutils_author_filename) {
    @files = ("$rootdir/test/Authors.txt","$rootdir/common/ArcGeoSim/test/Authors.txt",$optional_sharedutils_author_filename);
   }
   else{
    @files = ("$rootdir/test/Authors.txt","$rootdir/common/ArcGeoSim/test/Authors.txt");
    }
}

foreach my $file (@files) {
    open (FILE,$file) or die "Cannot open Authors file";

    while(my $line = <FILE>) {
        chomp $line;
        if ($line =~ m/^$testname\::\s*(.*)\s*$/) {
            my $author = $1;
            if ($author eq "") {
                exit 1;
            } else {
                print "$author";
                exit 0;
            }
        }
    }
    close FILE;
}

exit 1;
