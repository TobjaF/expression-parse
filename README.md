# Expression parser
Wir haben uns im Modul 411 dazu entschieden, einen Parser in C zu programmieren, welcher verschiedene mathematische Rechnungen berechnen kann. Dabei ist es egal ob es sich um eine Multiplikation, Division, Summe oder Differenz handelt. Des Weiteren ist es auch möglich, Ausdrücke auszurechnen, die aus mehreren Rechnungen zusammensetzen, beispielsweise 34*(9+2). 

## EBNF
Beim BNF handelt es sich um die Backus Naur Form. Dabei handelt es sich um ein Set von grammatischen Regeln, welche sich rekursiv wieder aufrufen. Die EBNF, also die Extended Backus Naur Form, bietet zusätzliche Syntax, ist vom Prinzip her aber das gleiche wie die BNF. Bei der EBNF gibt es die {}, diese geschweiften Klammern bedeutet, dass der Inhalt entweder 0-Mal oder beliebig oft mal wiederholt werden kann. Die eckigen Klammern [] bedeuten, dass der Inhalt 0 oder 1 Mal wiederholt wird. Die vertikale Linie | bedeutet 'oder'.

Beim Expression Parser können folgende Ausdrücke berechnet werden:

ziffer = 0|1|2|3|4|5|6|7|8|9 \
zahl = ziffer {ziffer} \
faktor = zahl | ( summe ) \
produkt = faktor {* faktor} \
summe = produkt {+ produkt} 

Aus diesen Regeln ergibt sich für die Berechnung von 34*(2+1) folgender parse Baum

                                |summe
                               34*(2+1)
                                |
                                |produkt
                              34*(2+1)
                             /        \
                            |faktor    |faktor
                            34        (2+1)
                            |           |
                            |zahl       |( summe )
                           34         (2+1)
                        /     \         |
                    ziffer   ziffer     |summe
                       3       4        2+1
                                       /       \
                                      |produkt  |produkt
                                      2         1
                                      |faktor   |faktor
                                      2         1
                                      |         |
                                      |zahl     |zahl
                                      2         1
                                      |         |
                                    ziffer    ziffer
                                      2         1
                                      
Dabei ist zu beachten, dass jede Berechnung eine Summe ist, welche sich aus einem Produkt zusammensetzt. Dieses Produkt besteht aus mindestens einem Faktor, kann aber auch aus mehreren Faktoren bestehen. Ein Faktor wiederum setzt sich aus einer Zahl oder einer weiteren Summe zusammen. Die Zahl besteht aus mindestens einer Ziffer von 0 bis 9. Es ist wichtig, dass kein Schritt übersprungen wird. Deshalb setzt sich die Summe in der Klammer nicht nur aus zwei Ziffern zusammen, sondern man muss verstehen, dass diese Ziffern Zahlen sind, welche Faktoren darstellen um ein Produkt zu erstellen aus welchem schlussendlich eine Summe ensteht.

## Unser Parser
Der Expression Parser wurde aus dem Modul 403 übernommen. Dort gab es noch Probleme bezüglich der Verarbeitung der Eingabe. Es konnte keine Summe gebildet werden, wenn es zweistellige Zahlen waren und das Programm erkannte auch nicht, wenn es nach der geschlossenen Klammer nochmals eine Klammer gab.
Wir überlegten und, wie wir diese Probleme lösen sollten. Die Idee war, das wir einen tokenizer erstellen, der die einzelnen Teile der Eingabe in ein Array schreibt, welches aus dem token_type besteht und aus dem value. 
Mit Hilfe des Tokenizer war es schlussendlich möglich, alle Operationen richtig auszurechnen, egal wie der Ausdruck aufgenbaut war.
Mit den Erweiterungen, die Tobja zu Hause noch gemacht hat, ist es nun sogar möglich, einen Ausdruck in einzugeben, welcher dann in einen Funktions-Ausdruck und sogar einen HTML-Baum compiliert wird. So ist unser Expression Parser schlussendlich beim Compiler angelangt.
