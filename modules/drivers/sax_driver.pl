use Driver;

@ARGV >= 1 or die "We at least need the platform name!";

my $name = "expat";
my $version = "2.0.0";

my %params = (
    Module => "xml.lite",
    ShortName => $name,
    InstallRoot => shift,
    Where => shift() . "/" . $name . "-" . $version,
    Platform => shift,
    Header => $name . ".h",
    DriverDist => $name . "-" . $version,
    UnpackMethod => "tar",
    CFlags => shift
);


my $driver = Driver::new(%params);


$driver->run;
