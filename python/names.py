#!/bin/python  
# -*- coding: utf-8 -*-  

import math

def find_lcs_len(s1, s2):
  m = [ [ 0 for x in s2 ] for y in s1 ]
  for p1 in range(len(s1)):
    for p2 in range(len(s2)):
      if s1[p1] == s2[p2]:
        if p1 == 0 or p2 == 0:
          m[p1][p2] = 1
        else:
          m[p1][p2] = m[p1-1][p2-1]+1
      elif m[p1-1][p2] < m[p1][p2-1]:
        m[p1][p2] = m[p1][p2-1]
      else:                             # m[p1][p2-1] < m[p1-1][p2]
        m[p1][p2] = m[p1-1][p2]
  return m[-1][-1]

def find_lcs(s1, s2):
  # length table: every element is set to zero.
  m = [ [ 0 for x in s2 ] for y in s1 ]
  # direction table: 1st bit for p1, 2nd bit for p2.
  d = [ [ None for x in s2 ] for y in s1 ]
  # we don't have to care about the boundery check.
  # a negative index always gives an intact zero.
  for p1 in range(len(s1)):
    for p2 in range(len(s2)):
      if s1[p1] == s2[p2]:
        if p1 == 0 or p2 == 0:
          m[p1][p2] = 1
        else:
          m[p1][p2] = m[p1-1][p2-1]+1
        d[p1][p2] = 3                   # 11: decr. p1 and p2
      elif m[p1-1][p2] < m[p1][p2-1]:
        m[p1][p2] = m[p1][p2-1]
        d[p1][p2] = 2                   # 10: decr. p2 only
      else:                             # m[p1][p2-1] < m[p1-1][p2]
        m[p1][p2] = m[p1-1][p2]
        d[p1][p2] = 1                   # 01: decr. p1 only
  (p1, p2) = (len(s1)-1, len(s2)-1)
  # now we traverse the table in reverse order.
  s = []
  while 1:
    print p1,p2
    c = d[p1][p2]
    if c == 3: s.append(s1[p1])
    if not ((p1 or p2) and m[p1][p2]): break
    if c & 2: p2 -= 1
    if c & 1: p1 -= 1
  s.reverse()
  return ''.join(s)

'''
if __name__ == '__main__':
  print find_lcs('abcoisjf','axbaoeijf')
  print find_lcs_len('abcoisjf','axbaoeijf')
  exit(0)
'''

class FuckPlugin():
    def __init__(self):
        self.names = {}

    def loadlog(self, filename):
        logfile = open(filename)
        line = logfile.readline()
        while line : 
            line = line.strip(' \n')
            self.names[line] = self.calVectorLen(line)
            line = logfile.readline()
        logfile.close()

    def calVectorLen(self, str):
        sum = 0.0
        for i in range(0, len(str)):
            sum += float(ord(str[i])) * float(ord(str[i]))
        return math.sqrt(sum)

    def calcDotProduct(self, str1, str2):
        result = 0.0
        for i in range(0, min(len(str1), len(str2))):
            result += float(ord(str1[i])) * float(ord(str2[i]))
        return result

    def calcSimilarity(self, str1, str2):
        if len(str1) == 0 or len(str2) == 0:
            return 0.0
        if (not self.names.has_key(str1)) or (not self.names.has_key(str2)):
            return 0.0
        return self.calcDotProduct(str1, str2) / (self.names[str1] * self.names[str2])

    def minLCSLen(self, str, strs):
        if len(strs) == 0:
            return 0
        minlen = 100000
        for i in strs:
            minlen = min(minlen, find_lcs_len(str, i))
        return minlen

    def calc(self):
        clusters = []
        tmpnames = self.names.copy()
        while len(tmpnames):
            n1,value = tmpnames.popitem()
            #print n1, value
            simnames = []

            for n2 in tmpnames:
                sim = self.calcSimilarity(n1, n2)

                if len(n1) == len(n2) \
                    and sim > 0.99 \
                    and self.levenshtein(n1, n2) > 2 \
                    and find_lcs_len(n1, n2) > 4 :

                    if (len(simnames) == 0 or (len(simnames) > 0 and self.minLCSLen(n2, simnames) > 4)):
                        simnames.append(n2)
                        print n1, n2, sim #self.minLCSLen(n2, simnames)

            for n in simnames:
                tmpnames.pop(n)

            simnames.append(n1)
            if len(simnames) > 4:
                clusters.append(simnames)

        for i in range(0, len(clusters)):
            for j in range(0, len(clusters[i])):
                print i, clusters[i][j]

        return

        for n1 in self.names:
            for n2 in self.names:
                if len(n1) == len(n2):
                    sim = self.calcSimilarity(n1, n2)
                    print n1,n2,sim 
    
    def levenshtein(self,first,second):  
        if len(first) > len(second):  
            first,second = second,first  
        if len(first) == 0:  
            return len(second)  
        if len(second) == 0:  
            return len(first)  
        first_length = len(first) + 1  
        second_length = len(second) + 1  
        distance_matrix = [range(second_length) for x in range(first_length)]   
        #print distance_matrix  
        for i in range(1,first_length):  
            for j in range(1,second_length):  
                deletion = distance_matrix[i-1][j] + 1  
                insertion = distance_matrix[i][j-1] + 1  
                substitution = distance_matrix[i-1][j-1]  
                if first[i-1] != second[j-1]:  
                    substitution += 1  
                distance_matrix[i][j] = min(insertion,deletion,substitution)  
        #print distance_matrix  
        return distance_matrix[first_length-1][second_length-1]
    
    def lcs(self,first,second):  
        first_length = len(first)  
        second_length = len(second)  
        size = 0  
        x = 0  
        y = 0  
        matrix = [range(second_length) for x in range(first_length)]  
        #print matrix  
        for i in range(first_length):  
            for j in range(second_length):  
                #print i,j  
                if first[i] == second[j]:  
                    if i - 1 >= 0 and j - 1 >=0:  
                        matrix[i][j] = matrix[i-1][j-1] + 1  
                    else:  
                        matrix[i][j] = 1  
                    if matrix[i][j] > size:  
                        size = matrix[i][j]  
                        x = j  
                        y = i  
                else:  
                    matrix[i][j] = 0  
        #print matrix  
        #print size,x,y   
  
        return second[x-size+1:x+1]  
      
if __name__ == "__main__":
    fp = FuckPlugin();
    fp.loadlog("1.txt")
    fp.calc()