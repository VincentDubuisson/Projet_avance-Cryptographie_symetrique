import sys
import random
from queue import Queue
from threading import Thread

# finction lecture parametre Diffie-hellman depuis un fichier 
def lire_parametre(nomFichier):
    with open(nomFichier,'r') as file :
        params = file.readline()
        print( params.split(":"))
        p =str(params[0].split(":")[1].strip()) 
        g =int(params[1].split(":")[1].strip())
          
    return p , g

#fct calcule a**e mod n
def puissance_mod_n(a,e,n):
    return pow(a,e,n)

def alice(queue,p,g,a):
    A=puissance_mod_n(g,a,p)
    queue.put(A) #alice envoi A
    print("alice envoi A = {A}")
    B = queue.get() #alice recoit B
    shared_key = puissance_mod_n (B,a,p)
    print("alice calcule la cle partager: {shared_key}")

def bob(queue,p,g,b):
    A=queue.get()#bob recupere A
    B=puissance_mod_n(g,b,p)
    queue.put(B)
    print("bob envoi B = {B}")
    shared_key = puissance_mod_n (A,b,p)
    print("bob calcule la cle partager: {shared_key}")

def main(param_fichier):
    p,g=lire_parametre(param_fichier)
    print("parametre : p =  {p} ,g = {g} ")

    #alice et bob choisissent des secret aleatoir 
    a=random(1,p-1)
    b=random(1,p-1)
    print("alice choisie a = {a} et bob b={b}")

#creation file d'attente queue pour la comunication et thread pour l'execution de alice et bob en parallele
    queue=Queue()
    thread_alice=Thread(target=alice,args=(queue,p,g,a))
    thread_bob=Thread(target=bob,args=(queue,p,g,b))
#demarage et attente fin execution des thread
    thread_alice.start()
    thread_bob.start()
    thread_alice.join
    thread_bob.join

if __name__ == "__main__":
    if len(sys.argv) <2:
        print("usage : python3 dh_genkey.py <param_fichier>")
        sys.exit(1)

    param_fichier = sys.argv[2]
    main(param_fichier)





    

    

