import re

def translate(value, leftMin, leftMax, rightMin, rightMax):
    # Figure out how 'wide' each range is
    leftSpan = leftMax - leftMin
    rightSpan = rightMax - rightMin

    # Convert the left range into a 0-1 range (float)
    valueScaled = float(value - leftMin) / float(leftSpan)

    # Convert the 0-1 range into a value in the right range.
    return rightMin + (valueScaled * rightSpan)

re_exp = r"#|\("

with open("colors.dat") as fin:
    inlist = [re.split(re_exp, line) for line in fin]
[e.pop(1) for e in inlist]
inlist = [[s.strip(')\n').strip('\t').strip() for s in e] for e in inlist]
# inlist = [[s.strip('\t') for s in e] for e in inlist]
inlist = [[s.split('/')[0] for s in e] for e in inlist]
# inlist = [[s.strip() for s in e] for e in inlist]
outdict = dict(inlist)
for k, v in outdict.items():
    outdict[k] = [translate(int(e), 0, 255, 0, 1) for e in list(v.split(','))]

with open("colors_translated.dat", 'w') as fout:
    for k, v in outdict.items():
        outstr = '{' + '\"' + k + '\"' + ',' + str(v).replace('[','{').replace(']', '}') + '}' + ',' + '\n'
        fout.write(outstr)

