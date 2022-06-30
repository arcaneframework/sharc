#!/usr/bin/env perl

use Cwd;
use File::Basename qw/ dirname /;

#------------------------------------------------------------

sub usage;

#------------------------------------------------------------

use warnings;
use strict;

my $N = shift or die usage();
my $M = shift or die usage();
die usage() if($M != 1);

my $script_dir = dirname(Cwd::realpath($0));

opendir(DIR, ".");
my @templates = grep(/\.template$/, readdir(DIR));
closedir(DIR);

foreach my $template (@templates) 
{
    for(my $n=1;$n<=$N;$n++) 
    {
        for(my $m=1;$m<=$M;$m++)
        {
            print "-- ($template, R$n, R$m)\n";
            `$script_dir/nxm.pl $n $m $template`;
        }
    }
}
print "Done\n";

#------------------------------------------------------------

sub usage()
{
    print <<USAGE;
nxmAll.pl n m

** Warning ** only m = 1 available for the moment being
USAGE
}
