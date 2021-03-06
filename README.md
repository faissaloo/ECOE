ECOE (ENIGMA C Object Editor)  
===  
ECOE is a fast and lightweight linux terminal based object editor for .object.GMX files.  
ECOE depends on libxml2 and ncurses (with widechar support).  
  
![Screenshot](/misc/images/Screenshot1.png)  
![Screenshot](/misc/images/Screenshot2.png)  
![Screenshot](/misc/images/Screenshot3.png)  
  
Controls
==  
Enter = select  
Tab = change sections
Arrows = scroll/move  
New event = E
New action = A
Move action down = R
Move action up = N  
ESC = Toolbar/exit window  
  
Compiling  
==  
To install use:  
```
sudo make install
```  

To compile simply use:  
```
make
```
  
To do general debugging of ECOE:  
```
make debug
```  
  
To test for memory leaks use (requires Valgrind):  
```
make memcheck
```  
  
Notes  
==  
1. ECOE allows you to have more than 11 alarms however, some other editors and implementations of GML may refuse to compile it or crash.
2. Settings are in ./src/settings.h because they won't need to be frequently changed so it's not currently worth my time to create a config system.
  
Licence  
==
**All files in this repository are licenced under version 3 of the GPL**, 
provided in the file 'LICENCE.md'.  
![GPLv3](/misc/images/GPL-logo.png)  
