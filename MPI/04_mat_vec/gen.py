import numpy as np
import sys

n=int(sys.argv[1])

M=np.random.rand(n,n)
v=np.random.rand(n)
Mv=np.dot(M,v)
 
np.savetxt('M.txt' ,M, delimiter='\n')
np.savetxt('v.txt' ,v)
np.savetxt('Mv_numpy.txt',Mv, fmt='%.12f')
