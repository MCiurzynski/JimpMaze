# JimpMaze
Autorzy:
-Mateusz Ciurzyński,
-Borys Piątek

Program służy do znalezienia najkróteszej ścieżki w labiryncie podanym jako argument. Umożliwia również zapis labiryntu w formie binarnej i tekstowej, zapis ścieżki do pliku binarnego oraz odczyt ścieżki z pliku binarnego.

Uzycie: ./main.out [FILE] [OPTIONS]\
Program wyszukuje najkrotsza sciezke w labiryncie.\
Plik labiryntu może być w formie tekstowej lub binarnej.\
Wynik dzialania programu wypisywany jest na standardowe wyjscie\
Ponadto program moze zapisac sciezke w pliku binarnym podanym na wejsciu\
lub stworzyc plik binarny i w nim zapisac labirynt oraz sciezke jesli zostal podany plik tekstowy.\
Opcje:\
 -h	wyświetla krotki opis programu, sposob jego uzycia oraz opcjonalne parametry\
 -f	umozliwia zapis znalezionej sciezki do pliku binarnego. Mozna podac w przypadku pliku binarnego lub po podaniu opcji -b dla pliku tekstowego\
 -b	umozliwia zapis labiryntu w postaci pliku binarnego w lokalizacji podanej po parametrze. Dziala jedynie w przypadku pliku tekstowego.\
 -t	umożliwia zapis labiryntu w postaci tekstowej w lokalizacji podanej po parametrze. Dziala jednynie w przypadku pliku binarnego\
 -p	umożliwia odczyt ścieżki zapisanej w podanym labiryncie binarnym, ignoruje inne opcje\
