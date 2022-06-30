#!/usr/bin/env perl

BEGIN {
    unshift(@INC, "/home/irsrvshare1/R11/arcuser/softs/site_perl/lib/perl5");
}

use strict;
use POSIX;
use Getopt::Long;
use Cwd;

my $ToolName = 'OctopusTest';
my $ConfigFile = "$ENV{HOME}/.$ToolName.cfg";

# Check config file availability
my $HasConfigFileFeature = 0;
eval { require Config::General; $HasConfigFileFeature = 1; };

my %project_config = ( 'CooresArcane'      => { 'global' => 'ArcSim',
                                                'branch' => 'CooresArcane' },
                       'ArcTem'            => { 'global' => 'ArcSim',
                                                'branch' => 'ArcTem',
                                                'tag'    => 'Indus|Daily' },
                       'ArcTem-v3'         => { 'global' => 'ArcSim',
                                                'branch' => 'ArcTem-branches/ArcTem-V3-20120213',
                                                'tag'    => 'Indus' },
                       'CATSArcane'        => { 'global' => 'ArcSim',
                                                'branch' => 'CATSArcane',
                                                'tag'    => 'Daily' },
                       'CAMEL'             => { 'global' => 'ArcSim',
                                                'branch' => 'CAMEL',
                                                'tag'    => 'Daily' },
                       'CAMEL-Dev-V2'      => { 'global' => 'ArcSim',
                                                'branch' => 'CAMEL-branches/CAMEL-Dev-V2' },
                       'CAMEL-Indus-V1'    => { 'global' => 'ArcSim',
                                                'branch' => 'CAMEL-branches/CAMEL-V1-20130128-Indus',
                                                'tag'    => 'Indus' },
                       'Geoxim'            => { 'global' => 'ArcSim',
                                                'branch' => 'Geoxim',
                                                'tag'    => 'Daily' },
                       'Geoxim-unstable'   => { 'global' => 'ArcSim',
                                                'branch' => 'Geoxim-branches/Geoxim-unstable',
                                                'tag'    => 'Daily' },
                       'ArcEOR'            => { 'global' => 'ArcSim',
                                                'branch' => 'ArcEOR',
                                                'tag'    => 'Daily' },
                       'ArcaneDemo'        => { 'global' => 'ArcSim',
                                                'branch' => 'ArcaneDemo',
                                                'tag'    => 'Daily' },
                       'ArcaDES'           => { 'global' => 'ArcSim',
                                                'branch' => 'ArcaDES',
                                                'tag'    => 'Indus|Daily' },
                       );

my %allowed_modes = ( 'debug'   => 'debug mode', 
                      'release' => 'optimized mode' );

my %restart_ids = ( 'checkout' => 1, 
                    'patch'    => 2,
                    'compile'  => 3,
                    'test'     => 4,
                    'end'      => 5);


my %global_config = ( 'ArcSim' => { 'url'         => 'https://websvn.ifpen.fr/svn/ArcSim',                                                                                                   # svn root url
                                    'compilcmd'   => 'cd $BUILDDIR && ../configure --verbose --arcane=$arcane_version --arcane-path=$arcane_path --$mode && make -j $config->{concurrency}', # compil command (will be expanded later)
                                    'builddir'    => 'build',                                                                                                                      # build dir (relative to checkdir)
                                    'testdir'     => 'build/test',                                                                                                                 # test directory (relative to checkdir)
                                  },                                       
                    );

my ($ArcaneInfra_path,$ArcGeoSim_path,$ArcGeoPhy_path,$SharedUtils_path) = ('@HEAD', '@HEAD', '@HEAD', '@HEAD');
my $internal_debug;
my $internal_restart = 'checkout';
my $mode = 'debug';
my ($arcane_path, $arcane_version);
my $target_revision = 'HEAD';
my $patch_command;
my $skip_test = 0;

my $msg_offset      = ' 'x(length(scalar localtime)+3);
my $separator       = '-'x80;

my $config = { $ToolName => { 'concurrency' => 4, 'workdir' => '/tmp', 'verbose' => 0 } };
if ($HasConfigFileFeature) {
    if (! -e $ConfigFile) {
        my $conf = new Config::General(-ConfigHash => $config,
                                       -AllowMultiOptions => 0,
                                       -MergeDuplicateOptions => 1,
                                       -StoreDelimiter => ' = ',
                                       );
        $conf->save_file($ConfigFile);
        $config = $config->{$ToolName};
    } else {
        my $conf = new Config::General(-ConfigFile => $ConfigFile,
                                       -DefaultConfig => $config,
                                       -AllowMultiOptions => 0,
                                       -MergeDuplicateOptions => 1,
                                       -MergeDuplicateBlocks => 1,
                                       -StoreDelimiter => ' = ',
                                       );
        $config = do { my %d = $conf->getall(); $d{$ToolName}; }
    }
}
else
{   # revert to default mode without config
    $config = $config->{$ToolName};    
}

my %default_config = %{$config};

usage("ERROR: Bad configuration") unless (GetOptions ("mode=s"             => \$mode,
                                                      "concurrency|j=n"    => \$config->{concurrency},
                                                      "revision|r=s"       => \$target_revision,
                                                      "arcane=s"           => \$arcane_version,
                                                      "arcane-path=s"      => \$arcane_path,
                                                      "ArcaneInfra=s"      => \$ArcaneInfra_path,
                                                      "ArcGeoSim=s"        => \$ArcGeoSim_path,
                                                      "ArcGeoPhy=s"        => \$ArcGeoPhy_path,
                                                      "SharedUtils=s"      => \$SharedUtils_path,
                                                      "patch-command|p=s"  => \$patch_command,
                                                      "verbose!"           => \$config->{verbose},
                                                      "workdir=s"          => \$config->{workdir},
                                                      "skip-test"          => \$skip_test,
                                                      "help"               => sub { usage("Requested help page"); },
                                                      "internal-restart=s" => \$internal_restart,
                                                      "internal-debug=s"   => \$internal_debug)
                                          and exists $allowed_modes{$mode}
                                          and $ArcGeoSim_path
                                          and $ArcaneInfra_path
                                          and $ArcGeoPhy_path
                                          and $SharedUtils_path);

$config->{workdir} =~ s/^\s*(~)/$ENV{HOME}/;
$config->{workdir} = Cwd::realpath($config->{workdir}); # considere relative or ~ reference

my $rewritten_project_list = 0; # check if @ARGV project list has been rewritten while checking
my @project_list;
{
    my %project_by_tag;
    foreach my $p (keys %project_config)
    {
        push @{$project_by_tag{$_}}, $p foreach(split /\|/, $project_config{$p}{tag});
    }

    my %project_set; # set of already listed projects
    foreach my $PROJECT (@ARGV) {
        if ($PROJECT =~ s/^@//) { # select project by tag
            if (exists $project_by_tag{$PROJECT}) {
                $rewritten_project_list = 1;
                my @tagged_projects = @{$project_by_tag{$PROJECT}};
                if ($config->{verbose}) { print $msg_offset."Add project tag $PROJECT : ",join(",", @tagged_projects),"\n"; }
                foreach my $p (@tagged_projects) {
                    if (exists $project_set{$p}) {
                        print $msg_offset."Do not duplicate already listed project '$p'\n" if ($config->{verbose});
                    } else {
                        $project_set{$p} = 1;
                        push @project_list, $p;
                    }
                }
            } else {
                usage("ERROR: Unknown project tag '$PROJECT'");
            }            
        } else {
            if (exists $project_config{$PROJECT}) {
                if (exists $project_set{$PROJECT}) {
                    $rewritten_project_list = 1;
                    print $msg_offset."Do not duplicate already listed project '$PROJECT'\n" if ($config->{verbose});
                } else {
                    $project_set{$PROJECT} = 1;
                    push @project_list, $PROJECT;
                }
            } else {
                usage("ERROR: Unknown project '$PROJECT'");
            }
        }
    }
}

$target_revision =~ s/^@//; # remove heading @ (compatibility with ArcGeoSim & ArcaneInfra revisions)
usage("ERROR: Revision number must be positive or HEAD") 
    unless ($target_revision eq 'HEAD' or $target_revision =~ /^\d+/);

usage("ERROR: Invalid internal-restart while debugging") 
    if ($internal_debug and not exists $restart_ids{$internal_restart});

usage("ERROR: Empty project list")
    unless (@project_list);

my @date            = localtime();
my $DATE            = sprintf("%02d%02d%02d-%02d%02d",$date[5]+1900,$date[4]+1,$date[3],$date[2],$date[1]);
my $ROOTDIR         = "$config->{workdir}/${ToolName}_${DATE}";
   $ROOTDIR         = $internal_debug if ($internal_debug);
my $DATADIR         = $ROOTDIR;

# nettoyage du processus en cas d'arrêt anticipé 
$SIG{INT} = \&cleanup;

if ($config->{verbose} or $rewritten_project_list) {
    print " $separator\n";
    print ' ',(scalar localtime),"  $ToolName on applications : ",join(' ',@project_list),"\n";
}

$ArcaneInfra_path = rebase_common('ArcaneInfra', $ArcaneInfra_path);
$ArcGeoSim_path = rebase_common('ArcGeoSim', $ArcGeoSim_path);
$ArcGeoPhy_path = rebase_common('ArcGeoPhy', $ArcGeoPhy_path);
$SharedUtils_path = rebase_common('SharedUtils', $SharedUtils_path);
$arcane_path = Cwd::realpath($arcane_path) if ($arcane_path);

if ($config->{verbose}) {
    print $msg_offset."with ArcGeoSim=$ArcGeoSim_path\n";
    print $msg_offset."with ArcaneInfra=$ArcaneInfra_path\n";
    print $msg_offset."with ArcGeoPhy=$ArcGeoPhy_path\n";
    print $msg_offset."with SharedUtils=$SharedUtils_path\n";
    print $msg_offset."with Arcane=$arcane_path\n" if ($arcane_path);
    print $msg_offset."with Arcane Version=$arcane_version\n" if ($arcane_version);
    print $msg_offset."into directory $ROOTDIR\n";
}

my $anyerror = 0;

PROJECT: foreach my $PROJECT (@project_list) {
    print " $separator\n";

    my $GLOBAL = $project_config{$PROJECT}{global};
    unless (exists $global_config{$GLOBAL}) {
        print STDERR "ERROR: Global [$GLOBAL] not configured\n";
        exit 3;
    }

    my $URL             = $global_config{$GLOBAL}{url};
    my $CHECKDIR        = "${ROOTDIR}/${PROJECT}"; # checkout dir
    my $BUILDDIR        = "${CHECKDIR}/$global_config{$GLOBAL}{builddir}";
    my $TESTDIR         = "${CHECKDIR}/$global_config{$GLOBAL}{testdir}";

    my $COMPILCOMMAND   = $global_config{$GLOBAL}{compilcmd};
    $COMPILCOMMAND      =~ s/(\$\w+(->{\w+})?)/$1/eeg; # interpolation des variables simples et des références de tables de hachage
    $COMPILCOMMAND      =~ s/--arcane= //g unless ($arcane_version);
    $COMPILCOMMAND      =~ s/--arcane-path= //g unless ($arcane_path);
    $COMPILCOMMAND      =~ s/--verbose //g unless ($config->{verbose});

    my $PATCHLOG        ="${CHECKDIR}/patchlog";
    my $MKLOG           ="${BUILDDIR}/mklog";
    my $CTESTLOG        ="${TESTDIR}/ctestlog";
    my $BRANCH          = $project_config{$PROJECT}{'branch'};
    my $TEST_TIME       = time;

    # init log file
    execute("mkdir -p $CHECKDIR", "Fatal error: cannot create checkout directory");

    unless ($restart_ids{$internal_restart} > $restart_ids{checkout}) {        
        print ' ',(scalar localtime),"  Checkout project $PROJECT revision $target_revision into $CHECKDIR\n";
        execute("svn checkout  --ignore-externals -q -r $target_revision $URL/$BRANCH $CHECKDIR", "Cannot retrieve project $PROJECT @ $target_revision");
        unlink "$CHECKDIR/common/ArcaneInfra", "$CHECKDIR/common/ArcGeoSim"; # remove dead link if exists (useful for internal debug mode)
        execute("ln -sf $ArcaneInfra_path $CHECKDIR/common/ArcaneInfra", "Fatal error: cannot create symlink from $ArcaneInfra_path to $CHECKDIR/common/ArcaneInfra");
        execute("ln -sf $ArcGeoSim_path $CHECKDIR/common/ArcGeoSim", "Fatal error: cannot create symlink from $ArcGeoSim_path to $CHECKDIR/common/ArcGeoSim");
        execute("ln -sf $ArcGeoPhy_path $CHECKDIR/common/ArcGeoPhy", "Fatal error: cannot create symlink from $ArcGeoPhy_path to $CHECKDIR/common/ArcGeoPhy");
        execute("ln -sf $SharedUtils_path $CHECKDIR/common/SharedUtils", "Fatal error: cannot create symlink from $SharedUtils_path to $CHECKDIR/common/SharedUtils");
    } else {
        print ' ',(scalar localtime),"  [Internal Debug] Skip checkout; use project $PROJECT into $CHECKDIR\n";
    }

    my $compilation_report;
    my $last_test_code;

    if ($config->{verbose}) {
        my @lines = `svn info $CHECKDIR`;
        foreach my $line (@lines) { print $msg_offset.$line; }
    }

    if ($patch_command) {
        unless ($restart_ids{$internal_restart} > $restart_ids{patch}) {
            print ' ',(scalar localtime),"  Apply patch command : $patch_command (logs in $PATCHLOG)\n";
            chdir $CHECKDIR;
            my @lines = `$patch_command 2>&1`;
            my $errcode = $?;
            if ($errcode) {
                print ' ',(scalar localtime),"  Patch command failed with code = $errcode\n";
            } elsif ($config->{verbose}) {
                print ' ',(scalar localtime),"  Patch command applied successfully with code = $errcode\n";
            }

            open LOG, ">$PATCHLOG" or dier("Cannot open $PATCHLOG");
            foreach my $line (@lines) { print LOG $line; }
            close LOG;

            if ($errcode or $config->{verbose}) {
                if (@lines) {
                    print $msg_offset."Patch output follows:\n";
                    foreach my $line (@lines) { print $msg_offset.$line; }
                } else {
                    print $msg_offset."Empty patch output\n";
                }
            }
            if ($errcode) {
                print ' ',(scalar localtime),"  Skip project $PROJECT due to patch failure\n";
                $anyerror += 1;
                next PROJECT;
            }
        } else {
            print ' ',(scalar localtime),"  [Internal Debug] Skip patch command\n";
        }
    }
    
    execute("mkdir -p $BUILDDIR", "Cannot make build directory $BUILDDIR");

    my $last_compilation_code = 0; # choose problem to internal_debug (1: compilation pb, 0: test pb)
    unless ($restart_ids{$internal_restart} > $restart_ids{compile}) {
        print ' ',(scalar localtime),"  Starting compile (logs in $MKLOG)\n";
        print ' ',(scalar localtime),"  Configuring project using: $COMPILCOMMAND\n";
        $last_compilation_code = system("($COMPILCOMMAND) >$MKLOG 2>&1");
    } else {
        print ' ',(scalar localtime),"  [Internal Debug] Skip compilation (logs in $MKLOG)\n";
    }

    if ($last_compilation_code) {
        print ' ',(scalar localtime),"  Finishing compile with errors\n";
        $anyerror += 1;
    } else {
        print ' ',(scalar localtime),"  Finishing compile successfully\n" unless ($restart_ids{$internal_restart} > $restart_ids{compile});
        my ($error_count,$warning_count) = (0,0);
        my %user_warning;
        open MKFILE, $MKLOG or dier("Cannot open $MKLOG");
        while(<MKFILE>) { 
            ++$error_count if (/error:/); 
            ++$warning_count if (/warning:/); 
            if (/\#warning \"(\w+):/) { ++$user_warning{$1}; }
        }
        close MKFILE;
        my $user_warning_log;
        while (my ($k,$v) = each %user_warning) { $user_warning_log .= "$k:$v "; }
        if ($user_warning_log) { $user_warning_log = "\tuser warnings: $user_warning_log"; }
        if ($error_count) {
            print $msg_offset."Compilation has ".plural($error_count,"non fatal error")."and ".plural($warning_count,"warning")."\n".
                $msg_offset."$user_warning_log\n";
        } elsif ($warning_count) {
            print $msg_offset."Compilation has ".plural($warning_count,"warning")."\n".
                $msg_offset."$user_warning_log\n";
        }

        if ($skip_test) {
            print ' ',(scalar localtime),"  Skip test\n";
        } else {
            my $testcode = 0;
            my $testcount = 0;
            my $testname_size = 0;
            { # get test count
                chdir $TESTDIR or $testcode=1;
                unless ($testcode) {
                    if (open CTEST, "ctest -N|") {
                        my @lines = <CTEST>;
                        $lines[-1] =~ /(\d+)/;
                        $testcount = $1;
                        close CTEST;
                        foreach my $line (@lines) {
                            if ($line =~ m/\#\d+: (.*)$/) { $testname_size = length($1) if ($testname_size < length($1)); }
                        }
                    } else {
                        $testcode = 1;
                    }
                }
            }
            my $testcount_size = length("$testcount");

            unless ($testcode) {
                unless ($restart_ids{$internal_restart} > $restart_ids{test}) {
                    unlink $CTESTLOG;
                    print ' ',(scalar localtime),"  Starting tests (logs in $CTESTLOG)\n";
                } else {
                    print ' ',(scalar localtime),"  [Internal Debug] Skip test (logs in $CTESTLOG)\n";
                }
                my @failed_tests;

                my $maxline_size = 0;
                for(my $itest=1;$itest<=$testcount;++$itest) 
                {
                    my $testname;
                    open CTEST, "(cd $TESTDIR && ctest -N -I $itest,$itest --submit-index $itest) |" or dier("Cannot stat ctest");
                    while (my $line = <CTEST>) {
                        if ($line =~ m/\#$itest: (.*)$/) { $testname = $1; }
                    }
                    close CTEST;
                    my $line = ' '.(scalar localtime).sprintf "  Test %${testcount_size}d / $testcount : %${testname_size}s : error count = $testcode\r", $itest, $testname;
                    print $line;
                    $maxline_size = length($line);
                    my $code |= system("(cd $TESTDIR && ctest -I $itest,$itest --submit-index $itest) >> $CTESTLOG 2>&1") unless ($restart_ids{$internal_restart} > $restart_ids{test});
                    if ($code) {
                        push @failed_tests, $testname;
                        ++$testcode;
                    }
                }
                print ' 'x$maxline_size."\r"; # cleanup progression line

                if ($testcode == 0) {
                    print ' ',(scalar localtime),"  Finishing test successfully\n" unless ($restart_ids{$internal_restart} > $restart_ids{test});
                } else {
                    print ' ',(scalar localtime),"  Finishing test with ".plural($testcode,"error")."\n";
                    map { print "\t$_\n" } @failed_tests if ($config->{verbose});
                    $anyerror += 1;
                }
            } else {
                # broken test system
                print ' ',(scalar localtime),"  Broken test system\n";
                $anyerror += 1;
            }
        }
    }

    $TEST_TIME = ceil((time - $TEST_TIME)/60);
    print ' ',(scalar localtime),"  $ToolName done for $PROJECT @ $target_revision in $TEST_TIME mn\n";
}

print " $separator\n";
if ($anyerror) {
    print ' ',(scalar localtime),"  Global test process has ".plural($anyerror,"error")." : check log above\n";
} else {
    my @goods = ( 'Amazing', 'Astonishing', 'Astounding', 'Breathtaking', 'Brilliant', 
                  'Extraordinary', 'Marvelous', 'Miraculous', 'Phenomenal', 'Prodigious', 
                  'Remarkable', 'Sensational', 'Singular', 'Spectacular', 'Stupendous', 'Wondrous' );
    my $good_work = $goods[int(rand(scalar(@goods)))];
    print ' ',(scalar localtime),"  $good_work : all tests are OK !\n";
}

END {
    &cleanup;
}

# sous-routines utilitaires

sub usage() {
    my $msg = shift;
    if ($config->{verbose}) {
        my $ascii_art = q#
                          ___
                       .-'   `'.
                      /         \
                      |         ;
                      |         |           ___.--,
             _.._     |0) ~ (0) |    _.---'`__.-( (_.
      __.--'`_.. '.__.\    '--. \_.-' ,.--'`     `""`wis
     ( ,.--'`   ',__ /./;   ;, '.__.'`    __
     _`) )  .---.__.' / |   |\   \__..--""  """--.,_
arc*`---' .'.''-._.-'`_./  /\ '.  \ _.-~~~````~~~-._`-.__.'
          | |  .' _.-' |  |  \  \  '.               `~---`camel
           \ \/ .'     \  \   '. '-._)
            \/ /        \  \    `=.__`~-.
            / /\         `) )    / / `"".`\
      , _.-'.'\ \        / /    ( (     / /
    atm`--~`   ) )    .-'.'      '.'.  | (
              (/`    ( (`          ) )  '-;jgs
               `geox  '-;eor      (-`cats
#;
        print "$msg_offset$_\n" foreach (split "\n", $ascii_art);
    }

    # trick for a well formatted workdir default value
    my $workdir_format = ' 'x(38-length($default_config{workdir}))."(user default=$default_config{workdir})";

    print STDERR "$msg\n" if ($msg);
    print STDERR <<EOF;
usage : $ToolName [options] --ArcGeoSim=path --ArcaneInfra=path [project|\@tag]...
Common options: 
      --ArcGeoSim=path        : path to your ArcGeoSim directory                                         (default=\@HEAD)
                                path may be \@HEAD or \@revision to checkout specified revision from SVN
      --ArcaneInfra=path      : path to your ArcaneInfra directory                                       (default=\@HEAD)
                                path may be \@HEAD or \@revision to checkout specified revision from SVN
      --ArcGeoPhy=path        : path to your ArcGeoPhy directory                                       (default=\@HEAD)
                                path may be \@HEAD or \@revision to checkout specified revision from SVN
      --SharedUtils=path      : path to your SharedUtils directory                                       (default=\@HEAD)
                                path may be \@HEAD or \@revision to checkout specified revision from SVN
      --mode=release|debug    : select compilation mode                                                  (default=debug)
      --arcane=version        : override selected arcane version while testing projects
      --arcane-path=path      : override selected arcane path while testing projects
      --concurrency=n         : use n processes for compilation                                         (user default=$default_config{concurrency})
                                or alternative '-j n' syntax
      --revision=rev          : use revision 'rev' for testing;                                           (default=HEAD)
                                or alternative syntax '-r rev'
      --patch-command=cmd     : command to execute before compiling (may help to propagate a custom patch)
                                or alternative '-p cmd' syntax
                                The command must return 0 when applied successfully.
                                ex: "perl -i.bak -ne 'print unless(/ExpressionParser/)' libraries-arcgeosim.xml"
      --[no-]verbose          : increase progress details display                                       (user default=$default_config{verbose})
      --workdir=path          : absolute path for working directory$workdir_format
      --skip-test             : skip test step
      --help                  : this help page

Internal debugging options:
      --internal-restart=step : restart $ToolName process from checkout|patch|compile|test|end (default=checkout)
      --internal-debug=path   : select existing $ToolName directory for internal debugging
                                override 'workdir' option

EOF

    if ($HasConfigFileFeature) {
      print STDERR "Your $ToolName user config parameters are located $ConfigFile.\n";
    } else {
      print STDERR "Sorry, user config file not available in this environment.\n";
    }

    print STDERR <<EOF;

Available projects (and their tags):
EOF
    my ($maxsize_name,$maxsize_tag) = (0, 0);
    foreach my $p (keys %project_config) { 
      $maxsize_name = length($p) if (length($p) > $maxsize_name);
      $maxsize_tag = length($project_config{$p}{tag}) if (length($project_config{$p}{tag}) > $maxsize_tag);
    }
    $maxsize_tag +=2;

    my $i = 0;
    map { print STDERR sprintf("\t%-${maxsize_name}s %-${maxsize_tag}s", $_,'('.$project_config{$_}{tag}.')'); print STDERR "\n" unless (++$i%3); } sort keys %project_config;
    print STDERR "\n" if ($i%3);
    exit 1;
}

sub cleanup {
  my $signal = shift;
  if ($ROOTDIR) {
      print " $separator\n";
      print " $separator\n";
      print " To clean all : rm -fr ${ROOTDIR}\n";
  }
  dier("Killed by signal $signal") if ($signal);
}

sub rebase_common {
  my ($what, $where) = @_;
  if ($where =~ /^@(.*)$/) {
    my $revision = $1;
    my $CHECKDIR = $where = "${ROOTDIR}/${what}";
    my $URL = $global_config{ArcSim}{url};
    execute("mkdir -p $CHECKDIR", "Fatal error: cannot create checkout directory");
    unless ($restart_ids{$internal_restart} > $restart_ids{checkout}) {
        print ' ',(scalar localtime),"  Checkout $what revision $revision into $CHECKDIR\n";
        execute("svn checkout  --ignore-externals -q -r $revision $URL/$what $CHECKDIR", "Cannot retrieve $what @ $revision");
    } else {
        print ' ',(scalar localtime),"  [Internal Debug] Skip checkout; use $what into $CHECKDIR\n";
    }
    return $CHECKDIR;
  } else {
      return Cwd::realpath($where);
  }
} 

sub execute {
    my ($cmd, $msg) = @_;
    my @lines = `$cmd 2>&1`;
    my $errcode = $?;
    my $errmsg = $!;
    if ($errcode) {
        $errmsg = " [$errmsg]" if ($errmsg);
        my $fullmsg = "$msg $errmsg";
        foreach my $line (@lines) { $fullmsg .= "\n\t$line"; }
        dier($fullmsg);
    }
    return $errcode;
}

sub dier {
    my $msg = shift;
    die "\n Fatal error: $msg";
}

sub plural {
    my ($n,$word) = @_;
    if ($n <= 1) { return "${n} ${word}"; }
    else { return "${n} ${word}s"; }
}
