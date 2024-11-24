import sys
import random
from queue import Queue
from threading import Thread
from math import log

# finction lecture parametre Diffie-hellman depuis un fichier 
def lire_parametre(nomFichier):
    l=[]
    with open(nomFichier,'r') as file :
        for param in file:
            l+=[param]
        
    
    L1=l[0].split(":")
    L2=l[1].split(":")
    
    p=int(L1[1].split("\n")[0])
    g=int(L2[1].split("\n")[0])
  
          
    return p , g

#fct calcule a**e mod n
def puissance_mod_n(a,e,n):
    return pow(a,e,n)

def alice(queue1,queue2,p,g,a):
    A=puissance_mod_n(g,a,p)
    queue1.put(A) #alice envoi A
    print("alice envoi A =", A)
    while (True):
        B = list(queue2.queue) #alice recoit B 
        if (B != []):
            B=B[0]
            print("alice a recu B :",B)
            break       
    shared_key = puissance_mod_n (B,a,p)
    print("alice calcule la cle partager:", shared_key)
    
def bob(queue1,queue2,p,g,b):
   
    B=puissance_mod_n(g,b,p)
    queue2.put(B)
    print("bob envoi B =", B)
    while (True):
        A=list(queue1.queue)#bob recupere A
        if (A !=[]):
            A=A[0]
            print ("bob a recu A:",A)
            break
    shared_key = puissance_mod_n (A,b,p)
    print("bob calcule la cle partager:",shared_key)
    

def eve(queue1,queue2,p,g):
    print(" eve connai les valeur p:",p," et g:",g)
    print ("eve intersepte la comunication")
    while (True):
        A=list(queue1.queue)#eve essai d'intercepter A
        if (A !=[]):
            A=A[0]
            print ("eve intersepte A=",A)
            break
    while (True):
        B = list(queue2.queue) #eve essai d'intercepter B 
        if (B!=[]):
            B=B[0]
            print("eve intersepte B=",B)
            break 
    print("S = a+b et d = a-b")  
    S=log(A*B)/log(g)
    d= log(A/B)/log(g)
    print (S,d)
    
    print ("eve est incapable de retrouver la cle partager, a ni b")


def main(param_fichier):
    p,g=lire_parametre(param_fichier)
    print("parametre : p =",  p ,"g =", g )

    #alice et bob choisissent des secret aleatoir 
    a=random.randint(1,p-1)
    b=random.randint(1,p-1)
    print("alice choisie a =", a ,"et bob b=",b)

#creation file d'attente queue pour la comunication et thread pour l'execution de alice et bob en parallele
    queue1=Queue()
    queue2=Queue()
    thread_alice=Thread(target=alice,args=(queue1,queue2,p,g,a))
    thread_bob=Thread(target=bob,args=(queue1,queue2,p,g,b))
    thread_eve=Thread(target =eve,args=(queue1,queue2,p,g))
#demarage et attente fin execution des thread
    thread_alice.start()
    thread_bob.start()
    thread_eve.start()
    thread_bob.join
    thread_alice.join
    thread_eve.join()

if __name__ == "__main__":
    if len(sys.argv) <2:
        print("usage : python3 dh_genkey.py <param_fichier>")
        sys.exit(1)

    param_fichier = sys.argv[2]
    main(param_fichier)





    

    

