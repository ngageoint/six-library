# ==================== DO NOT EDIT THIS HEADER ======================== 
# 
# $RCSfile: makedist.pl,v $
# $Revision: 1.1.1.1 $
# $Name: not supported by cvs2svn $
# $Date: 2003-12-18 15:19:26 $
# $Author: dpressel $
# 
# Copyright (c) 2003 Veridian Systems Division
# Ann Arbor, MI. 
# 
# All Rights Reserved 
# 
# THIS IS UNPUBLISHED COPYRIGHTED SOURCE CODE containing
# proprietary information of Veridian Systems Division
# This copy has been provided on the basis of strict
# confidentiality and on the express understanding that
# it may not be reproduced, or revealed to any person,
# in whole or in part, without express written permission
# from Veridian Systems Division, which is sole owner of
# copyright and all other rights therein.
# 
# $Log: not supported by cvs2svn $
# Revision 1.1.1.1  2003/07/24 12:31:09  dpressel
# Initial commit of modules to coda repository
#
#
# Revision 1.1.1.1  2003/05/09 13:07:57  dpressel
# Added modules directory (all source) to VSDK
#
#
# Revision 1.1.1.1  2003/04/25 20:21:57  kanelson
# Newest version
#
# Revision 1.1  2003/04/22 15:07:47  kanelson
# Removed rcvsk.pl and added makedist.pl
#
# Revision 1.1  2003/03/03 15:59:55  kanelson
# Updated the checkin script to put a comment header in each file when checked in.
# 
# 
# ==================== DO NOT EDIT THIS HEADER ========================
##/usr/local/bin/perl -w

#use strict;

#my $ShowFiles = 1;

$package = $ARGV[0];
$version = $ARGV[1];
$target  = $ARGV[2];
if (!$package || !$version || !$target)
{
    print "Usage: makedist.pl <package name> <version> <target platform>";
    exit;
}

system("mkdir -p dist_tmp");
system("mkdir -p dist_tmp/bin");
system("mkdir -p dist_tmp/tests");
system("mkdir -p dist_tmp/tests/shared");
system("mkdir -p dist_tmp/tests/lib");

if (-e "include")      { system("cp -r include dist_tmp"); }
if (-e "lib/$target" ) { system("cp -r lib/$target/. dist_tmp/lib"); }
if (-e "tests") 
{ 
    system("cp tests/*.cpp dist_tmp/tests");
    if (-e "tests/$target") {
        system("cp tests/$target/*exe dist_tmp/bin");
    }
    if (-e "tests/shared") {
        system("cp tests/shared/* dist_tmp/tests/shared");
    }
    if (-e "tests/lib/$target") {
        system("cp tests/lib/$target/*.so dist_tmp/tests/lib");
    }
}

Recurse("-f", "dist_tmp");
system("cd dist_tmp; tar -cvf ../$package\_$version\_$target.tar *");
system("rm -rf dist_tmp");

sub Recurse {
    
    my ($mode, $root) = @_;
    my $file = "";
    
    opendir(DIR, $root);
    my @files = grep(!/^\.\.?$/, readdir(DIR));
    closedir(DIR);
    foreach $file (@files) {
	my $pathFile = "$root/".$file;
        
	# If its a Directory, we want to go down the tree
	if (-d $pathFile) {

	    if ($pathFile =~ /CVS/ || $pathFile =~ /SunWS_cache/) {
		system("rm -rf $pathFile"); 
	    }
	    else { Recurse($mode, $pathFile); }
	}
	elsif (-f $pathFile) {
	    if ($pathFile =~ /\#/) {
		system("rm $mode $pathFile");
	    }
	}
    }
}
