#!/usr/bin/env perl

BEGIN {
    my $base = '/home/irsrvshare1/R11/infradev/softs/perl/perl5/site_perl';
    @INC = (@INC,$base);
    for my $f1 (glob("$base/*")) {
        if (-d $f1) {
            for my $f2 (glob("$f1/*")) {
                @INC = (@INC,$f2) if (-d $f2);
            }
        }
    }
}

#------------------------------------------------------------

sub usage;

#------------------------------------------------------------

use warnings;
use strict;
use Template;

my $n    = shift or die usage();
my $m    = shift or die usage();
die usage() if($n < 1 || $m < 1 || $m != 1);
my $tmpl = shift or die usage();
my $output = $tmpl;
$output =~ s/Rn/R$n/g;
$output =~ s/Rm/R$m/g;
$output =~ s/.template//g;

print "$tmpl => $output\n";

my $config =
{
    EVAL_PERL => 1
    };
my $template = Template->new($config) or die "$Template::ERROR\n";
my @vars = '';
for(my $i=0;$i<$n;$i++)
{
    $vars[$i] = "var$i";
}
my @ress = '';
my @diffress = '';
my @ressmo = '';

for(my $i=0;$i<$m;$i++)
{
    $ress[$i] = "res$i";
    $diffress[$i] = "diffres$i";
}
my $subs =
{
    n       => $n,
    m       => $m,
    vars    => \@vars,
    ress    => \@ress,
    diffress=> \@diffress,
    filename=> $tmpl,
    date    => scalar gmtime,
};
$template->process($tmpl, $subs, $output) or die $template->error(),"\n";

#------------------------------------------------------------

sub usage()
{
    print <<USAGE;
nxm.pl n m template

** Warning ** only m = 1 available for the moment being
USAGE
}
