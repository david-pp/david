# -*- coding: GB2312 -*-
import pexpect
import thread
import threading
import time

def ssh_ali(cmd):
    username = 'wangdawei'
    hostname = '192.168.66.223'
    # password = raw_input("Password:")

    # ssh = pexpect.spawnu('ssh wangdawei:%s@192.168.66.223' % password)
    ssh = pexpect.spawn('ssh david@139.196.7.121')
    ssh.expect("david@139.196.7.121's password:")
    ssh.sendline('123581321')
    ssh.expect("\~\]\$")
    ssh.sendline(cmd)
    ssh.expect("\~\]\$")
    print ssh.before
    # ssh.interact()


class SSHThread (threading.Thread):
    def __init__(self, id, name, cmd):
        threading.Thread.__init__(self)
        self.threadid = id;
        self.name = name;
        self.cmd = cmd;

    def run(self):
        print "staring ......." + self.name
        ssh_ali(self.cmd)
        print "Exiting ......." + self.name


threads = []

for i in range(0, 5):
    thread = SSHThread(1, "thread1", 'ls')
    threads.append(thread)

for t in threads:
    t.start()
