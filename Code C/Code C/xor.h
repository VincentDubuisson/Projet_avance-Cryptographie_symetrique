/*
  Applique l'opérande xor (^) sur le message 'msg' de longueur 'msg_length'
  avec la clé 'key' dans le message 'crypted'
*/
void xor(char *msg, char* key,int msg_length,char *crypted);

/*
  Génère une clé aléatoire d'une longueur donnée
*/
void gen_key (int length, char *key);

/*
  Applique le chiffrage mask sur le message msg et le renvoie dans le message 'masked'
*/
void mask(char *msg, int msg_length, char *masked);
