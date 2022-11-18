from subprocess import check_output, check_call, DEVNULL
import os
import sys
from platform import dist
from sys import stdout
from threading import Timer, Event
import itertools, sys


def thinking(evt):
    def thought():
        # https://stackoverflow.com/a/22616059/5401366
        spinner = itertools.cycle(["-", "/", "|", "\\"])
        while not evt.isSet():
            stdout.write(next(spinner))  # write the next character
            stdout.flush()  # flush stdout buffer (actual character display)
            stdout.write("\b")  # erase the last written char

    return thought


def linux_version():
    distro, version, _ = dist()
    maj_version = version.split(".")[0]
    return distro, maj_version


def swig_version():
    decoded_result = check_output(["swig", "-version"]).decode("ascii")
    return decoded_result.split("\n")[1].split(" ")[-1]


def python(args):

    stdout, stderr = (
        (sys.stdout, sys.stderr)
        if "DEBUG_PY_BINDINGS" in os.environ
        else (DEVNULL, DEVNULL)
    )

    return check_call([sys.executable] + args.split(" "), stdout=stdout, stderr=stderr)


def main():

    (distro, major_version) = linux_version()

    if distro != "centos" and major_version != "7":
        print(
            "Found non-standard os %s{}:%s{}, which may introduce subtle differences in generated code; YMMV"
            % (distro, major_version)
        )

    sv = swig_version()
    if sv != "3.0.12":
        print("Found unapproved swig version '%s'. Output may vary a great deal!" % sv)

    print("Thinking about how great cmake is...")

    evt = Event()
    thread = Timer(0.7, thinking(evt))
    thread.start()

    try:
        python("waf configure --enable-debugging --prefix=installed --enable-swig")
        print("Just")

        python("waf build")
        print("about")

    except Exception as e:
        print("Thoughts interrupted by: ")
        print(e)
    finally:
        python("waf distclean")
        print("done")

    evt.set()


if __name__ == "__main__":
    main()
