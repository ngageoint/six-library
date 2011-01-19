use Driver;

@ARGV >= 1 or die "We at least need the platform name!";

my $name = "nrt";

my %params = (
    Module => "nitf",
    ShortName => $name,
    InstallRoot => shift,
    Where => shift() . "/" . $name,
    Platform => shift,
    Header => $name . ".h",
    DriverDist => $name,
    UnpackMethod => "None",
    CFlags => shift
);


my $driver = Driver::new(%params);


$driver->run;
