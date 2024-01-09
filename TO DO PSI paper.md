Latency profile:
- adaptar no test_H1_H2 a parte comentada do profile plot
- spht: aproveitar instrumentacao detailed_profiling
- si-htm: idem
- pisces
- replicar para psi-strong e spht-ll



TO DO PSI paper:
- rever comentários no overleaf
- foco em SI -> generalizar
- fig arch.
- comparar commutative writers vs parallel durmarker flush

NOW:
- hashmap results -> OK
- tpcc results
- breakdown SPHT e SIHTM: análise por no. threads
hashmap, small hashmap (50K), u=10 e u=100

- implementar variantes psi para breakdown
. basta variantes sem as 2 primeiras otimizacoes
. podem ser apresentadas junto com os outros sistemas
. a 3ª otimizacao afeta apenas o log replayer, logo não é relevante na tx processing
. na analise de log replayer, dizer que a solucao com scans corresponde ao spht estudato; o spht com log linking teria overhead de X (referir números do artigo do SPHT)

- Pisces: random relay after abort; ver se melhora no tpcc-spht

LATER:
- ver misterios nos resultados hashmap (msg no skype)
- adicionar escrita unlogged no hashmap (e, mais tarde, no tpcc)
- no PSI, comentar READ_TIMESTAMP (para ganhar algum desempenho)