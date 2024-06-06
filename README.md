# Serveur HTTP

Ce projet consiste à réaliser un server HTTP _complet_.


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