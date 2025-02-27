# default implementation: sptx_naive

# build speculative log
cd ./lib
make

# build benchmark 
make -f Makefile.ptm TX=sptx_naive -j &>> compile.output

# note
1.The malloc in benchmark and class SPLog need to be replaced with the pm malloc you are using
2.The input folder of kmeans and yada has been deleted




MEU LOG:

- usando "apt-get install" instalei g++, libtool, automake, libnuma-dev
- instalei memkind
- no /include/util.h mudei _mm_clflushopt => _mm_clflush (e provavelmente será diferente em IBM)

- export LD_LIBRARY_PATH=/usr/local/lib:~/SpeculativeLogging/lib

- TODO: substituir memkind_create_pmem por memkind_create_kind?