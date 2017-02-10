# -*- coding: GBK -*-

import pexpect
import threading
import sys
import time


def ssh2_idc(password, idc_code, cmd):
    ssh = pexpect.spawnu('ssh wangdawei@192.168.66.223', encoding='gb2312')
    # ssh.logfile = sys.stdout
    ssh.expect(u"Password:")
    ssh.sendline(password)
    # ssh.expect(u'Select group:')
    ssh.sendline('0')
    # ssh.expect('Select page:')
    ssh.sendline(idc_code)

    ssh.expect(u"\~\]\$")
    ssh.sendline(cmd)
    ssh.expect(u"\~\]\$")
    return ssh.before
    # ssh.interact()

results = {}
result_lock = threading.Lock()

class SSHThread (threading.Thread):
    def __init__(self, password, idc_code, cmd):
        threading.Thread.__init__(self)
        self.password = password
        self.idc_code = idc_code;
        self.cmd = cmd;

    def run(self):
        print "staring ......." + self.idc_code
        result_lock.acquire()
        result = ssh2_idc(self.password, self.idc_code, self.cmd)
        results[self.idc_code] = result
        result_lock.release()
        print "Exiting ......." + self.idc_code


idcs = ['ZH11', 'ZH13', 'ZH06', 'ZH85', 'ZH01']
threads = []

dynamic_code = raw_input("DynamicCode:")
# ssh2_idc(dynamic_code, 'ZH13', "tools/monitor")

for idc in idcs:
    thread = SSHThread(dynamic_code, idc, 'tools/monitor')
    threads.append(thread)

for t in threads:
    t.start()

for t in threads:
    t.join()

for idc in idcs:
    print '-------------- ', idc, '-----------------'
    print results[idc]


# from pexpect import pxssh
# import getpass
# import time
# try:
#     s = pxssh.pxssh()
#     username = 'wangdawei'
#     hostname = '192.168.66.223'
#     password = raw_input("Password:")
#     s.login(hostname, username, password)
#     s.sendline('0')
#     s.sendline('ZH13')
#     time.sleep(3)
#     s.interact()
#     s.logout()
#
# except pxssh.ExceptionPxssh as e:
#     print("pxssh failed on login.")
#     print(e)


    #
    #
    # username = 'david'
    # hostname = '139.196.7.121'
    # password = '123581321'
    # s.login(hostname, username, password)
    # s.sendline('uptime')   # run a command
    # s.prompt()             # match the prompt
    # print(s.before)        # print everything before the prompt.
    # s.sendline('ls -l')
    # s.prompt()
    # print(s.before)
    # s.sendline('df')
    # s.prompt()
    # print(s.before)
    # s.interact()
    # s.logout()