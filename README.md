# FUR: Fast and Unlimited Reads on Persistent Memory Transactions

In this project we develop a Persistent Transactional Memory implementation for IBM's POWER8/POWER9. The starting point is POWER's HTM implementation with, Rollback-only transactions and suspend-resume. We currently emulate Persistent Memory using spin-loops and having in account the performance of Intel Optance DC PM Persistent Memory.

# Setup

You will need a POWER8/POWER9 server. Clone the repo and run the tests (`python ./tests_py/test_*.py`).

### Citation

Please cite our work:

```
João Barreto, Daniel Castro, Alexandro Baldassin and Paolo Romano. "FUR: Fast and Unlimited Reads on Persistent Memory Transactions". EuroSys'26: Proceedings of the Twenty First European Conference on Computer Systems. 2026.
```

bibtex:

```
@inproceedings{Barreto2026EuroSys,
    author = {João Barreto and Daniel Castro and Alexandro Baldassin and Paolo Romano},
    title = {{FUR: Fast and Unlimited Reads on Persistent Memory Transactions}},
    booktitle = {{EuroSys'26: Proceedings of the Twenty First European Conference on Computer Systems}},
    pages = {1--15},
    year = {2026}
}
```
