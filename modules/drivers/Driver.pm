package Driver;
use strict;
eval { require warnings; };
eval "use warnings;" unless $@;
use Cwd;
use POSIX qw(WEXITSTATUS);

my $TIMESTAMP_FILE_NAME = "INSTALL_SUCCEEDED";

# For documentation, run pod2text at the command line on this program

=head1 Driver Documentation

=item new(%params):
    
    Driver.pm is a package for installing a CODA driver to a module location.
We instantiate the Driver with a call to 'new', and we pass it several
mandatory hash values.
    
Required Hash Values:

    Module: The name of the CODA module to install to

    ShortName: The short name identifies this driver and is used 
    for temp names
    
    Platform: This is the platform string that comes out of configure

    Header: This is a header that we will check for

    DriverDist:  This is the name of the package, minus the tar suffix

=cut

sub new {


    my %params = @_;
    my $self = {};

    # pre-populate the values;
    while (my ($k, $v) = each(%params)) {
	$self->{$k} = $v;
    }

    
    # force the location to exist
    my $tmpPath = $self->{Where};
    my $cmd = "mkdir -p $tmpPath";
    system ($cmd) == 0
	or die "'$cmd' failed: " . POSIX::WIFEXITED($?);
        
    # now we can safely get the absolute path
    $self->{ModulesDir}    = Cwd::abs_path($self->{Where}); #Cwd::getcwd() . $params{Where};

    $self->{DriverName}    = $self->{ShortName} . " driver";
    $self->{InstallRoot}   = Cwd::abs_path($self->{InstallRoot});

    $self->{TarPrefix}     = "tar -xvf ";
    $self->{LibPath}       = $self->{InstallRoot} . "/" . $self->{Module} . "/lib/" . $self->{Platform}
                                unless defined($self->{Configure});
    $self->{IncludePath}   = $self->{InstallRoot} . "/" . $self->{Module} . "/include/"
                                unless defined($self->{Configure});
    $self->{ConfPrefix}    = "./configure --disable-shared "
                                unless defined($self->{ConfPrefix});
    $self->{MakeLine}      = "make && make install"          
                                unless defined($self->{MakeLine});
    $self->{ConfOptions}   = "" unless defined($self->{ConfOptions});
    $self->{TimestampFile} = $self->{ModulesDir} . "/" . uc($self->{ShortName}) . 
                                "_" . $TIMESTAMP_FILE_NAME;

    #use Data::Dumper;
    #print Dumper($self);

    return bless $self, __PACKAGE__;
}


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  already_exists()
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  Figures out whether the package already exists and returns
#  1 if so, 0 if not
sub already_exists {
    my $self = shift;
    return -e $self->{TimestampFile};
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  unpack()
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  Unpack the tarball that contains the driver.
#  We take the driver distro name which 
#  typically includes the name of the driver
#  and its version number, and we concat the 
#  tar to make the packed name
#  Then we just call untar with it
sub unpack {
    my $self = shift;
    if ($self->{UnpackMethod} eq "tar") {
        my $cmd = $self->{TarPrefix} . $self->{DriverDist} . ".tar";
        # look one directory up for the tarfile that will be unpacked
        # to the path ModulesDir

        #my $tmp = $self->{ModulesDir};
        # kill the last part of the modules dir
        #$tmp =~ s{/+}{/}g;        # collapse repeated '/' into single '/'
        #$tmp =~ s{^(.*)/$}{$1};   # remove trailing '/' if present
        #$tmp =~ s{^(.*/).*$}{$1}; # remove last element of path

        $self->chdir_command($cmd, $self->{ModulesDir} . "/../");
    }
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  cleanup()
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  Cleanup the tarball that contains the driver.
#  WOW, this is way too dangerous,  dont bother!
sub cleanup {
    #my $self = shift;
    #my $cmd = $self->{RmPrefix} . $self->{DriverDist} . "/";
    ##$self->command($cmd);
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  build()
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  Does a make and make install
sub build {
    my $self = shift;
    my $cmd = $self->{MakeLine} . "\n";
    $self->chdir_command($cmd);
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  command()
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  Print a command and then execute it
sub command {
    my $self = shift;
    my $cmd = shift;
    print "[" . $self->{DriverName} . "]: " . $cmd . "\n";
    system ($cmd) == 0
	or die "'$cmd' died with exit code: " . POSIX::WIFEXITED($?);

}
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  chdir_command()
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~x
#  Run command() in a different working directory
sub chdir_command {
    my $self = shift;
    my $cmd  = shift;
    my $dir  = shift || $self->{ModulesDir};
    # makefile for coda looks at stale value of $PWD
    # force it to use the directory we're cd'ing to
    local $ENV{PWD} = $dir;
    # tack a cd command onto the beginning of the command
    $cmd = "cd " . $dir . "; " .  $cmd;
    $self->command($cmd);
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  conf()
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  Configure the driver
sub conf {
    my $self = shift;
    my $cmd = "";
    if (defined ($self->{Configure})) {
        # let the caller provide their own config line
        # instead of trying to guess it
        $cmd = $self->{Configure};
    }
    else {
        # old way for stuff like pcre, expat, etc.
        if (defined $self->{CFlags}) {
            $cmd .= "CFLAGS='$self->{CFlags}'; export CFLAGS; " 
        }
        $cmd .= $self->{ConfPrefix} . 
                " --prefix=" . $self->{InstallRoot} . "/" . 
                " --libdir=" . $self->{LibPath} . 
                " --includedir=" . $self->{IncludePath} . " " .
                $self->{ConfOptions} .
                " --with-flags='" . $self->{CFlags} . "'";
    }
    $self->chdir_command($cmd);
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  timestamp()
#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#  Put a timestamp install file in the include directory
sub timestamp {
    my $self = shift;
    my $tmp = $self->{TimestampFile};
    # crappy use of global symbol 'FH' to satisfy
    # ancient versions of perl
    open (FH, ">$tmp")
        or die "$!: couldn't write " . $tmp;
    print FH scalar localtime;
    close(FH)
	or die "$!: couldn't close " . $tmp;
}


=item run():
    
    Runs the driver.  This will perform the following steps (see code for
individual functions):

    1. Unpack the data
    2. Change into driver directory
    3. Configure driver
    4. Build driver and install it
    5. Change back to base directory
    6. Remove the driver

=cut

sub run {

    my $self = shift;

    if ( $self->already_exists() ) {
	print "Found existing " . $self->{ShortName} . " driver: skipping installation.\n";
	return;
    }

    $self->unpack;
    
    $self->conf;
    
    $self->build;

    $self->cleanup;


    $self->timestamp;
    
}

1;
