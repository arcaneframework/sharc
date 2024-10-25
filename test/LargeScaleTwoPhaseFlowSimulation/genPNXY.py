import sys
import math

def decompose(a):
    root_a = math.sqrt(a)
    closest_factor = None
    min_distance = float('inf')
    
    for i in range(1, int(root_a) + 1):
        if a % i == 0:
            factor1 = i
            factor2 = a // i
            distance = abs(factor1 - root_a) + abs(factor2 - root_a)
            
            if distance < min_distance:
                min_distance = distance
                closest_factor = (factor1, factor2)
    
    return closest_factor

n_cores = int(sys.argv[1])
template_arc_file = sys.argv[2]

PN = decompose(float(n_cores))


print(PN)


with open(template_arc_file, 'r') as file:
    content = file.read()
    
NPX = str(int(PN[0]))
NPY = str(int(PN[1]))
NPZ = "1"
NP = str(n_cores)

content = content.replace("__NPX__",NPX)
content = content.replace("__NPY__",NPY)
content = content.replace("__NPZ__",NPZ);
content = content.replace("__NP__",NP);
    
arc_file = template_arc_file.replace("__NPX__",NPX)
arc_file = arc_file.replace("__NPY__",NPY)
arc_file = arc_file.replace("__NPZ__",NPZ)
arc_file = arc_file.replace("__NP__",NP)

with open(arc_file, 'w') as file:
    file.write(content)

