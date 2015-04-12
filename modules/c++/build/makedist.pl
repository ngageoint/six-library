# ==================== DO NOT EDIT THIS HEADER ======================== 
# 
# $RCSfile: makedist.pl,v $
# $Revision: 1.6 $
# $Name: not supported by cvs2svn $
# $Date: 2005-12-06 00:29:15 $
# $Author: abeaucha $
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
# Revision 1.5  2005/11/22 21:16:17  abeaucha
#
#
# changed script so that test executables are not copied to root of bin folder
#
# Revision 1.4  2005/11/15 22:49:18  abeaucha
# changed script so that test executables are properly installed
#
# Revision 1.3  2005/04/28 19:28:24  jrandolp
# Added line to install plugins dir
#
# Revision 1.2  2004/06/01 17:04:49  jrandolp
# Made the dist slightly different
#
# Revision 1.1.1.1  2004/01/05 16:03:22  dpressel
# Imported new build directory
#
#
# Revision 1.1.1.1  2003/07/24 12:31:01  dpressel
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

print "target is: $target\n";

system("mkdir -p dist_tmp");
system("mkdir -p dist_tmp/bin");
system("mkdir -p dist_tmp/bin/$target");
#system("mkdir -p dist_tmp/plugins");
system("mkdir -p dist_tmp/tests");
system("mkdir -p dist_tmp/tests/shared");
system("mkdir -p dist_tmp/tests/lib");

if (-e "include")      { system("cp -r include dist_tmp"); }

if (-e "lib/$target" ) {
    system("mkdir -p dist_tmp/lib/$target");
    
    use Cwd 'abs_path';
    my $temp_target_dir = abs_path();
    $temp_target_dir .= "/dist_tmp/lib/$target";

    # backticks to ensure we get the shell
    `( cd ./lib/$target; tar cf - . | ( cd $temp_target_dir && tar xfB - ) )`;
    #system("cp -r lib/. dist_tmp/lib");
}
#if (-e "lib/$target" ) { system("cp -r lib/. dist_tmp/lib"); }

if (-e "plugins/$package/$target") { system("cp -r plugins/. dist_tmp/plugins"); }
if (-e "tests") 
{ 
    system("cp tests/*.cpp dist_tmp/tests");
    if (-e "tests/$target") {
        system("cp -r tests/$target/* dist_tmp/bin/$target");
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
