#!/usr/bin/python3
import time
import subprocess
import sys
from multiprocessing.pool import ThreadPool

def sha(fname):
	return subprocess.check_output([] + str('sha256sum ' + f).split(' '))[0]

file = ' '.join(sys.argv[1:])
rm_str = 'rm'
for f in sys.argv[1:]:
	rm_str += ' t/' + f
#rm_str = [] + ['t/'+f] for f in sys.argv[1:]
print(file)
print('='*80)
print('testing timing of cp & quick copy with file {}'.format(file))
print('='*80)
subprocess.run([] + rm_str.split(' '))
subprocess.run(['sync'])
start = time.perf_counter()
p = subprocess.run([] + str('./a.out ' + file + ' t').split(' '))
print('sync')
subprocess.run(['sync'])
cpvtime = time.perf_counter() - start
print('end cpv test')


subprocess.run([] + rm_str.split(' '))
subprocess.run(['sync'])
start = time.perf_counter()
p = subprocess.run([] + str('cp ' + file +  ' t').split(' '))
print('sync')
subprocess.run(['sync'])
cptime = time.perf_counter() - start
print('end cp test')


print('time for unix cp : ' + str(cptime))
print('time for quick cp: ' + str(cpvtime))
print('checking sha265sum')
pool = ThreadPool(processes = 2)
for f in sys.argv[1:]:
	asyncres1 = pool.apply_async(sha,(f,))
	asyncres2 = pool.apply_async(sha, ('t/'+f,))
	sum1 = asyncres1.get()
	sum2 = asyncres2.get()

	if sum1 is not sum2:
		print('FAIL sha256sum missmatch')
		print(sum1)
		print(sum2)
	else:
		print('Success')
