# ArduinoShell

Un interpreteur de commande avec système de plugin.

## Shell basis
C'est simple, la méthode `process()` est appelée à chaque boucle. Elle attends un entrée sur le port série. Un entrée valide doit avoir la format suivant :
 * `<commande> <arg1> <arg2> <...>;`
 * chaque argument est séparé par un expace
 * une commande doit finir par un point-virgule `;`

Quand `process()` trouve quelque chose suivant ce format, elle appelle chaque méthode `parse()` de chaque plugin jusqu'à ce que l'un d'eux retourne le code DONE, ce qui signifie que la commande à été prise en charge et qu'il n'y a pas besoin de chercher plus loin dans les plugins.

Si un plugin ignore une commande, pour n'importe quelle raison comme par exemple :
 * la commande est inconnue pour le plugin
 * la commande est connue mais n'a pas le bon nombre d'arguments
 * la commande est connue, a le bon nombre d'argument mais le contenu des arguments est inattendu
 * n'importe quelle autre raison qui fait que le plugin ne fait rien avec cette commande

simplement retourner le code NEXT, ce qui signifie que le plugin ne fait rien et que `process()` doit continuer de chercher.

Si aucun plugin ne prends en charge la commande, `process()` écrit "UNKNOWN" sur le port série.

La classe ShellPlugin ne prend aucune commande en charge, c'est juste une base.

## le fichier ino principal
le fichier ino principal est très simple, il initialise le port série dans `setup()` et appelle `ShellPlugin::process()` dans `loop()`. Allez jeter un oeil à ArduinoShell.ino, c'est très explicite. 

## Ecrire un plugin

#### Bien démarrer on plugin
Pour écrire un plugin, créez simplement un fichier ino dans le dossier du sketch et déclarez une nouvelle classe qui hérite de ShellPlugin. Elle doit implémenter un contructeur par défaut qui apelle le constructeur par défaut de ShellPlugin. à la suite de la déclaration, appelez la macro `REGISTER_SHELLPLUGIN(ClassName)`

L'implémentation de la classe se fera directement dans la déclaration à la manière de Java.

Exemple : 
```cpp
class BasicShell : public ShellPlugin
{
public:
    BasicShell() : ShellPlugin() {;}
    
    //     ...
    //Suite de l'implémentation, voir plus bas
    //     ...
};
REGISTER_SHELLPLUGIN(BasicShell)
```

#### La métode parse()
Chaque plugin soit implémenter la méthode publique `int parse()`. Par soucis d'élégance, une série de macros est définie pour aider à son implémentation. commencez par appeler , ensuite associez chaque commande à une function avec `ASSOC`, enfin, terminez avec `END_PARSE`.

Dans la macro `ASSOC`, il n'est pas nécessaire de placer des guillemets (`"cmd"`), écrire `cmd` suffit. Nul besoin de donner un pointeur de fonction. le nom de la fonction suffit.
`ASSOC` est simplement un raccourci vers une série de `if else`.

Exemple :
```cpp
int parse() {
    BEGIN_PARSE

    ASSOC(c,config) //commande c <pin> <mode>; configure une pin dans un mode ("DI" pour digital input, etc...)
    ASSOC(w,write)  //commande w <pin> <valeur>; écrit une valeur numérique sur une pine
    ASSOC(r,read)   //commande r <pin>; lit une entrée numérique
    ASSOC(a,aread)  //commande a <pin>; lit une entrée analogique
    ASSOC(p,pwm)    //commande p <pin> <duty cycle>; écrit un signal pwm

    END_PARSE
}
```

#### The user-defined callback function
Après avoir déclaré les associations dans `parse()`, vous devez définir des méthode pour prendre en charge chaque commande. Elles doivent impérativement retourner un `int` et ne prendre aucun argument. Je prendrais l'éxemple de la commande `w <pin> <valeur>` que j'ai définit plus haut, associée avec la fonction `write`. Le prototype de la fonction sera donc `int write()`.

la valeur de retour de la fonction doit être un appel à une des trois fonction ci-dessous:
* `done()` Si la commande a été prise en charge et n'a pas besoin de retourner de valeur à l'utilisateur. Cette fonction isncrit `DONE` sur le port série et informe `process()` que tut va bien.
* `value(SOMETHING)` Si la commande a été prise en charge et qu'elle doit retourner un résultat. Cette fonction inscrit `V=YOUR_VALUE` sur le port série et informe `process()` que tout va bien. cette fonction s'utilise comme la méthode `print` dans Arduino.
* `next()` Si la commande n'est pas prise en charge. elle informe `process()` de chercher dans le plugin suivant.

Vous pouvez utilier les méthodes et macros suivantes pour vous aider :
* `getNextArg()` retourne chaine de caractère `char*` terminée par le caractère `\0` avec l'argument suivant fournit dans la commande. Elle retourne un pointeur nul si il n'y a pas d'argument.
* `COMP(v,s)` compare `v`, la variable `char*` terminée par le caractère `\0` avec `s`. `s` n'a pas besoin de guillemet. example : `COMP(maVariable,test)`
* `GET_DPIN(v)` prend l'argument suivant dans la commande et l'interprete comme un identifiant de pin numérique. Elle déclare une variable nommée en fonction du parametre `v`. Si l'utilisateur a entré un pin analogique par exemple `A2`, la macro le converti en identifiant numérique valide. La macro retourne automatiquement `next()` si une erreur est survenue.
* `GET_APIN(v)` prend l'argument suivant dans la commande et l'interprete comme un identifiant de pin analogique. Elle déclare une variable nommée en fonction du parametre `v`. La macro retourne automatiquement `next()` si une erreur est survenue.
* `CHECK_END_ARGS` vérifie qu'il n'y a pas d'autres arguments en attentes dans la commande. Cette macro doit être appelée après que vous ayez interpreté tous les arguments dont vous avez besoin dans la fonction. De cette manière vous êtes certain d'interpreter la commande que vous attendez et pas une autre portant le même nom mais avec plus d'arguments.

voici l'exemple de la métode `write` :
```cpp
// associée avec la commande w <pin> <valeur>.
int write() {
    GET_DPIN(dpin)

    char* _value = getNextArg();

    if(!_value)
        return next();

    CHECK_END_ARGS

    int value = atoi(_value) > 0 ? HIGH : LOW;

    digitalWrite(dpin, value);

    return done();
}
```

Et ici un autre exemple retournant une valeur sur le port série :
```cpp
int aread() {
    GET_APIN(apin)
    CHECK_END_ARGS

    return value(analogRead(apin));
}
```

Allez voir le dossier `examples` pour des éxemples complets. Copiez les dans le dossier principal du sketch et compilez le tout avec l'IDE Arduino.