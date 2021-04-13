# SO-Proyekt1
Projekt nr.1 z Systemów Operacyjnych<br>
<br>
<br>
## [Temat 2 - Demon synchronizujący dwa podkatalogi:](https://cez2.wi.pb.edu.pl/moodle/mod/page/view.php?id=5248)
<pre>
Podstawowe <12pkt>:
- Demon samowywołujący się co kilka minut,
- Kopiowanie do folderu docelowego plików:
  - których w nim nie ma a są w źródłowym,
  - z późniejszą datą niż odpowiednik źródłowego w folderze docelowym.
- Jeśli znajdzie w folderze docelowym plik którego nie ma w źródłowym - usuwa go,
+ Demon musi tylko synchronizować jeden katalog bez podkatalogów,
- Natychmiastowe wywołanie demona w reakcji na sygnał systemowy (SIGUSR1),
- Demon loguje każdą swoją aktywność (wybudzenie, zaśnięcie, kopiowanie itp.),
+ Demon musi koniecznie korzystać z wywołań linux'a, nie bibliotek C,
+ Broń boże nie można kopiować całego katalogu, pliki trzeba kopiować indywidualnie,
+ Program musi być samodzielny, nie może delegować swojego działania do wywołać shell'a,

Dodatkowe <22pkt>:
-<10pkt> Synchronizacja podkatalogów (tryb jest przełączany poprzez flagę -R).
-<12pkt> Program ma 2 sposoby kopiowania dla 2 różnych rozmiarów plików:
  - dla małych używa read/write,
  - dla dużych używa mmap/write,
 - threshold pomiędzy oboma sposobami może być podawany jako argument
</pre>
## [Temat 3 - Demon(y) poszukujący plików:](https://cez2.wi.pb.edu.pl/moodle/mod/page/view.php?id=5249)
<pre>
Podstawowe <20pkt>:
- Demon samowywołujący się co kilka sekund,
- Czas samowybudzania można zespecyfikować w parametrach,
- Demon reaguje na sygnały systemowe:
  - SIGUSR1: if("demon śpi") od razu wybudza demona,
- Demon przeszukuje system plików w poszukiwaniu plików z zawierających hasła zawarte w parametrach,
+ Demon pomija katalogi i pliki do których nie ma dostępu,
- Demon loguje wyniki swoich poszukiwań do logów systemowych,
- Demon pod wpływem parametru -v szczegółowo loguje swoje poczynania,

Dodatkowe <14pkt>:
-<6pkt> reagowanie na sygnały systemowe:
  - SIGUSR1: od razu resetuje poszukiwanie,
  - SIGUSR2: przerywa obecnie trwające przeszukiwanie i usypia demona,
-<8pkt> współbieżność (nie wiem czy ktokolwiek czuje się pewnie w współbieżności).
</pre>


