#!/bin/bash

redis-cli PUBLISH push_1 '{"type":1,"id":"1024","title":"测试","content":"消息测试"}'
