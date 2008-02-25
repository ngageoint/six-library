# ==================== DO NOT EDIT THIS HEADER ======================== 
# 
# $RCSfile: rfk.pl,v $
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
# Revision 1.1.1.1  2003/04/21 17:16:57  kanelson
# Initial version of scripts.
#
# Revision 1.1  2003/04/10 20:47:45  dpressel
# Added rfk
#
# Revision 1.1  2003/03/03 15:59:55  kanelson
# Updated the checkin script to put a comment header in each file when checked in.
# 
# 
# ==================== DO NOT EDIT THIS HEADER ========================
# ------------ DO NOT EDIT THIS HEADER: IT IS AUTO-GENERATED ----------  
#   
# $RCSfile: rfk.pl,v $  
# $Revision: 1.1.1.1 $  
# $Name: not supported by cvs2svn $  
# $Date: 2003-12-18 15:19:26 $  
# $Author: dpressel $  
#   
# Copyright (c) 2002 Veridian Systems Division  
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
# Revision 1.1.1.1  2003/04/21 17:16:57  kanelson
# Initial version of scripts.
#
# Revision 1.1  2003/04/10 20:47:45  dpressel
# Added rfk
#
# Revision 1.2  2002/11/05 18:27:20  dpressel
# Want to ditch carriage returns
#
# Revision 1.1  2002/10/24 13:56:47  kanelson
# Moved all current scripts to new directory.
#
# Revision 1.1  2002/10/15 17:50:12  kanelson
# Moved all scripts to a new scripts/ directory.
#  
# ------------ DO NOT EDIT THIS HEADER: IT IS AUTO-GENERATED ----------  
###!/usr/bin/perl -w
##########################################################
# This file is the personal property of Daniel Pressel.
# It is available for all purposes and may be modified if
# necessary.
##########################################################

##/usr/local/bin/perl -w

use strict;
# RFK - recursively delete all of the stuff emacs leaves behind
# all object files, and all logs
# Usage : ./rfk [-i|-f] <directory>

my $ShowFiles = 1;

if (@ARGV == 0) { Recurse("-i", "."); }
elsif (@ARGV == 2) {
    if ($ARGV[0] =~ /-f/) {
	Recurse("-f", $ARGV[1]);
    }
    else { 
	Recurse("-i", $ARGV[1]);
    }
}
else { 
    if ($ARGV[0] =~ /-f/) {
	Recurse("-f", ".");
    }
    elsif ($ARGV[0] =~ /-i/) {
	Recurse("-i", ".");
    }
    else {
	Recurse("-i", $ARGV[0]); 
    }
}

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
	    # Hack for Solaris
	    if ($pathFile =~ /SunWS_cache/)
	    {

		print "Deleting cache: $pathFile\n" unless ($ShowFiles == 0);
		system("rm -rf $pathFile"); 
		
	    }
	    elsif ($pathFile =~ /autom4te-*/)
	    {
		print "Deleting autoconf cache: $pathFile\n" unless
                   ($ShowFiles == 0);
		system("rm -rf $pathFile");
	    }
	    else { Recurse($mode, $pathFile); }
	}
	elsif (-f $pathFile) {
	    if ($pathFile =~ /~$/ || $pathFile =~ /\.o$/ 
		|| $pathFile =~ /\#/ || $pathFile =~ /\.log$/
		|| $pathFile =~ /\.LOG$/ || $pathFile =~ /core$/
		|| $pathFile =~ /\.out$/) {
		print "Deleting file: $pathFile\n" unless ($ShowFiles == 0 || $mode eq "-i");
		system("rm $mode $pathFile");
	    }
	}
    }
}
