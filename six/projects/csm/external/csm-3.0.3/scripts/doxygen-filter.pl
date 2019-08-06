#!/bin/sh
eval 'exec perl -x $0 ${1+"$@"}'
    if 0;  # [Construct to start `perl' when we do not know where it is.]
#!/pub/bin/perl --	 # -*-Perl-*-
##############################################################################
## FILE:              doxygen-filter
##
##    Copyright (C) 2006 Harris Corporation.  All rights reserved.
##
## CLASSIFICATION:    Unclassified
##
## DESCRIPTION:       Filters the input files to translate Origin style
##                    documentation into Doxygen style.
##
## LIMITATIONS:       Only really works on one file at a time.
##
## SOFTWARE HISTORY:
##
##  19JAN06 932D PTR#06427-IPL  S. Minster
##               Initial delivery.
##  09APR07 932D PTR#07441-IPL  S. Minster
##               Use doxygen tags instead of HTML.
##  30JUN08 956F PTR#10111-IPL  S. Minster
##               Improved filter to better mark code sections.
##  17DEC12 195G PTR#?????-IPL  S. Minster
##               Removed SW history logic.
##############################################################################

$state = 0;

my $extraClassInfo = "";
if (@ARGV)
{
  my $fullpath = $ARGV[0];
  my ($path, $file) = ($fullpath =~ /\/vobs\/\w+\/(.*\/([^\/]+))$/);

  if ($path and $file)
  {
    $extraClassInfo = "$file \"$path\"";
  }
}

my $prevWasClass = 0;
while (<>)
{
  if ($state == 0)
  {
    if ($prevWasClass)
    {
      # use "//!" for class declarations, which usually come before the class
      if (s|//\>\s*(\S+\|\n)|//\! $1|)
      {
	$state = 1;
      }
    }
    # use "//!" for comments on the first column, which usually come before the thing being documented
    elsif (s|^//\>\s*(\S+\|\n)|//\! $1|)
    {
      $state = 1;
    }
    # use "//!<" for comments on the later columns, which usually come after the thing being documented
    elsif (s|//\>\s*(\S+\|\n)|//\!\< $1|)
    {
      $state = 1;
    }

    $prevWasClass = 0;

    if (s|//\s+CLASS:\s+(\S+)|//\! \\class $1 $extraClassInfo| or
	s|//\s+STRUCT:\s+(\S+)|//\! \\struct $1 $extraClassInfo| or
	s|//\s+ENUM:\s+(\S+)|//\! \\enum $1 $extraClassInfo|)
    {
      $prevWasClass = 1;
    }
    if (m|//\*\*\*|)
    {
      # documentation blocks usually denote something later
      $prevWasClass = 1;
    }
    if (m|^\s*enum\s+|)
    {
      $prevWasClass = 1;
    }
  }
  elsif ($state == 1)
  {
    if (!s/^(\s*)\/\/( |$)/$1\/\/\!$2/)
    {
      # <pre> seems to strip C++ comments from the output
      # According to bug 315006, this should be fixed, but I saw this error in 1.5.1:
      # http://gcsd.harris.com/orgs/programs/origin/doxygen/3.11/html/d7/d22/classTimer.html#_details
      # See the bug report for the potential workaround:
      # http://bugzilla.gnome.org/show_bug.cgi?id=315006
      $state = 2 if (s|^(\s*)//\-|$1//! \\code\n$1//!|);
      $state = 0 if (s|\/\/<|\/\/\!|);
    }
  }
  elsif ($state == 2)
  {
    if (!s|//\-|//!|)
    {
      $state = 1 if (s/(\s*)\/\/( |$)/$1\/\/\! \\endcode\n$1\/\/\!$2/);
      $state = 0 if (s|//<|//\! \\endcode|);
    }
  }

  print;
}

