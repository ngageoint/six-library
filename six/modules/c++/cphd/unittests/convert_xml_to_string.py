import os, sys

# Reads in the xml file
def convert_to_string(path_in, path_out):
    if not os.path.isfile(path_in):
        print("Input file provided was not found")
        exit(1)

    with open(path_in, 'r') as f:
        data = f.readlines()

    for ii in range(0,len(data)):
        data[ii] = "\"" + data[ii].rstrip() + "\\n\"" + "\n"

    with open(path_out, 'w+') as f:
        f.writelines(data)

if __name__ == '__main__':
    argList = sys.argv[1:]
    print(argList)
    convert_to_string(argList[0], argList[1])