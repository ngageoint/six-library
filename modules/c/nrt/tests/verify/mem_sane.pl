#!/usr/bin/perl -w

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~#
#_____M____E____M_______S____A____N____E____.____P____L____#
#  This program will check a memory trace from a nitf      #
#  library run.  The nitf library must have been compiled  #
#  with NITF_DEBUG defined, otherwise no memory trace can  #
#  be generated.                                           #
#                                                          #
#  The usage for the program is quite simple.  After you   #
#  do a run, the program will dump out a file named        #
#  memory_trace.<pid> or something along those lines.      #
#  You should run this program with that trace as an arg.  #
#                                                          #
#  (perl) mem_sane.pl <memory_trace>                       #
#__________________________________________________________#

#  Make sure the args are right
@ARGV == 1 or die "Usage $0 <memory_trace>";

#  The name of the trace
my $mem = $ARGV[0];

#  A hash of info about how the memory was allocated
#  The key is the address which was malloc'ed
my %mem_info;

#  Open the trace or die
open MEM, $mem or die "Could not open memory trace \"$mem\"";

#  Scan the trace...
while (<MEM>) {

  #  Here is where we allocated
  if (/^\tMALLOC\t(\S+)\t([0-9]+)\t(\S+)\t([0-9]+)/) {
    $mem_info{$1}[0] = $2;
    $mem_info{$1}[1] = $3;
    $mem_info{$1}[2] = $4;
  }
  #  Check if there is a corresponding deletion
  elsif (/^REQUEST: free\t\[(\S+)\]/) {

    #  If it doesnt exist, usually that just means that NITF_MALLOC wasnt
    #  used to malloc it, but NITF_FREE was
    #  that is usually not a problem
    if ( ! exists $mem_info{$1} )
    {
      print "Warning: Deleting unknown memory at $1\n";
      print "\t(This could be the result of a hash adopting which is OK)\n";
      my $questionable = <MEM>;
      if ($questionable =~ /\tFREE\t(\S+)\t([0-9]+)/)
      {
	  print "\tThe questionable free was performed in file \"$1\" at line $2\n";
      }
      else
      {
	  print "Warning: got out of sync.  This is a bug.  Sorry\n";
      }
      print "\n";
    }
    #  If there IS a corresponding free, delete this from the hash
    #  of unresolved values
    else
    {
      delete $mem_info{$1};
    }
  }

}
#  Print all of the leftover unfreed values
foreach my $loc ( keys %mem_info ) {
  my $size = $mem_info{ $loc }[0];
  my $file = $mem_info{ $loc }[1];
  my $line = $mem_info{ $loc }[2];
  print "Unfreed: [$loc]\n";
  print "\tNumber of bytes [$size]\n";
  print "\tAllocated in file \"$file\" at line [$line]\n\n";
}

close MEM;
