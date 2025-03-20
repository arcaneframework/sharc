#! /usr/bin/env perl

use strict;
use warnings;
use Cwd;

my $file_name = shift;
die "Could not find $file_name" unless -f $file_name;

# Deduce old header
my $old_header = $file_name;
$old_header =~ s|\.|_|g;
$old_header = uc('BISON_'.$old_header);

# Construct new header
our $path = '';
$path = Cwd::realpath(getcwd());

unless ($path =~ s|^(.*/)?(\w+)/src/(.*)|$2/$3|) {
    usage("Path '$path' does not look like an compatible path");
}
my $new_header = $path.'_'.$file_name;
$new_header =~ s|/|_|g;
$new_header =~ s|\.|_|g;
$new_header .= '_';
$new_header = uc($new_header);

my $backup_name = $file_name."\.old";
`mv $file_name $backup_name`;
open INFILE, "<", "$backup_name" or die "Could not open $backup_name";
open OUTFILE, ">", "$file_name" or die "Could not open $file_name";
my @file_content = <INFILE>;
foreach my $current_line (@file_content) {
    if($current_line =~ m|$old_header| or $current_line =~ m|PARSER_HEADER_H|) {
        $current_line =~ s|$old_header|$new_header|g;
        $current_line =~ s|PARSER_HEADER_H|$new_header|g;
    }
    print OUTFILE $current_line;
}

