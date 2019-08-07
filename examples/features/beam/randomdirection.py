import random as _rn

def make_rand_vector(dims):
    vec = [_rn.random() for i in range(dims)]
    mag = sum(x**2 for x in vec) ** .5
    return [x/mag for x in vec]

def unit_dir():
    return make_rand_vector(3)
