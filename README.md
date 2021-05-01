# SO-Proyekt1 - Daemon synchronizing two directories
Project nr.1 in Operating Systems (Systemów Operacyjnych)<br>
Polish version of manual you can find here: [Manual.txt](/Manual.txt)<br>

#### Disclaimer:
For educational purpouses only, please don't use this daemon in 
real-life scenario, I can't take responsibility for if it explodes or something :/
<br>
<br>
(I'm not providing below text in any other language than polish...)
## [Temat 2 - Demon synchronizujący dwa podkatalogi:](https://cez2.wi.pb.edu.pl/moodle/mod/page/view.php?id=5248)
<pre>
Podstawowe <12pkt>:
- Demon samowywołujący się co kilka minut,
  - Czas samowybudzania można zespecyfikować po podaniu flagi (-t)
- Kopiowanie do folderu docelowego plików:
  - których w nim nie ma a są w źródłowym,
  - z późniejszą datą niż odpowiednik źródłowego w folderze docelowym.
- Jeśli znajdzie w folderze docelowym plik którego nie ma w źródłowym - usuwa go,
+ Demon musi tylko synchronizować jeden katalog bez podkatalogów,
- Natychmiastowe wywołanie demona w reakcji na sygnał systemowy (SIGUSR1),
- Demon loguje każdą swoją aktywność (wybudzenie, zaśnięcie, kopiowanie itp.),
+ Demon musi koniecznie korzystać z wywołań linux'a, nie bibliotek C,
+ Broń boże nie można kopiować całego katalogu, pliki trzeba kopiować indywidualnie,
+ Program musi być samodzielny, nie może delegować swojego działania do wywołań shell'a,

Dodatkowe <22pkt>:
-<10pkt> Synchronizacja podkatalogów (tryb jest przełączany poprzez flagę -R).
-<12pkt> Program ma 2 sposoby kopiowania dla 2 różnych rozmiarów plików:
  - dla małych używa read/write,
  - dla dużych używa mmap/write,
 - threshold pomiędzy oboma sposobami może być podawany jako argument
</pre>

## Treść zadania:
[12p.] Program który otrzymuje co najmniej dwa argumenty: ścieżkę źródłową oraz ścieżkę docelową . Jeżeli któraś ze ścieżek nie jest katalogiem program powraca natychmiast z komunikatem błędu. W przeciwnym wypadku staje się demonem. Demon wykonuje następujące czynności: śpi przez pięć minut (czas spania można zmieniać przy pomocy dodatkowego opcjonalnego argumentu), po czym po obudzeniu się porównuje katalog źródłowy z katalogiem docelowym. Pozycje które nie są zwykłymi plikami są ignorowane (np. katalogi i dowiązania symboliczne). Jeżeli demon (a) napotka na  nowy plik w katalogu źródłowym, i tego pliku brak w katalogu docelowym lub (b) plik w katalogu źrodłowym ma późniejszą datę ostatniej modyfikacji demon wykonuje kopię pliku z  katalogu źródłowego do katalogu docelowego - ustawiając w katalogu docelowym datę modyfikacji tak aby przy kolejnym obudzeniu nie trzeba było wykonać kopii (chyba  ze plik w katalogu źródłowym zostanie ponownie zmieniony). Jeżeli zaś odnajdzie plik w katalogu docelowym, którego nie ma w katalogu źródłowym to usuwa ten plik z katalogu docelowego. Możliwe jest również natychmiastowe obudzenie się demona poprzez wysłanie mu sygnału SIGUSR1. Wyczerpująca informacja o każdej akcji typu uśpienie/obudzenie się demona (naturalne lub w wyniku sygnału), wykonanie kopii lub usunięcie pliku jest przesłana do logu systemowego. Informacja ta powinna zawierać aktualną datę.<br>
<br>
a) [10p.]  Dodatkowa opcja -R pozwalająca na rekurencyjną synchronizację katalogów (teraz pozycje będące katalogami nie są ignorowane). W szczególności jeżeli demon  stwierdzi w katalogu docelowym  podkatalog którego brak w katalogu źródłowym powinien usunąć go wraz z zawartością.<br>
b) [12p.] W zależności  od rozmiaru plików dla małych plików wykonywane jest kopiowanie przy pomocy read/write a w przypadku dużych przy pomocy mmap/write (plik źródłowy) zostaje zamapowany w całości w pamięci. Próg dzielący pliki małe od dużych  może być przekazywany jako opcjonalny argument.<br>
<br>
Uwagi: (a) Wszelkie operacje na plikach należy wykonywać przy pomocy API Linuksa a nie standardowej biblioteki języka C (b)  kopiowanie za każdym obudzeniem całego drzewa katalogów zostanie potraktowane jako poważny błąd (c) podobnie jak przerzucenie części zadań na shell systemowy (funkcja system).<br>
