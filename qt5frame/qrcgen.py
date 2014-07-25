#!/usr/bin/env python

import os
import argparse
import sys

def scan(direc): 
    """
    Scan tree starting from direc
    """
    resources = []
    for path, dirs, files in os.walk(direc):
        for f in files:
            #if f.endswith(".qml"):
            resources.append(os.path.join(path,f))
    write_to_qrc(resources)
        
def write_to_qrc(resources):
    """
    Write to the qrc file under the prefix specified
    """
    with open('%s.qrc'%resname.strip('/'),'w') as f:
        f.write('<RCC>\n  <qresource prefix="%s">\n'%prefix)
        for r in resources:
            f.write('    <file>%s</file>\n'%r)
        f.write('  </qresource>\n</RCC>\n')
        
def valid_path(string):
    """
    check if the path entered is a valid one
    """
    if not os.path.isdir(string):
        msg = "%s is not a valid directory"%string
        raise argparse.ArgumentTypeError(msg)
    return string
    
if __name__=="__main__":
    parser = argparse.ArgumentParser(description='Generates a qrc (Qt resource file) from all files on a directory tree.',
        epilog='A directory.qrc file will be generated in the current directory')
    parser.add_argument('directory',metavar='directory', 
        type=valid_path,
        help='A valid path, full or local.')
    parser.add_argument('prefix',metavar='prefix',
        type=str,
        help='The prefix in the qrc file under which the resources will be available.')

    #~ parser.print_help()
    args = parser.parse_args()
    prefix = args.prefix
    resname = os.path.split(args.directory)[-1]
    scan(args.directory)
