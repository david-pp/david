#!/usr/bin/python
# -*- coding: UTF-8 -*-

import yaml

ymlfile = file('player.yml', 'r')

# player = yaml.load(ymlfile)
# print player

class StructDescription:
    # 结构名称
    name = ''
    # 结构字段
    fields = []
    # 主键
    primary_keys = []

    def __init__(self):
        pass

    def parseByYAML(self, name, doc):
        self.name = name
        for field in doc:
            if len(field) == 1:
                field_name = field.keys()[0]
                field_attributes = field[field_name]

                if isinstance(field_attributes, str):
                    print '--simple:', name, field_name, field_attributes
                elif isinstance(field_attributes, dict):
                    out = '--dict  : ' + name + ' ' + field_attributes['type']
                    if field_attributes.has_key('size'):
                        out = out + ' ' + str(field_attributes['size'])
                    if field_attributes.has_key('comment'):
                        out = out + ' ' + field_attributes['comment']
                    print out
                else:
                    print '--error'
            else:
                print 'wront fields syntax'

def parse_ddl(docs):
    for name in docs:
        struct = StructDescription()
        struct.parseByYAML(name, docs[name])

for data in yaml.load_all(ymlfile):
    print '-------------'
    parse_ddl(data)
