Ispas Alexandra-Petrina
336CA
alexandra.ispas@stud.acs.upb.ro

								Tema 1 EGC

Pentru implementare am folosit header-ul 'Transform2D' din laborator
si functiile de transformare fereastra-poarta.
'Helper.h' contine informatii despre jucator si are functii de de creare
a unor obiecte 2D: 'CreateCircle' intoarce mesh-ul corespunzator unui cerc
(functia nu este implementata de mine, am gasit-o pe internet), functia
'CreateItem' realizeaza simbolurile folosite pentru bonus, iar 'CreateRectangle'
este preluata din laboratorul 3, dar am modificat-o incat sa primeasca si inaltimea
ca parametru, astfel produce si dreptunghiuri, nu doar patrate.

Structurile din 'Tema.h' sunt:
	- ViewportSpace si LogicSpace sunt cele din laboratorul 3
	- Enemy contine informatii despre fiecare inamic. Daca acesta este implicat 
	  intr-o coliziune cu jucatorul, campul 'collision' devine true, iar acesta 
	  nu se mai afiseaza
	- Obstacle contine informatii despre obstacole. Acestea sunt adaugate manual,
	  la pozitii fixate de mine. (Am incercat sa le pun random, dar imi era greu
	  sa pastrez un offset acceptabil de mare intre obstacole si acestea sa nu se
	  suprapuna).
	- Projectile contine informatii despre un proiectil. Retin si pozitia initiala
	  a acestuia pentru a tine cont de puterea armei jucatorului. (In implementare
	  calculez distanta Euclidiana intre pozitia initiala si cea curenta pentru a
	  verifica acest lucru).
	- Item stocheaza detalii despre diverse pick-up-uri folosite in implementare


Pick-up-uri:
	- toate apar la pozitii random
	- cel verde apare odata la 10 secunde si creste parametrul health
	- cel rosu apare odata la 10 secunde si creste viteza, dar viteza scade in
		update, in functie de timp, pentru a ajunge la valoarea initiala.
	- cel galben apare odata la 15 secunde si creste scorul jucatorului

Health bar, score bar - 2 dreptunghiuri, iar cel din mijloc este scalat relativ 
						la valoarea maxima a acestui parametru 
						Score bar = stanga sus, health bar = dreapta sus
Scorul se mai afiseaza si in consola atunci cand jucatorul omoara un inamic.

Player-ul este alcatuit din mai multe meshe suprapuse si se afiseaza in fucntia
'RenderPlayer'. 
Proiectilele apar in momentul in care se apasa pe mouse si se tine cont de fire
rate. Acestea se deplaseaza in functie de unghiul dintre jucator si mouse in momentul
in care a fost lansat proiectilul.
Inamicii apar odata la 3 secunde si urmaresc jucatorul in scena. Sunt afisati in
functia 'RenderEnemy'

In functia 'OnInputUpdate', inainte de a deplasa jucatorul in orice directie, mai 
intai verific daca va avea loc o coliziune cu vreun obstacol sau cu peretii scenei.

Jocul se incheie atunci cand jucatorul nu mai are vieti, iar health a ajuns la 0 sau
daca se apasa tasta L.

