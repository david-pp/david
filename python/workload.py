#!/usr/bin/env python
# -*- coding: utf8 -*-

import sys
import os
import xml.etree.ElementTree as ET


def svnlog(date1, date2, path=''):
    bashcmd = 'svn log --xml -r{%s}:{%s} %s' % (date1, date2, path)
    return os.popen(bashcmd).read()


def svndiff_summarize(revision, path=''):
    bashcmd = 'svn diff --xml --summarize -c%s %s' % (revision, path)
    return os.popen(bashcmd).read()


def svndiff_file_change(revision, path=''):
    bashcmd = 'svn diff -x -w -c%s %s | grep ^[+-] | wc -l' % (revision, path)
    changed = int(os.popen(bashcmd).read())
    if changed >= 2:
        return changed - 2
    else:
        return 0


###########################################

if len(sys.argv) < 3:
    print 'usage:', sys.argv[0], 'date1 date2 [path]'
    print '  eg. %s 2018-01-01 2018-01-05  https://svn.apache.org/repos/asf/subversion/trunk' % sys.argv[0]
    sys.exit(0)

date1 = sys.argv[1]
date2 = sys.argv[2]
svnpath = ''
if len(sys.argv) > 3:
    svnpath = sys.argv[3]

###########################################

excepts = {}

authors = {}

#
# Load Except List
#
exceptfile = 'workload_except.txt'
if os.path.exists(exceptfile):
    for line in open(exceptfile):
        filename = line.strip()
        if len(filename) > 0:
            excepts[filename] = ''

#
# SVN Log
#
log = svnlog(date1, date2, svnpath)

#
# Parse Log
#
tree = ET.fromstring(log)
for logentry in tree:
    revision = logentry.attrib['revision']
    author = logentry.find('author').text
    msg = logentry.find('msg').text

    if not authors.has_key(author):
        authors[author] = []

    authors[author].append({'revision': revision, 'msg': msg})

#
# Output
#
for author in authors:
    print 'Author:%s' % author
    print '--------------------------------------'

    totalchange = 0
    for commit in authors[author]:
        revision = commit['revision']
        msg = commit['msg']
        revision_change = 0
        tree = ET.fromstring(svndiff_summarize(revision, svnpath))
        for paths in tree:
            for path in paths:
                filename = path.text
                if not excepts.has_key(filename):
                    change = svndiff_file_change(revision, filename)
                    revision_change += change
                    # print path.text, change

        totalchange += revision_change
        print revision, revision_change, msg

    print '--------------------------------------'
    print 'Total: %s, +%u' % (author, totalchange)
    print ''
