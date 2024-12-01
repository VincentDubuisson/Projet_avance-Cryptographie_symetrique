import sys
import random
from queue import Queue
from threading import Thread
from math import log

RED = '\033[31m'
GREEN = '\033[32m'
BLUE = '\033[34m'
YELLOW = '\033[33m'
RESET = '\033[0m'


# fonction lecture parametre Diffie-hellman depuis un fichier 
def read_parameter(file_name):
    l = []
    with open(file_name,'r') as file :
        for param in file:
            l += [param]
        
    L1 = l[0].split(":")
    L2 = l[1].split(":")
    
    p = int(L1[1].split("\n")[0])
    g = int(L2[1].split("\n")[0])
  
    return p, g


#fct calcule a**e mod n
def pow_mod_n(a, e, n):
    return pow(a, e, n)


def alice(queue1, queue2, p, g, a, result):
    A = pow_mod_n(g, a, p)
    print(f"{GREEN}- Alice calcule A = ({g} ^ {a})[{p}] = {A}\n"
          f"\n[ALICE]\np = {p}\ng = {g}\na = {a}\nA = {A}\n"
          f"\n- Alice transmet A sur le réseaux {RESET}\n")
    queue1.put(A)  # alice envoie A

    while True:
        B = list(queue2.queue)  # alice reçoit B
        if B:
            B = B[0]
            print(f"{GREEN}\n- Alice reçoit B\n"
                  f"\n[ALICE]\np = {p}\ng = {g}\na = {a}\nA = {A}\nB = {B} {RESET}\n")
            break

    secret_key = pow_mod_n(B, a, p)
    result["alice"] = secret_key
    print(f"{GREEN}- Alice calcule la clé secrète: ({B} ^ {a})[{p}] = {secret_key}\n"
          f"\n[ALICE]\np = {p}\ng = {g}\na = {a}\nA = {A}\nB = {B}\nClé secrète = {secret_key} {RESET}\n")


def bob(queue1, queue2, p, g, b, result):
    B = pow_mod_n(g, b, p)
    print(f"{BLUE}- Bob calcule B = ({g} ^ {b})[{p}] = {B}\n"
          f"\n[BOB]\np = {p}\ng = {g}\nb = {b}\nB = {B}\n"
          f"\n- Bob transmet B sur le réseaux {RESET}\n")
    queue2.put(B)

    while True:
        A = list(queue1.queue)  # bob récupère A
        if A:
            A = A[0]
            print(f"{BLUE}- Bob reçoit A\n"
                  f"\n[BOB]\np = {p}\ng = {g}\nb = {b}\nB = {B}\nA = {A} {RESET}\n")
            break

    secret_key = pow_mod_n(A, b, p)
    result["bob"] = secret_key
    print(f"{BLUE}- Bob calcule la clé secrète: ({A} ^ {b})[{p}] = {secret_key}\n"
          f"\n[BOB]\np = {p}\ng = {g}\nb = {b}\nB = {B}\nA = {A}\nClé secrète = {secret_key}{RESET}\n")


def eve(queue1, queue2, p, g):
    print(f"{RED}- Eve intercepte les communications\n"
    f"\n[EVE]\np = {p}\ng = {g}{RESET}\n")

    while (True):
        A = list(queue1.queue) #eve essai d'intercepter A
        if (A != []):
            A = A[0]
            print(f"{RED}- Eve intercepte A depuis le réseaux\n"
            f"\n[EVE]\np = {p}\ng = {g}\nA = {A}{RESET}\n")
            break

    while (True):
        B = list(queue2.queue) #eve essai d'intercepter B 
        if (B != []):
            B = B[0]
            print(f"{RED}- Eve intercepte B depuis le réseaux\n"
            f"\n[EVE]\np = {p}\ng = {g}\nA = {A}\nB = {B}{RESET}\n")
            break 

    S = log(A * B) / log(g)
    d = log(A / B) / log(g)
    print(f"{RED}- Eve essaye de calculer la clé secrète\n"
    f"S = a+b = log(A * B) / log(g) = {S}\nd = a-b = log(A / B) / log(g) = {d}\n"
    f"\n- Eve est incapable de retrouver la clé secrète{RESET}\n")


# Vérifications de la commande
def command_control():
    if len(sys.argv) != 5:
        print(f"{RED}Usage : python3 ./dh_genkey.py -i param_file -o key_file{RESET}")
        sys.exit(1)

    param_file = None
    key_file = None

    for i in range(1, len(sys.argv), 2):
        if sys.argv[i] == '-i':
            param_file = sys.argv[i + 1]
        elif sys.argv[i] == '-o':
            key_file = sys.argv[i + 1]
        else:
            print(f"{RED}Erreur : Paramètre non reconnu '{sys.argv[i]}'. Utilisez -i pour le fichier des paramètres et -o pour le fichier de sortie.{RESET}")
            sys.exit(1)

    if not param_file or not key_file:
        print(f"{RED}Erreur : Les paramètres -i et -o sont obligatoires.{RESET}")
        sys.exit(1)

    return param_file, key_file


def main(param_file, key_file):
    p, g = read_parameter(param_file)
    print(f"{YELLOW}\n-----------------SYNTAXE-----------------\n"
          f"[NOM]\nx = ...\ny = ...\n=> variables connu par NOM\n\n- NOM action\n=> action executé par NOM\n"
          "-----------------------------------------\n"
          f"\nParamètres: p = {p} et g = {g}{RESET}\n")

    # Alice et Bob choisissent des secrets aléatoires
    a = random.randint(1, p-1)
    b = random.randint(1, p-1)
    print(f"{GREEN}- Alice choisit a = {a}\n\n"
          f"[ALICE]\np = {p}\ng = {g}\na = {a}\n"
          f"{BLUE}\n- Bob choisit b = {b}\n\n"
          f"[BOB]\np = {p}\ng = {g}\nb = {b}{RESET}\n")

    # Création file d'attente queue pour la communication et threads
    queue1 = Queue()
    queue2 = Queue()
    results = {}

    thread_alice = Thread(target=alice, args=(queue1, queue2, p, g, a, results))
    thread_bob = Thread(target=bob, args=(queue1, queue2, p, g, b, results))
    thread_eve = Thread(target=eve, args=(queue1, queue2, p, g))

    # Démarrage des threads
    thread_eve.start()
    thread_alice.start()
    thread_bob.start()

    # Attente de fin d'exécution
    thread_eve.join()
    thread_bob.join()
    thread_alice.join()

    # Vérification et écriture dans le fichier
    if results["alice"] == results["bob"]:
        with open(key_file, 'w') as file:
            file.write(f"{results['alice']}")
        print(f"{YELLOW}Clé secrète écrite dans {key_file}.{RESET}\n")
    else:
        print(f"{RED}Erreur : les clés secrètes ne correspondent pas.{RESET}\n")


if __name__ == "__main__":
    param_file, key_file = command_control()
    main(param_file, key_file)