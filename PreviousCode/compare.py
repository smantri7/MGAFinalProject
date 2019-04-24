f1 = open('output_cpp.txt', 'r')
f2 = open('output_python.txt', 'r')

c = [] 
p = []
for lines in f1.readlines():
	line = lines.split(" ")
	if(len(line) == 2):
		c.append((line[0].strip(), line[1].strip()))
	else:
		c.append((line[0].strip(), line[1].strip(), line[2].strip(), line[3].strip()))

for lines in f2.readlines():
	line = lines.split(" ")
	if(len(line) == 2):
		p.append((line[0].strip(), line[1].strip()))
	else:
		p.append((line[0].strip(), line[1].strip(), line[2].strip(), line[3].strip()))

#for i in range(len(c)):
	#if c[i] != p[i]:
		#print("MisMatch: ", i, p[i], c[i])
		#break
print(set(p) - set(c))
f1.close()
f2.close()