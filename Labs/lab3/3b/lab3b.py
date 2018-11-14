#!/usr/bin/python

#NAME: Lauren Fromm
#EMAIL: laurenfromm@gmail.com
#ID: 404751250

import sys
import csv
import os


errFlag = 0
testdir  = dict()


class superblock:
    def __init__(sb, line):
        sb.numBlocks = int(line[1])
        sb.numInodes = int(line[2])
        sb.blockSize = int(line[3])
        sb.inodeSize = int(line[4])
        sb.blocksPer = int(line[5])
        sb.inodesPer = int(line[6])
        sb.firstUnreserved = int(line[7])

class group:
    def __init__(grp, line):
        grp.groupNum = int(line[1])
        grp.numBlocks = int(line[2])
        grp.numInodes = int(line[3])
        grp.freeBlocks = int(line[4])
        grp.freeInodes = int(line[5])
        grp.freeBlBitmap = int(line[6])
        grp.freeInBitmap = int(line[7])
        grp.blockInodes = int(line[8])

class bfree:
    def __init__(bfr, line):
        bfr.numFree = int(line[1])

class ifree:
    def __init__(ifr, line):
        ifr.numFree = int(line[1])

class inode:
    def __init__(ino, line):
        ino.inodeNum = int(line[1])
        ino.fileType = line[2]
        ino.mode = int(line[3])
        ino.owner = int(line[4])
        ino.group = int(line[5])
        ino.linkCount = int(line[6])
        ino.ctime = line[7]
        ino.mtime = line[8]
        ino.atime = line[9]
        ino.fileSize = int(line[10])
        ino.numBlocks = int(line[11])
        ino.blocks = map(int, line[12:24])
        ino.indir = int(line[24])
        ino.doubleIndir = int(line[25])
        ino.tripleIndir = int(line[26])
        ino.test = (ino.inodeNum, ino.mode, ino.linkCount)

class directory:
    def __init__(direct, line):
        direct.parNum = int(line[1])
        direct.offset = int(line[2])
        direct.inodeNum = int(line[3])
        direct.entLength = int(line[4])
        direct.nameLength = int(line[5])
        direct.name = line[6].rstrip()

class indirect:
    def __init__(indir, line):
        indir.inodeNum = int(line[1])
        indir.level = int(line[2])
        indir.offset = int(line[3])
        indir.blockNum = int(line[4])
        indir.refNum = int(line[5])
 
def output(sb, gp, freeb, freei, inos, directs, indirects):
    blocks = {}
    holder = {}
    for i in inos:
        for o,b in enumerate(i.blocks):
            if(b != 0):
                if(b > sb.numBlocks - 1):
                    sys.stdout.write('INVALID BLOCK ' + str(b) + ' IN INODE ' +str(i.inodeNum) + ' AT OFFSET '+ str(o) + '\n' )
                    errFlag = 1
                elif (b < 8):
                    sys.stdout.write('RESERVED BLOCK '  + str(b) + ' IN INODE ' + str(i.inodeNum) + ' AT OFFSET '+ str(o)  +'\n')
                    errFlag = 1
                elif b in blocks:
                    blocks[b].append((0, o,i.inodeNum))
                else:
                    blocks[b] = [(0, o, i.inodeNum)]
        if i.indir != 0:
            if i.indir > sb.numBlocks -1 :
                sys.stdout.write('INVALID INDIRECT BLOCK ' + str(i.indir) + ' IN INODE ' + str(i.inodeNum) + ' AT OFFSET 12\n')
                errFlag = 1
            elif i.indir < 8 :
                sys.stdout.write('RESERVED INDIRECT BLOCK ' + str(i.indir) + ' IN INODE ' + str(i.inodeNum) + ' AT OFFSET 12\n')
                errFlag = 1
            elif i.indir in blocks:
                blocks[i.indir].append((1, 12, i.inodeNum))
            else:
                blocks[i.indir] = [(1, 12, i.inodeNum)]

        if i.doubleIndir != 0:
            if i.doubleIndir > sb.numBlocks -1 :
                sys.stdout.write('INVALID DOUBLE INDIRECT BLOCK ' + str(i.doubleIndir) + ' IN INODE ' + str(i.inodeNum) + ' AT OFFSET 268\n')
                errFlag = 1
            elif i.doubleIndir < 8 :
                sys.stdout.write('RESERVED DOUBLE INDIRECT BLOCK ' + str(i.doubleIndir) + ' IN INODE ' + str(i.inodeNum) + ' AT OFFSET 268\n')
                errFlag = 1
            elif i.doubleIndir in blocks:
                blocks[i.doubleIndir].append((2, 268, i.inodeNum))
            else:
                blocks[i.doubleIndir] = [(2, 268, i.inodeNum)]
                
        if i.tripleIndir != 0:
            if i.tripleIndir > sb.numBlocks -1 :
                sys.stdout.write('INVALID TRIPLE INDIRECT BLOCK ' + str(i.tripleIndir) + ' IN INODE ' + str(i.inodeNum) + ' AT OFFSET 65804\n')
                errFlag = 1
            elif i.tripleIndir < 8 :
                sys.stdout.write('RESERVED TRIPLE INDIRECT BLOCK ' + str(i.tripleIndir) + ' IN INODE ' + str(i.inodeNum) + ' AT OFFSET 65804\n')
                errFlag = 1
            elif i.tripleIndir in blocks:
                blocks[i.tripleIndir].append((3, 65804, i.inodeNum))
            else:
                blocks[i.tripleIndir] = [(3, 65804, i.inodeNum)]

    

    for ind in indirects:
        if ind.level == 0:
            tempOffset = 0
            words = ''
        elif ind.level == 1:
            tempOffset = 12
            words = ' INDIRECT'
        elif ind.level == 2:
            tempOffset = 268
            words = ' DOUBLE INDIRECT'
        elif ind.level == 3:
            tempOffset = 65804
            words = ' TRIPLE INDIRECT'
        if ind.refNum != 0:
            if ind.refNum > sb.numBlocks -1 :
                sys.stdout.write('INVALID' + words + ' BLOCK ' + str(ind.refNum) + ' IN INODE ' + str(ind.inodeNum) + ' AT OFFSET ' + str(ind.offset) + '\n' )
                errFlag = 1
            elif ind.refNum < 8 :
                sys.stdout.write('RESERVED ' + words + ' BLOCK ' + str(ind.refNum) + ' IN INODE ' + str(ind.inodeNum) + ' AT OFFSET ' + str(ind.offset) + ' \n')
                errFlag = 1
            elif ind.refNum  in blocks:
                blocks[ind.refNum].append((ind.level, ind.offset, ind.inodeNum))
            else:
                blocks[ind.refNum] = [(ind.level, ind.offset, i.inodeNum)]



    for block in range(8, sb.numBlocks):
        if block not in blocks and block not in freeb:
            sys.stdout.write('UNREFERENCED BLOCK ' + str(block) + '\n')
            errFlag = 1
        elif block in blocks and block in freeb:
            sys.stdout.write('ALLOCATED BLOCK ' + str(block) + ' ON FREELIST\n')
            errFlag = 1
        elif block in blocks and len(blocks[block]) > 1:
            holder = blocks[block]
            for level, offset, inodeNum in holder:
                if level == 0:
                    sys.stdout.write('DUPLICATE BLOCK ' + str(block) + ' IN INODE ' +  str(inodeNum) + ' AT OFFSET ' + str(offset) + '\n')
                    errFlag = 1
                if level == 1:
                    sys.stdout.write('DUPLICATE INDIRECT BLOCK ' + str(block) + ' IN INODE ' +  str(inodeNum)+ ' AT OFFSET '+ str(offset) +'\n')
                    errFlag = 1
                if level == 2:
                    sys.stdout.write('DUPLICATE DOUBLE INDIRECT BLOCK ' + str(block) + ' IN INODE ' +  str(inodeNum) + ' AT OFFSET '+ str(offset) +'\n')
                    errFlag = 1    
                if level == 3:
                    sys.stdout.write('DUPLICATE TRIPLE INDIRECT BLOCK ' + str(block) + ' IN INODE ' +  str(inodeNum)+ ' AT OFFSET '+ str(offset) +'\n')
                    errFlag = 1


def inodeOutput(sb, gp, freeb, freei, inos, directs, indirects):
    numholder = []
    numholder = freei
    inodeHolder = []



    for i in inos:
        if i.fileType == None:
            if i.inodeNum not in freei:
                sys.stdout.write('UNALLOCATED INODE ' + str(i.inodeNum) + ' NOT ON FREELIST\n')
                errFlag = 1
                numholder.append(i.inodeNum)
        else:
            if i.inodeNum in freei:
                sys.stdout.write('ALLOCATED INODE ' + str(i.inodeNum) + ' ON FREELIST\n')
                errFlag = 1
                numholder.remove(i.inodeNum)
            #sys.stdout.write('in her with inode ' + str(i.inodeNum) + '\n')

            inodeHolder.append(i)

    keys = testdir.keys()
        

    for i in range(sb.firstUnreserved, sb.numInodes):
        if i not in freei and i not in keys:
            sys.stdout.write('UNALLOCATED INODE ' + str(i) + ' NOT ON FREELIST\n')
            errFlag = 1
            numholder.append(i)

    inodeFun(numholder, inodeHolder, sb, gp, freeb, freei, inos, directs, indirects)





def inodeFun(numholder, inodeHolder, sb, gp, freeb, freei, inos, directs, indirects):
    
    #count = sb.numInodes
    inoder = {}
    
    for d in directs:
        if d.inodeNum > sb.numInodes:
            sys.stdout.write('DIRECTORY INODE ' + str(d.parNum) + ' NAME ' + d.name + ' INVALID INODE ' + str(d.inodeNum) + '\n')
            errFlag = 1
        elif d.inodeNum in numholder:
            sys.stdout.write('DIRECTORY INODE ' + str(d.parNum) + ' NAME ' + d.name + ' UNALLOCATED INODE ' + str(d.inodeNum) + '\n')
            errFlag = 1
        else:
            inoder[d.inodeNum] = inoder.get(d.inodeNum, 0) + 1
           # sys.stdout.write('hi ' + str(inoder[d.inodeNum]) + ' and '  + str(d.inodeNum) + '\n')
        

    j = 0
    for i in inodeHolder:
        #sys.stdout.write('test ' + str(i.inodeNum) + '\n')
        j += 1
        if i.inodeNum in inoder:
            if i.linkCount != inoder[i.inodeNum]:
                sys.stdout.write('INODE ' + str(i.inodeNum) + ' HAS ' + str(inoder[i.inodeNum]) + ' LINKS BUT LINKCOUNT IS ' + str(i.linkCount) + '\n')
                errFlag = 1
        else:
            if i.linkCount != 0:
                sys.stdout.write('INODE ' + str(i.inodeNum) + ' HAS 0 LINKS BUT LINKCOUNT IS ' + str(i.linkCount) + '\n')
                errFlag = 1

    child = {}
    for d in directs:
        if d.inodeNum <= sb.numInodes and d.inodeNum not in numholder:
            if(d.name != "'.'" and d.name != "'..'"):
                child[d.inodeNum] = d.parNum

    for d in directs:
        if d.name == "'.'" and d.inodeNum != d.parNum:
            sys.stdout.write('DIRECTORY INODE ' + str(d.parNum) + ' NAME ' + d.name + ' LINK TO INODE ' + str(d.inodeNum) + ' SHOULD BE ' + str(d.parNum) + '\n')
            errFlag = 1
        elif d.name == "'..'" :
            test = d.inodeNum
            if test  in child:
                if d.inodeNum != child[test]:
                    sys.stdout.write('DIRECTORY INODE ' + str(d.parNum) + ' NAME ' + d.name + ' LINK TO INODE ' + str(d.inodeNum) + ' SHOULD BE ' + str(child[test]) + '\n')
                    errFlag = 1





if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.stderr.write('Error: one argument required')
        exit(1)



    filename = sys.argv[1]
    if not os.path.isfile(filename):
        sys.stderr.write('File does not exist\n')
        exit(1)

    fd = open(filename, 'r')
    if not fd:
        sys.stderr.write('Error opening file\n')
        exit(1)
    lines = csv.reader(fd)

    sb = None
    gp = None
    freeb = []
    freei = []
    inos = []
    directs = []
    indirects = []




    for line in lines:
        if line[0] == 'SUPERBLOCK':
            sb = superblock(line)
        elif line[0] == 'GROUP':
            gp = group(line)
        elif line[0] == 'BFREE':
            freeb.append(int(line[1]))
        elif line[0] == 'IFREE':
            freei.append(int(line[1]))
        elif line[0] == 'INODE':
            inos.append(inode(line))
        elif line[0] == 'DIRENT':
            directs.append(directory(line))
        elif line[0] == 'INDIRECT':
            indirects.append(indirect(line))
        

    for i in inos:
        testdir[i.inodeNum]= i

    output(sb, gp, freeb, freei, inos, directs, indirects)
    inodeOutput(sb, gp, freeb, freei, inos, directs, indirects)
    if errFlag == 1:
        exit(2)
    else:
        exit(0)
