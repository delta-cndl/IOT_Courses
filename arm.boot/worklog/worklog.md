# Introduction

This is **your worklog** when discovering programming for embedded systems. 
It should contain what you will need to reopen this project in 6 months 
or a year and be able to understand and evolve it.

Also, as an Appendix, there should be the necessary details about what is
new to you, something like a cheat sheet in some sense. Something you can 
go back to when your memory will not server you well.

# First Sprint

- [Setup](./setup.md)
- [Understanding the build](./build.md)
- [Understanding the execution](./execution.md)
- [Advanced debugging](./debugging.md)



####    Mon  worklog #######

J'ai commencé par installer les dépendances nécessaires pour exécuter le projet, en suivant les instructions du fichier [setup.md].
Ensuite, j'ai répondu aux questions du fichier [build.md] afin de mieux comprendre les différentes parties du Makefile.
J'ai également commencé à exécuter le programme et à faire du débogage avec GDB, ce qui m'a permis d'apprendre à utiliser cet outil pour suivre l'exécution pas à pas.

Étant débutant dans le domaine des systèmes embarqués, je rédige ce worklog de manière détaillée pour expliquer les concepts que je ne comprenais pas au départ.

Il m'arrive de définir d'abord les composants (variables, fichiers, périphériques, etc.) avant de répondre aux questions ou de mettre en œuvre une fonctionnalité.

Pour chaque étape, je l'associe à une version de la console, ce qui me permet de garder une trace claire de l'évolution du projet et de mes progrès.

 ## version1 : UART en polling
 ==> Il s’agit de la toute première version fonctionnelle du programme ARM exécuté sur QEMU. 
 Le but de cette version était de verifier que  :
 - l’UART fonctionne,
 - les caractères tapés au clavier arrivent bien au programme,
 - le programme est capable d'afficher manuellement les lettres reçus.

==> Ce qu'il faut comprendre ici:

Un UART (Universal Asynchronous Receiver Transmitter) est un périphérique matériel de communication série qui permet d’échanger des données bit par bit, de manière asynchrone, entre un processeur et un autre équipement (PC, terminal,...).
Il y a plusieurs UARTs :UART0 ,  UART1, UART2
UART0 désigne le premier contrôleur UART matériel, connecté au terminal série principal (console) dans QEMU.
( des explications de chatgpt)

Le programme utilise des fonctions de bas niveau pour interagir avec l’UART: 
 uart_receive (): permet de lire les caractéres tapés  depuis l’UART
 uart_send() : permet de renvoyer les caractéres tapés vers l’UART
 ex: uart_receive(uart, &c) : lit un octet depuis UART0 et stocke la valeur dans c. Elle retourne 0 s'il n’y a aucun octet disponible. 

 A ce niveau , notre programme ne s'arrete jamais ,  le CPU est utilisé en permanence, ce qui est inefficace et gaspille des ressources.

## version2: UART en polling et affichage des codes ASCII

Dans cette version, le programme n'affiche plus les caractères tapés sur le terminal, mais affiche directement leur code ASCII.

==> Ce qu'il faut comprendre:

Chaque caractère tapé sur le clavier est représenté par un octet, codé selon la table ASCII.
Voir le fichier ascii-table.webp pour mieux comprendre .
Ainsi avec l'aide de la fonction kprintf() , nous avons les codes ASCII des différentes lettres du clavier. 
remarque : le code ASCII des fléches du clavier est de 3 octets 
27 91 65 pour la fléche de haut
27 91 66 pour la fléche du bas 
27 91 68 pour la fléche de gauche 
27 91 67 pour la fléche de droite

## version3 : Console avec gestion  des entrées
Dans cette version, nous avons intégré la console, qui permet de traiter les caractères reçus via l’UART de manière plus structurée.

Nous avons commencé par implémenter les fonctions se trouvant dans le fichier console.h dans un fichier console.c qu'on a créé.

==> Ce qu'il faut comprendre

La console est représentée par une matrice de caractères [ligne][colonne].
Dans le module console.c , on gére la console et le cursuer:

 - Gestion du curseur : cursor_left, cursor_right, cursor_up, cursor_down, cursor_at, cursor_hide, cursor_show.

- Contrôle de la console : console_clear, console_color .

- console_init(callback) permet de définir une fonction callback appelée chaque fois qu’une ligne complète est saisie par l’utilisateur.

- console_echo(byte) gère les caractères reçus par l’UART.

- Ctrl-C (byte 3) : efface l’écran.

- Backspace ou DEL (byte 8 ou 127) : supprime le dernier caractère et met à jour l’affichage.

- Caractères ASCII imprimables (32–126) : ajout dans le buffer de ligne et affichage à l’écran.

==> bugg rencontrer: apres avoir ajouter de nouveaux fichiers(console.h/c) , le projet se lance mais semblait ne pas s'executer. La solution etait juste de faire augmenter la valeur de la variavle MEMSIZE dans le makefile. 

## version4 : introduction d'interruptions (IRQ)

Dans cette version, l’objectif principal est d’introduire la gestion des interruptions matérielles, en particulier celles générées par l’UART. 
Pour cela, trois nouveaux fichiers ont  été introduit:
- isr.h 
- irq.S
- isr-mmio.h 

==> Ce qu'il faut comprendre: 

Essayons de decrire le role de chacun de ses fichiers.

Le système repose sur le VIC (Vectored Interrupt Controller), qui peut gérer jusqu’à 32 interruptions matérielles (NIRQS 32).
Chaque périphérique matériel (UART) est associé à un numéro d’IRQ: 
 * UARTs
 *       UART2 IRQ = 14
 *       UART1 IRQ = 13
 *       UART0 IRQ = 12
 Le vic se charge de recevoir les signaux d'interruptions et de notifier le processeur lorsqu’une interruption active doit être traitée.

- isr-mmio.h:  Ce fichier définit les registres mémoire-mappés du VIC, à partir de l’adresse de base: #define VIC_BASE_ADDR 0x10140000. 
Dans ce fichier,  on definit les Parmi les registres importants :
VICIRQSTATUS : indique quelles IRQ actives génèrent une interruption IRQ

VICRAWSTATUS : état brut des interruptions (avant masquage)

VICINTENABLE : permet d’activer une interruption

VICINTCLEAR : permet de désactiver une interruption

VICINTSELECT : permet de choisir IRQ(=0) ou FIQ(=1)

- isr.h: Ce fichier fournit une API pour manipuler les interruptions :

    - void irqs_setup(): initialise les IRQs
    - void irqs_enable(): active les interruptions
    - void irqs_disable(): desactive les interruptions
    - void wfi(void): wait for interrupt: met le cpu en attente d'une interruption 
    - void irq_enable(uint32_t irq,void(*callback)(uint32_t,void*),void*cookie): cette fonction permet d'activer une IRQ et d'associer un callback qui sera automatiquement appelé lors de l'interruption. 
    Ducoup ses fonctions sont implémentés dans un fichier isr.c crée pour l'occasion. 

- irq.S : C'est dans ce fichier où est implémenté les mécanismes bas niveau     nécessaires à la gestion des interruptions sur processeur ARM.
il definit les fonctions essentielles:
    _irqs_setup : initialise le support des interruptions au niveau processeur.
    _irqs_enable et _irqs_disable : activent ou désactivent globalement les interruptions au niveau du processeur en modifiant le registre CPSR. 
    _wait(wait for interrupt): place le processeur dans un état de veille. 
    
Actuellement on a qu'une seule pile définit dans le fichier versatile.ld qui correspond à la stack SVC.
On doit en ajouter un autre pour les IRQs: irq_stack_top. 

==> Mon nouveau console se lance mais n'affiche rien . Ce bugg m'as prix beaucoup de temps pour l'instant et le pire c'est que c'est diffcile de savoir ou ca bloque . 



        






