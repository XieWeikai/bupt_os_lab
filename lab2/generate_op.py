from random import randint
from random import choice
from sys import argv

used = set()
all = set([i for i in range(10000)])

def gen_alloc():
    pos = choice(list(all - used))
    num = randint(0,10000)
    size = randint(0,1024)
    used.add(pos)
    print(f'alloc {pos} {num} {size}')

def gen_free():
    pos = choice(list(used))
    used.discard(pos)
    print(f'free {pos}')

def gen_check():
    pos = choice(list(used))
    print(f'check {pos}')

if __name__ == '__main__':
    n = 5000

    if len(argv) >= 2:
        n = int(argv[1])

    op = [gen_alloc,gen_free,gen_check]

    print(n)

    for _ in range(n):
        op_num = 0
        if len(used) > 0:
            op_num = randint(0,2)
        if len(used) == 10000:
            op_num = randint(1,2)
        op[op_num]()


