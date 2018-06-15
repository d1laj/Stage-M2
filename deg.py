import numpy as np

D = np.array([[1], [0,2]]);

def add(A,B,n):
    C = A + B
    C= np.append([0], C);
    while C.size < n:
        C = np.append(C,0);
    C[n-1] += 1;
    return C;

def f(D, n):
    mid1 = (n-1)//2;
    mid2 = (n-1) - mid1;
    D1 = D[mid1-1];
    D2 = D[mid2 -1];
    #print(D);
    D3 = add(D1,D2,n);
    print(D3);
    D = np.append(D,0);
    D[n-1] = D3;
    return D

def prog(D, n):
    for i in range(n+1):
        if i >= 3:
            D= f(D, i);
        print(D);
        

prog(D, 5);
