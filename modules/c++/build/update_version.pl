eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
    if 0; # not running under some shell

my $package = $ARGV[0];
my $major_version = $ARGV[1];
my $minor_version = $ARGV[2];
my $version_suffix = $ARGV[3];

if(!$package || !$major_version)
{
  print "Usage $0 <package> <major_version> <minor_version> [version_suffix]\n";
  exit;
}

if(!$version_suffix)
{
  $version_suffix = "";
}

print "Updating " . $package;
print " to version " .  $major_version . "\." . $minor_version . 
$version_suffix . "\n";

my $packageMakefile = $package . "/build/Makefile.in";

open(MAKEFILE_IN, $packageMakefile);
@makefile_in = <MAKEFILE_IN>;
close MAKEFILE_IN;

open(MAKEFILE_IN, ">$packageMakefile");
foreach $line (@makefile_in)
{
  @kv = split(/[\s]*=[\s]*/, $line);
  if($kv[0] eq "MAJOR_VERSION")
  {
    print MAKEFILE_IN $kv[0] . " = " . $major_version . "\n";
  }
  elsif($kv[0] eq "MINOR_VERSION" && ($minor_version || $minor_version eq "0"))
  {
    print MAKEFILE_IN $kv[0] . " = " . $minor_version . "\n";
  }
  elsif($kv[0] eq "VERSION_SUFFIX")
  {
    print MAKEFILE_IN $kv[0] . " = " . $version_suffix . "\n";
  }
  else
  {
    print MAKEFILE_IN $line;
  }
}
close MAKEFILE_IN;
