#  Copyright (C) 2008 by Josh Matthews <josh@joshmatthews.net>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the
#  Free Software Foundation, Inc.,
#  59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

"""
stringcheck.py - a tool to report all incorrectly hardcoded strings
                 in c++ source files.

Acceptable hardcoded strings include:

#include "string"
Error::Warning("string")
Error::Verbose("string")
Error::Exception("string")
_NOL10N("string")

The _NOL10N() construct is used when the string will NEVER, EVER
need localization, such as blank strings (" ").
"""

import re
import os
import stat
import sys

def checkLine(line):
    if line.startswith("#include"):
        return []

    untagged = []
    errorRE = re.compile("Error::(?:Verbose|Warning|Exception)\((.*)\);")
    taggedRE = re.compile('_NOL10N\("(.*)"\)')
    untaggedRE = re.compile('"(.*)"')
    
    tagged = taggedRE.findall(line)
    errors = errorRE.findall(line)
    
    allowed = []
    if errors:
        allowed.append(errors)
    if tagged:
        allowed.append(tagged)
    
    possiblyUntagged = untaggedRE.findall(line)
    
    for string in possiblyUntagged:
        flagUsage = True
        for valid in allowed:
            for item in valid:
                if string in item:
                    flagUsage = False
                    break
        if flagUsage:
            untagged.append(string)
    return untagged

def checkFile(filename):
    fh = open(filename)
    for linenum, line in enumerate(fh):
        untagged = checkLine(line)
        for string in untagged:
            print('%s:%s Untagged string "%s"' % (filename, linenum + 1, string))
    fh.close()

def main(argc, argv):
    if argc != 2:
        print "usage: %s [source path/file]" % os.path.basename(argv[0])
        print "written by Josh Matthews, 2008\n"
        print "Report all uses of hardcoded strings that are not:"
        print "- part of an error message"
        print "- tagged with _NOL10N()"
        return 1
    
    if not stat.S_ISDIR(os.stat(argv[1])[0]):
        checkFile(argv[1])
    else:
        for root, dirs, files in os.walk(argv[1]):
            for file in [f for f in files if f.endswith(".cpp")]:
                checkFile(root + file)
    return 0

if __name__ == "__main__":
    sys.exit(main(len(sys.argv), sys.argv))