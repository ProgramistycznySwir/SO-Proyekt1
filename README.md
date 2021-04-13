# SO-Proyekt1
Projekt nr.1 z Systemów Operacyjnych<br>
<br>

<pre>
Temat 2 - Demon synchronizujący dwa podkatalogi:
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

Dodatkowe:
-<10pkt> Synchronizacja podkatalogów (tryb jest przełączany poprzez flagę -R).
-<12pkt> Program ma 2 sposoby kopiowania dla 2 różnych rozmiarów plików:
  - dla małych używa read/write,
  - dla dużych używa mmap/write,
 - threshold pomiędzy oboma sposobami może być podawany jako argument
</pre>

<pre>
Temat 3 - Demon(y) poszukujący plików:
- Demon samowywołujący się co kilka sekund,
- Demon reaguje na sygnały systemowe,
- Przeszukiwanie systemu plików,
-<8pkt> współbieżność (nie wiem czy ktokolwiek czuje się pewnie w współbieżności).
</pre>


