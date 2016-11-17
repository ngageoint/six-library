
import sys
from datetime import datetime
from pysix.six_base import DateTime

def convertToPython():
    original = DateTime()
    converted = original.toPythonDateTime()
    try:
        assert original.getYear() == converted.year
        assert original.getMonth() == converted.month
        assert original.getDayOfMonth() == converted.day
        assert original.getHour() == converted.hour
        assert original.getMinute() == converted.minute
        assert int(original.getSecond()) == converted.second
    except AssertionError as error:
        print(error)
        return False
    return True

def convertFromPython():
    original = datetime.now()
    converted = DateTime.fromPythonDateTime(original)
    try:
        assert converted.getYear() == original.year
        assert converted.getMonth() == original.month
        assert converted.getDayOfMonth() == original.day
        assert converted.getHour() == original.hour
        assert converted.getMinute() == original.minute
        assert int(converted.getSecond()) == original.second
    except AssertionError as error:
        print(error)
        return False
    return True


def run():
    if convertToPython() and convertFromPython():
        print("Tests passed")
        sys.exit(0)
    sys.exit(1)

if __name__ == '__main__':
    run()

