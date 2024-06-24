payment:
- correr neworder de novo (agora com retry policy igual) para ver se a tendencia do payment tb se verifica lá


*** rever os todos abaixo (alguns já não fazem sentido)

26/abril
Payment: 
- 100% upd, no entanto o dumbo tem vantagem! Surpreendente, pois as vantagens conhecidas do dumbo são com RO.
- Além disso, o dumbo tem mais aborts que spht! Surpreendente.
O que será a causa?
- será o facto das update txs se sincronizarem?...
- será a lógica de durability commit?


neworder:
- latency profile: nada do spht* em readonly
- dumbo-readers: STL HTM commits inferior a sPHT. (seré que tem a ver com eu ter comentado o tratamento de empty WR no commit?)
- latency profile (upd): considerando o ponto 8threads, a relação de números em absoluto de tx time não é explicável; redo log flush quase invisível; dur wait spht muito superior à do dumbo-read; spht desaparece após 16T (provavelmente ficaria melhor se incluisse SGL time e abort time?).


- Melhorias aos stacked bars de commits/aborts:
    - profiles: só spht e dumbo, juntar também abort time (e SGL time?)
    - só incluir spht e dumbo?
    - OK commits devem incluir read commits tb. retirar #aborts? Ficaria só non-tx commits, htm commits, rot commits, sgl commits.
    - eliminar os plots de tipos de aborts? ou reduzir os tipos de aborts a: tx, non-tx e capacity (tal como no si-htm)?


- no dumbo, no release_write_lock, possível otimização: caso redo log vazio, saltar a quiescence wait.
Além disso, separar esse txtime como ro tx time.

- implementar variantes psi para breakdown
    . basta variantes sem as 2 primeiras otimizacoes
    . podem ser apresentadas junto com os outros sistemas
    . a 3ª otimizacao afeta apenas o log replayer, logo não é relevante na tx processing
    . na analise de log replayer, dizer que a solucao com scans corresponde ao spht estudato; o spht com log linking teria overhead de X (referir números do artigo do SPHT)



- Debug - MISTÉRIOS:
    - capacity aborts muito raros no spht/htm.
    - pisces, o grande mistério: com tpcc 100%updates, Pisces tem desempenho péssimo. Mas com hashmap 100% updates, escala até 64 threads com isolation wait menor que dumbo. PORQUÊ?
    - mistério com SPHT com algumas operações do tpcc: parece não conseguir qq speedup, em contraste com htm.



MENOS PRIORITÁRIO:
- adicionar escrita unlogged no hashmap, rbtree (e, mais tarde, no tpcc)
- no PSI, comentar READ_TIMESTAMP (para ganhar algum desempenho)
- Latency profile: replicar para spht-ll
- contagem de cache lines para flush asinc (código do Daniel) parece dar sempre 0
- erro python quando junto aborted times nos profile plots (ver com daniel)
- on_after_htm_commit e onbeforehtmcommit devem ser chamados no htm_retry_template só em caso de HTM (não em SGL). Isto deve corrigir a stat de #commits vs #sgl do spht


MISC ANTIGO:

s: 100% updates, 1 write (***mas a tx era lançada com ro=1! corrigi para ro=0***)          Pisces continua bem pior [MISTÉRIO]
d: 99.9% readonly (1 updTx per warehouse, 6-18 writes)  competitivo mas pisces pior
o: 100% readonly                                        competitivo mas pisces pior
p: 100% update, 5 writes                                Pisces mau
r: 99% update, 44-91 writes                             crash

conclusões/conjetura:
- eu devia entender porque é que, com txs com escritas, o pisces é tão mau (olhar para gráficos profile)

exploração 18-jan para ter um mix completo:
- funcionou (e é próximo do standard mix):
./code/tpcc -w 32 -m 32 -s 0 -d 6 -o 6 -p 43 -r 45 -n 32 -t 5
- funcionou (new order -> deliver)
./code/tpcc -w 32 -m 32 -s 0 -d 50 -o 0 -p 0 -r 50 -n 32 -t 5
- não funcionou (um dos casos mais simples que encontrei)
./code/tpcc -w 1 -m 1 -s 50 -d 0 -o 0 -p 0 -r 50 -n 3 -t 5



FEITO:
- TPCC: populate new orders primeiro, antes de testes single-op
- Nos plots, Pisces "HTM commits" -> "STM commits"
