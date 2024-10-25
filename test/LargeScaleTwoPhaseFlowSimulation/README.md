## Using test case
the .arc file with \_\_NPX\_\_x_\_NPY\_\_x\_\_NPZ\_\_-\_\_NP\_\_p pattern is a generic file
where the number of processor in each dimension has to be set.
\_\_NPX\_\_, \_\_NPY\_\_, \_\_NPZ\_\_ are the pattern for the number of processors in each dimension x, y and z
in the file name and in the file itself:
```
<nsd>__NPX__ __NPY__ __NPZ__</nsd>
```
You can use genPNXY.py to generates effective .arc file for a defined number of processors __NP__.
__NPZ__ will always be one, __NPX__ and __NPY__ are such as __NPX__ x __NPY__ == __NP__ and as close 
as possible to square root of __NP__.
An exemple for 720 processes:
```shell
python genPNXY.py 720 use-case-two-phase-flow-IFPSolver-2048x2048x10-dt10-nsd-__NPX__x__NPY__x__NPZ__-__NP__p.arc"
```
Will generates a file named: 

use-case-two-phase-flow-IFPSolver-2048x2048x10-dt10-nsd-24x30x1-720p.arc

with inside:
```
<nsd>24 30 1</nsd>
```