#coding=utf-8

import sys
import codecs

s = '测试'
u = u'测试'
print type(s), repr(s), sys.getsizeof(s), len(s)
print type(u), repr(u), sys.getsizeof(u), len(u)

# s->u
print type(s.decode('utf-8')), s.decode('utf-8')

# u->s
print type(u.encode('utf-8')), u.encode('utf-8')