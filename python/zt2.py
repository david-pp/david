# import getpass
import paramiko
import os
import sys
import time
import traceback
import interactive

paramiko.util.log_to_file('zt2.log')

default_username = 'david'

username = 'david'
hostname = '139.196.7.121'
password = '123581321'
# password = input("Password:")

username = 'wangdawei'
hostname = '192.168.66.223'
password = input("Password:")

print password

try:
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
    ssh.connect(hostname, 22, username, str(password))

    # stdin,stdout,stderr = ssh.exec_command('0')
    # for line in stdout:
    #     print '...' + line.strip('\n')

    chan = ssh.invoke_shell()
    print(repr(ssh.get_transport()))
    print chan.getpeername()
    print('*** Here we go!\n')
    chan.send('0\r')
    chan.send('ZH13\r')
    time.sleep(3)
    chan.send('ps x\r')
    interactive.interactive_shell(chan)


    # stdin, stdout, stderr = ssh.exec_command("/sbin/ifconfig")
    # print stdout.readlines()
    # Interactive.interact(c)
    #
    # tran = ssh.get_transport()
    # chan = tran.open_session()
    # chan.get_pty()
    # print('*** Here we go!\n')
    # chan.invoke_shell()
    # chan.send('0\r')
    # chan.send('ZH13\r')
    # time.sleep(3)
    # # chan.send('ps x\r')
    # # interactive.interactive_shell(chan)
    # stdin, stdout, stderr = chan.exec_command("ps x")
    # print stdout.readlines()
    # chan.close()

    ssh.close()
except Exception as e:
    print('*** Caught exception: %s: %s' % (e.__class__, e))
    traceback.print_exc()
