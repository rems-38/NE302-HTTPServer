# Serveur HTTP

Ce projet consiste à réaliser un server HTTP _complet_.


## Librairies Issues

Quand vous voulez exécuter le programme et que vous avez une erreur de ce genre :
```
./sock: error while loading shared libraries: librequest.so.0: cannot open shared object file: No such file or directory
```

Il s'agit d'une erreur de librairie. Pour résoudre ce problème, il suffit de faire comme suit :
```
$ export LD_LIBRARY_PATH=./lib/
```

Ensuite, vous pouvez exécuter votre programme sans problème.

## FCGI Issues

Lors de la requête pour un fichier php, si vous recevez :
- un **message_body** : _File not found._  ( :warning: Différent d'une page 404)
- un **FCGI_STDERR** : _Primary script unknown_

Voici comment résoudre ce problème.

Ouvrez un terminal et entrez la suite de commande suivante :
```
$ su -
# cd /home
# chmod 755 userir
```

Veuillez ensuite réessayer de lancer votre requête !

## Makefile Issues


### Exécution d'un autre dossier

Si on veut exécuter un `Makefile` d'un dossier autre sans faire des `cd` dans tous les sens voici comment procéder.

Imaginons que nous sommes à la racine du projet et que nous voulons utiliser ce [Makefile](librequest-0.5/Makefile) (qui compile la socket). Alors nous allons faire comme suit :
>$ make -C librequest-0.5/

### Options de compilation

Si vous souhaitez modifier les options de compilation (ex pour compiler le programme pour une utilisation dans valgrind) voici comment procéder :

Il s'agit de modifier la valeur de la varible qu'on veut, afin que les options de compilation se modifie comme indiqué dans le fichier [Makefile](librequest-0.5/Makefile)

Voici comment procéder :
> $ make NOM=valeur

Ainsi si on veut exécuter notre programme avec `valgrind`, puisqu'il lui faut certaines options de compilation et pas d'autres on fait ceci :
```
$ make VALGRIND=yes
$ valgrind ./sock
```