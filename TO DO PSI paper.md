Latency profile:
- spht vs si-htm: evolucao estranha dos tx_times
- replicar para psi-strong e spht-ll

- implementar variantes psi para breakdown
. basta variantes sem as 2 primeiras otimizacoes
. podem ser apresentadas junto com os outros sistemas
. a 3ª otimizacao afeta apenas o log replayer, logo não é relevante na tx processing
. na analise de log replayer, dizer que a solucao com scans corresponde ao spht estudato; o spht com log linking teria overhead de X (referir números do artigo do SPHT)


TPCC (cenário spht):
- pisces: MUITOS aborts, ro_tx_time muito alto apesar de não haver RO!
- atenção que esta workload tem 100% updates



LATER:
- adicionar escrita unlogged no hashmap (e, mais tarde, no tpcc)
- no PSI, comentar READ_TIMESTAMP (para ganhar algum desempenho)
- Nos plots, Pisces "HTM commits" -> "STM commits"
- nas stats, falta abort time (psi, spht, etc)