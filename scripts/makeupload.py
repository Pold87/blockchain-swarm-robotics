#!/usr/bin/python3

# this script does :
#  - make
#  - read and parse list of robots and associated controllers
#  - create a script that permits easy script lauching (e.g that does : "./controller -i con -c xml")
#  - upload xml argos file, binary of experiment and lauching script to all the robots in the username folder
#

# TODO : add methods to upload libs, to upload ssh-keys, to do stuff...

import json
from subprocess import run
import argparse
import os
import stat

p = argparse.ArgumentParser(description='maker and uploader software for e-pucks, this should be run in the "build" folder where the makefile is')
p.add_argument('-c', '--controller', help='Id of the controller you want to compile/upload (first item of the json file)', required=True)
p.add_argument('-j', '--json', help='config json file to parse to get controllers, lists of robots, options...', required=True)


def main():
    args = p.parse_args()
    try:
        run('make', check=True)
    except CalledProcessError:
        print("Build failed, see build log, stopping.")
        raise

    jsonfile = open(args.json)
    di = json.load(jsonfile)

    try:
        d = di[args.controller]
    except KeyError:
        print("Controller not found in json file, stopping.")
        raise

    fs = open('start%s.sh' % args.controller, 'w')
    fs.write(
    "#!/bin/sh\n./%s -i %s -c %s"\
    % (os.path.basename(d["bin"]),d["controller"],os.path.basename(d["xml"])))
    fs.close()

    st_file = os.stat('start%s.sh' % args.controller)
    os.chmod('start%s.sh' % args.controller, st_file.st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)

    try:
        robots = d["id"].split(',')
        for rob in robots:
            run(["scp", "start%s.sh" % args.controller, d["bin"], d["xml"],\
            "%s@%s.%s:%s" % (d["username"],d["baseip"],rob,d["uploadfolder"])], check=True)

    except Exception as e:
        print(e)
        raise


if __name__ == "__main__":
    main()
