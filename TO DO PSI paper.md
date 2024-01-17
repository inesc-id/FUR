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




LATER:
- adicionar escrita unlogged no hashmap (e, mais tarde, no tpcc)
- no PSI, comentar READ_TIMESTAMP (para ganhar algum desempenho)
- Nos plots, Pisces "HTM commits" -> "STM commits"
- nas stats, falta abort time (psi, spht, etc)