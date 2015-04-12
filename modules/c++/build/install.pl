#!/usr/bin/perl
#
# Usage: install.pl [-l] PREFIX package_name version language
#            The "-l" option on the command line means go ahead and create 
#            all the logical links. 
#
# Install the various parts of the package to the appropriate place.
# The "appropriate place" is defined thusly:  We'll assume that the
# installation location is to be appropriate for NFS mounting on various
# hosts of different types.  Thus the tree should look like this:
#
#      PREFIX
#        +-- lib
#             |
#             +-- language
#                  |
#                  +-- Library_A-version_#
#                  |           |
#                  |           +-- build
#                  |           |
#                  |           +-- docs
#                  |           |     +-- reference
#                  |           |     +-- man
#                  |           |     +-- html
#                  |           |     +-- templates
#                  |           |            +-- man
#                  |           |            +-- html
#                  |           |
#                  |           +-- include
#                  |           |
#                  |           +-- lib
#                  |           |    +-- host_type_1
#                  |           |    +-- host_type_2
#                  |           |    +-- ...
#                  |           |
#                  |           +-- tests
#                  |                 +-- bin
#                  |                 |    +-- host_type_1
#                  |                 |    +-- host_type_2
#                  |                 |    +-- ...
#                  |                 |
#                  |                 +-- data
#                  |
#                  +-- Library_A (link to latest version)
#
# It's expected that all hosts would mount prefix to their include
# local shared point (e.g. /var/share).
#
# The test directory should hold any example code or code which can test the
# functionality of the library.  There must be a Makefile in the test
# directory that will build all the code, possibly referencing files in the
# build directory.  Specifically, the build directory should have all the
# necessary Makeflags.<platform> needed to build on the supported platforms.
#
use Getopt::Std;

# The "-l" option on the command line means go ahead and create all the logical
# links.  You may not want to do this without warning people though...
# $opt_l will be a boolean indicating whether it should be done.
getopts('l');

$prefix         = $ARGV[0];
$package        = $ARGV[1];
$version        = $ARGV[2];
$language       = $ARGV[3];


if ( !$prefix  ||  !$package  ||  !$version || !$language)
{
   print "Usage: install.pl prefix package_name version language\n";
   exit;
}

# Where the libraries all live
$LIB_ROOT       = "$prefix/lib/$language";

# The un-versioned library path
$LIB_ROOT_NOVER = "$LIB_ROOT/$package";

# The versioned library path
$LIB_ROOT_VER   = "$LIB_ROOT/$package"."-$version"; 


print "Installing package '$package' (version $version) to '$LIB_ROOT_VER' ";
if ($opt_l) {print "and making links\n";}
else        {print "\n";}



####################
# include files.
#
$output_include_dir = "$LIB_ROOT_VER/include";
print "Installing include files in $output_include_dir\n";
if (! -e "$output_include_dir") {system("mkdir -p $output_include_dir");}
system("cp -R include/* $output_include_dir");


####################
# Library directory.  
#
# NOTE: the desired structure exactly mirrors how we've built it so we can just 
# copy the lib directory tree.  If the build tree changes or the libraries were
# built without using the "standard" build method this will BREAK!
$output_lib_dir = "$LIB_ROOT_VER/lib";
if (! -e "$output_lib_dir") {system("mkdir $output_lib_dir");}
system("cp -R lib/* $output_lib_dir");


####################
# Docs directories.  
if (-e "docs/html")
{
   $output_html_dir = "$LIB_ROOT_VER/docs/html";
   print "Installing html files in $output_html_dir\n";
   if (! -e "$output_html_dir") {system("mkdir -p $output_html_dir");}
   system("cp -R docs/html/* $output_html_dir");
}
if (-e "docs/man")
{
   $output_man_dir = "$LIB_ROOT_VER/docs/man";
   print "Installing man files in $output_man_dir\n";
   if (! -e "$output_man_dir") {system("mkdir -p $output_man_dir");}
   system("cp docs/man/* $output_man_dir");
}
if (-e "docs/reference")
{
   $output_reference_dir = "$LIB_ROOT_VER/docs/reference";
   print "Installing reference files in $output_reference_dir\n";
   system("mkdir -p $output_reference_dir");
   system("cp -R docs/reference/* $output_reference_dir");
}
# Copy over a hard-coded list of documents in the docs directory
@files = ("README", "README.1ST", "INSTALL", "README.html", "style.css");
foreach $name (@files)
{
   if (-e "docs/$name" ) {system("cp docs/$name $LIB_ROOT_VER/docs");}
}


####################
# Build directory.
#
# We'll copy over only the Makeflags* files and config.guess files...
# This install script will not be copied.
#
$output_build_dir = "$LIB_ROOT_VER/build";
print "Installing build files in $output_build_dir\n";
if (! -e "$output_build_dir") {system("mkdir $output_build_dir");}
system("cp -R build/makefiles $output_build_dir");
system("cp -R build/VS.NET $output_build_dir");


####################
# Test directory.
#
# We'll just copy over the entire test tree (source and all).  Make sure there's
# nothing personal in there :)
#
# NOTE: the desired structure exactly mirrors how we want it.  If the build tree 
# changes or the libraries were built without using the "standard" build method 
# this will BREAK!
#
$output_test_dir = "$LIB_ROOT_VER/tests";
print "Installing test files in $output_test_dir\n";
if (! -e "$output_test_dir") {system("mkdir $output_test_dir");}
system("cp -R tests/* $output_test_dir");
# We might need to clean up some of the object modules left over...
system("cd $output_test_dir; rm -f `find . -name \"*.o\" -print`");



# Make the link if the "-l" command line argument was given.
$versioned_name = $package."-".$version;
if ($opt_l) {`cd $LIB_ROOT; rm -f $LIB_ROOT_NOVER`;}
if ($opt_l) {`cd $LIB_ROOT; ln -s $versioned_name $package`;}

# One final thing.  Go through and make sure we have world-read permissions on
# everything we just installed.
system("chmod -R a=u $LIB_ROOT_VER");
system("chmod -R og-w $LIB_ROOT_VER");

