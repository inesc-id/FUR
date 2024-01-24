- testes: tpcc, rbtree, linked list  (tamanhos iguais aos usados com rbtree/hashmap)
- selecionar quais casos a pôr no artigo (em reunião)
- Melhorias aos stacked bars de commits/aborts:
    - profiles: só spht e dumbo, juntar também abort time (e SGL time?)
    - só incluir spht e dumbo?
    - OK commits devem incluir read commits tb. retirar #aborts? Ficaria só non-tx commits, htm commits, rot commits, sgl commits.
    - eliminar os plots de tipos de aborts? ou reduzir os tipos de aborts a: tx, non-tx e capacity (tal como no si-htm)?


- implementar variantes psi para breakdown
    . basta variantes sem as 2 primeiras otimizacoes
    . podem ser apresentadas junto com os outros sistemas
    . a 3ª otimizacao afeta apenas o log replayer, logo não é relevante na tx processing
    . na analise de log replayer, dizer que a solucao com scans corresponde ao spht estudato; o spht com log linking teria overhead de X (referir números do artigo do SPHT)

- implementar dumbo-readers

- Debug - MISTÉRIOS:
    - capacity aborts muito raros no spht/htm.
    - pisces, o grande mistério: com tpcc 100%updates, Pisces tem desempenho péssimo. Mas com hashmap 100% updates, escala até 64 threads com isolation wait menor que dumbo. PORQUÊ?
    - mistério com SPHT com algumas operações do tpcc: parece não conseguir qq speedup, em contraste com htm.



MENOS PRIORITÁRIO:
- adicionar escrita unlogged no hashmap, rbtree (e, mais tarde, no tpcc)
- no PSI, comentar READ_TIMESTAMP (para ganhar algum desempenho)
- Latency profile: replicar para spht-ll



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
