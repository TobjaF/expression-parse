# Expression parser
folgende Ausdrücke können berechnet werden:

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
                                      
