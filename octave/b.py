#!/bin/python  
# -*- coding: utf-8 -*-  

import os
import sys
import getopt

######################################################################

def Usage() :
	print "usage:", sys.argv[0], "[-o data.csv -b a,b,c,d..] log1,log2 ..."
	print "options: "
	print "  -h --help                 help"
	print "  -o file --output=file     output to the csv file"
	print "  -b list --behaviors=list  behaviors list seperated by comma"

config = {
	"behaviors" : [],
	"outputfile" : "",
	"files" : [],
}

def init() :
	if len(sys.argv) == 1:
		Usage()
		sys.exit()

	try :
		opts, args = getopt.getopt(sys.argv[1:], 'hb:o:', 
			['help', 'behaviors=', 'output='])
	except getopt.GetoptError:
		Usage()
		sys.exit()

	for o, a in opts:
		if o in ("-h", "--help"):
			Usage()
			sys.exit()
		elif o in ("-b", "--behaviors"):
			config["behaviors"] = a.split(',')
		elif o in ("-o", "--output"):
			config["outputfile"] = a

	config["files"] = args;
	#print config

def run() :
	for log in config["files"]:
		loadFromLogfile(log)

	if len(config['behaviors']) > 0 :
		saveToCSVfile(config['outputfile'], config['behaviors'])
	else:
		saveToCSVfile(config['outputfile'], behaviorsdesc)



######################################################################

behaviorsdesc = [
	# 上下线
	'上线',                
	'下线',
	# basic-move
	'移动',
	'切换地图',
	# basic-PK
	'击杀玩家',
	'击杀NPC',
	'被玩家击杀', 
	'被NPC击杀',
	# money and trade
	'充值', 
	'购买',
	'出售',
	'捡道具',
	'交易',
	'摆摊购买',
	'摆摊出售',
	# quest
	'接取任务',
	'注销任务',
	'完成任务',
	# mail
	'发送邮件',
	'获取附件',
	# relationship
	'聊天',
	'请求加好友',
]

behaviorcode = {
	# 上下线
	'上线'     : 1,                
	'下线'     : 2,
	# basic-move
	'移动'     : 10,
	'切换地图' : 11,
	# basic-PK
	'击杀玩家'    : 20,
	'击杀NPC'     : 21,
	'被玩家击杀'  : 22, 
	'被NPC击杀'   : 23,
	# money and trade
	'充值'     : 30, 
	'购买'     : 31,
	'出售'     : 32,
	'捡道具'   : 33,
	'交易'     : 34,
	'摆摊购买' : 35,
	'摆摊出售' : 36,
	# quest
	'接取任务' : 40,
	'注销任务' : 41,
	'完成任务' : 42,
	# mail
	'发送邮件' : 50,
	'获取附件' : 51,
	# relationship
	'聊天'       : 60,
	'请求加好友' : 61,
}

def getBehaviorCodeByDesc(desc):
	print desc
	if behaviorcode.has_key(desc):
		return behaviorcode[desc]
	return 0

######################################################################

#
# users's structure
#
# {
#   name: string,
#   behaviors:
#     {
#         move: num,
#         online : num,
#         offline : num,
#         ....
#     }
# }
#
users = {}

def loadFromLogfile(filename):
	for line in open(filename):
		fields = line.split(',')
		if len(fields) >= 3:
			charid = fields[0].strip()
			name = fields[1].strip()
			behavior = fields[2].strip()

			if not users.has_key(charid):
				users[charid] = {'name' : name,'behaviors' : {}}
			if not users[charid]['behaviors'].has_key(behavior):
				users[charid]['behaviors'][behavior] = 0

			users[charid]['behaviors'][behavior] += 1

def saveToCSVfile(filename, behaviors):
	output = sys.stdout
	if filename :
		output = open(filename, 'w')

	for id in users:
		info = id + "," + users[id]['name']
		userbs = users[id]['behaviors']
		for i in range(len(behaviors)):
			if (userbs.has_key(behaviors[i])):
				info += ",%d"%userbs[behaviors[i]]
			else:
				info += ",0"

		print >> output, info


######################################################################

init()
run()

######################################################################

