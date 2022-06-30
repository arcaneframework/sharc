#!/usr/bin/perl

use strict;

my $rootdir = shift @ARGV or die "Rootdir must be specified";
my $testname = shift @ARGV or die "Testname must be specified";

$testname =~ s/_(\d+proc)$//;
my $suffix = $1;

print "[Embedded test]";
exit 0;
