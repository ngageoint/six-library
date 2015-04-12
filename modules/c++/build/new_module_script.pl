#!/usr/bin/perl -w

use strict;
use File::Copy "mv"; 
use File::Find; 
use File::Path;
use Tie::File;

# --  subroutines -- 
sub usage;
sub substitute_by_type;
sub substitute_projname;
sub make_dir_structure;
sub add_includes;
sub str2file;


# -- main -- 

# autoflush
$|++;
# nobody else can read the files we are creating
umask 077;

# arguments
usage() unless @ARGV == 2;
my ($NAME, $TYPE) = @ARGV;
die "Bad project type: $TYPE"
    unless ($TYPE eq "so" or $TYPE eq "lib");
my @PARTS = split /\./, $NAME;

# actual work
make_dir_structure($NAME);
add_includes($NAME, @PARTS);
substitute_projname($NAME);
substitute_by_type($NAME, $TYPE);
make_task_skeleton($NAME, @PARTS) if $TYPE eq "so";

# -- helper routines --

sub usage {
    die "usage: $0 <module_name> <so|lib>\n";
}

# make a skeleton task for the plugin version
sub make_task_skeleton {
    my $name  = shift;
    my @parts = @_;
    my $file  = "./$name/source/SampleTask.cpp";

    # go parts -1 deep w/ the namespacing
    my @cut = @parts[0..$#parts-1];
    my $namespace_start = join "",
                          map { "namespace $_\n{\n" }
                          @cut;
    my $namespace_end   = join "",
                          map { "}\n"; }
                          @cut;

    # set up the namespace qualified name
    my $qual_name = join "::", (@cut, ucfirst $parts[-1]);

    # set up the 
    my $uc_qual_name = uc $qual_name;
    $uc_qual_name =~ s/::/_/g;

    # Nice class name for the task
    my $task_name = ucfirst $parts[-1];

    my %pats = (
        __NAMESPACE_START__   => $namespace_start,
        __NAMESPACE_END__     => $namespace_end,
        __QUAL_NAME__         => $qual_name,
        __TASK_NAME_UC_QUAL__ => $uc_qual_name,
        __TASK_NAME__         => $task_name
    );

    substitute_pats($file, %pats);
    mv($file, "./$name/source/$task_name.cpp");
}

# setup stuff for .a or for .so
sub substitute_by_type {
    my $name = shift;
    my $type = "." . shift();

    # File::Find
    find(sub
    {
        # use the sample task for so files
        unlink $_ if ($_ eq "SampleTask.cpp" && $type eq ".lib");
        
        # filter out files that don't concern us
        return unless /^(.*make.*(?:.so|.lib))$/ig;

        my $name = $1;
        # if the name matches the type
        #   set the file to the canonical one
        #   (ie project.make.in.so => project.make.in)
        # if not
        #   nuke the file
        if ($name =~ /^(.*)$type$/g) {
            my $tmp = $1;
            mv($name, $tmp);
        }
        else {
            unlink $name;
        }
    },
    "./$name");
}

# substitute the project name into project.make.in file
sub substitute_projname {
    my $name = shift;
    my $file = "./$name/build/project.make.in";
    my %pats = (__NAME => $name);
    substitute_pats($file, %pats);
}

# do some variable substitution on a file
sub substitute_pats {
    my $file = shift;
    my %pats = @_;
    my @file = ();
    tie @file, 'Tie::File', $file
        or die "$! could not read $file!!";
    for my $line (@file) {
        for my $pat (keys %pats) {
           $line =~ s/$pat/$pats{$pat}/g;
        }
    }
    untie @file;
}

# create the include directories
sub add_includes {
    my $root  = shift;
    my @parts = @_;

    # nuke the existing directory structure
    rmtree("./$root/include", { verbose => 0 });

    # make the directory structure for the imports
    my $include_prefix = "./$root/include/";

    # create the import dirs
    my @import_dirs = ();
    push @import_dirs, $include_prefix . "import/" . join "/", @parts[0..$#parts-1];
    push @import_dirs, $include_prefix . join "/", @parts;

    # make the directories
    map{ mkpath($_, { verbose => 1 , mode => 0700 }) } @import_dirs;

    # create the import header
    my $define = join "_", map {uc} @parts;
    $define = "__${define}_H__";
    my $import_hdr = "#ifndef $define\n" .
                     "#define $define\n" .
                     "#endif  // $define\n";

    # create the file
    str2file($import_dirs[0] . "/" . $parts[-1] . ".h", $import_hdr);
}

# put a string into a file
sub str2file {
    my $file = shift;
    my $str  = shift;

    open my $fh, ">", $file
        or die "$! could not open $file for write";

    print $fh $str;

    close $fh
        or die "$! could not close $file";
}


# dump the __DATA__ section to a temporary file,
# and extract it to the current directory
sub make_dir_structure {
    my $name    = shift;
    my $tarFile = $name . ".tgz";

    open my $fh, ">", $tarFile
        or die "$! couldn't open $tarFile for write";

    # write the file as binary data
    local $/;
    local $\;
    binmode($fh);
    my $tmp = <DATA>;
    syswrite $fh, $tmp;
    close $fh
        or die "$! couldn't close $tarFile after write";

    system("tar -xzf ./$tarFile");
    unlink "./$tarFile";
    mv("./module_template", "./$name");
}


# Begin Binary Data
__DATA__
