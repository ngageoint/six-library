#! /usr/bin/perl

################################################################################
# This script creates symbolic links for the codaforge libraries, as a means
# to have backward-compatibility with the lib names. The libraries now have a
# "coda." prefix in their names. This script will link the old lib name to this
# new one.
################################################################################

use strict;
eval { require warnings; };
eval "use warnings;" unless $@;
use POSIX qw(WEXITSTATUS);


@ARGV >= 1 or die "You must at least pass the target lib directory";

my $target = shift;

while (@ARGV)
{
    my $dir = shift @ARGV;
    my $lib = "libcoda." . $dir . "-c++.a";
    my $symLib = "lib" . $dir . "-c++.a";
    my $fullPath = $dir . "/lib/" . $target . "/";
    
    # makefile for coda looks at stale value of $PWD
    # force it to use the directory we're cd'ing to
    local $ENV{PWD} = $dir;
    # tack a cd command onto the beginning of the command
    
    #first, remove it
    my $cmd = "rm -f " . $fullPath . $symLib;
    command($cmd);
    
    #now, link it
    $cmd = "ln -s " . $lib . " " . $symLib;
    $cmd = "cd " . $fullPath . "; " . $cmd;
    command($cmd);
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  command()
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  Print a command and then execute it
sub command {
    my $cmd = shift;
    print $cmd . "\n";
    system ($cmd) == 0
    or die "'$cmd' died with exit code: " . POSIX::WIFEXITED($?);
}
