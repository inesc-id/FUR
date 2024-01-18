Latency profile:
- spht vs si-htm: evolucao estranha dos tx_times
- replicar para psi-strong e spht-ll

- implementar variantes psi para breakdown
. basta variantes sem as 2 primeiras otimizacoes
. podem ser apresentadas junto com os outros sistemas
. a 3ª otimizacao afeta apenas o log replayer, logo não é relevante na tx processing
. na analise de log replayer, dizer que a solucao com scans corresponde ao spht estudato; o spht com log linking teria overhead de X (referir números do artigo do SPHT)


TPCC (cenário spht -> 100% updates):
- pisces: 
    - embora seja 100% updates, há algumas txs que se consideram RO (pq a operação falha por razao legítima e a tx chega ao fim wset vazio EXPLICADO)
    - MUITOS aborts  => reduzi-os com o random back-off mas agora temos grande isolation wait (PORQUÊ AQUI MAS NÃO NO HASHMAP?)
    - ro_tx_time muito alto PORQUÊ?
    - a partir de 32 threads, upd_tx_time é inferior que o do dumbo! PORQUÊ?
    - não se observa flush_time no pisces. PORQUÊ? devia ser comparável ao do spht EXPERIMENTAR
    - o grande mistério: com tpcc 100%updates, Pisces tem desempenho péssimo. Mas com hashmap 100% updates, escala até 64 threads com isolation wait menor que dumbo. PORQUÊ?


s: 100% updates, 1 write (***mas a tx era lançada com ro=1! corrigi para ro=0***)          Pisces continua bem pior [MISTÉRIO]
d: 99.9% readonly (1 updTx per warehouse, 6-18 writes)  competitivo mas pisces pior
o: 100% readonly                                        competitivo mas pisces pior
p: 100% update, 5 writes                                Pisces mau
r: 99% update, 44-91 writes                             crash

conclusões/conjetura:
- no tpcc ou são RO (bom para dumbo) ou têm 5+ escritas (aparentemente, bom para dumbo -- pisces só é melhor com 1-2 escritas). Única exceção: "s" (ver acima).
- na R (new order), provavelmente haverá problema com o limite de alguma estrutura de dados de locking,etc. Posso tentar alterar o tpcc para que esta operação faça menos escritas => pedir ajuda ao Daniel.
- eu devia entender porque é que, com txs com muitas escritas, o pisces é tão mau (olhar para gráficos profile)


LATER:
- adicionar escrita unlogged no hashmap (e, mais tarde, no tpcc)
- no PSI, comentar READ_TIMESTAMP (para ganhar algum desempenho)
- Nos plots, Pisces "HTM commits" -> "STM commits"
- nas stats, falta abort time (psi, spht, etc)