  
**Choosing Rank and Select Structures on Low Order Bit Vectors**  
   
Vinicius dos S. Almeida 1, Bernardo Arantes 1, Victor H. A. Marques 1,   
Kayky Gibran de O. Matos 1, Matheus Yanetti C. F. Menezes 1,   
Guilherme P. Telles 1, João V. H. Toppa 2

*1\. Instituto de Computação, UNICAMP, Campinas-SP, Brasil*  
*2\. Instituto de Matemática, Estatística e Computação Científica, UNICAMP, Campinas-SP, Brasil*

# Abstract

Bit vectors are at the core of many data structures that are widely used in Bioinformatics. Examples include mapping reads on reference genomes using an FM-index (Yang 2024), representing pangenomes (Rossi 2022), determining suffix-prefix overlaps among reads for assembly (Louza 2016), representing genomes as compact de Bruijn graphs (Bowe 2017), among others. Many operations on compact data structures are reduced to operations on bitvectors, of which rank and select are the most important. Operation rank0(B, i) returns the number of zeros in B\[1, i\] and select0(B, i) returns the index of the i-th zero in B, or \|B\|+1 if B has less than i zeros. Operations rank1 and select1 are similar. Different ways of augmenting a bitvector to solve rank and select with varying tradeoffs between time and space exist (Navarro 2016). Some works have already compared these solutions on large datasets (Gonzales 2025).  
In this ongoing work we focus on analysing the performance of some rank/select supporting structures on small bitvectors. Specifically, we are interested in determining at which bitvector size one should abandon the raw evaluation of rank/select in favour of a supporting structure, and we are also interested in determining the least number of operations that must be executed to surpass the construction time of the supporting structure. Knowing such boundaries for small bitvectors is particularly useful for the space-efficient construction of wavelet trees, which are widely used in problems related to bioinformatics, as in indexing a sequence with the FM-index. Wavelet trees store a bitvector in each node. In the root the bitvector size is equal to the sequence length, and it diminishes with depth.  
We performed experiments evaluating rank/select (i) calculating popcount on 64-bit words iteratively, (ii) performing a binary search on a vector with the integer indexes of zeros (or ones) in B, (iii) using the two-level dictionary proposed by Jacobson (Jacobson 1989\) and (iv) using the dictionary/encoding proposed by Raman et al. (Raman 2007). Starting at size 16, bitvector size was increased by 16 up to 218. Bitvector density, (the imbalance between zeros and ones), which is a factor for performance of rank and select, was selected among 0.1%, 1%, 5%, 10%, 30%, 50%, 70%, 90%, 95%, 99% e 99.9% of ones. The algorithms were implemented in C++, time was measured with the chrono library, averaged across 10 benchmark executions of 100,000 operations each, and memory was measured with the malloc\_count library.  
Preliminary results showed clear boundaries for the choice of plain bitvector, the binary search approach, Jacobson dictionaries and Raman et al. dictionaries, and how they vary for different bit vector densities, mainly for the select structure. Nevertheless, our experiments have also shown a pronounced effect of dictionary block size, with significant running time reductions when block sizes didn’t cross memory word boundaries.

Keywords: *bitvector, rank and select queries, compact data structures, low order bitvectors, space-time trade-offs.*  

# References

Bowe, A., Onodera, T., Sadakane, K. and Shibuya, T. (2017) Succinct de Bruijn graphs. In WABI, volume 7534 of Lecture Notes in Computer Science, pages 225–235. Springer.

González, R., Grabowski, S., Mäkinen, V., and Navarro, G. (2005) Practical Implementation of Rank and Select Queries. In *Proceedings of 4th International Workshop on Efficient and Experimental Algorithms*.

Jacobson, G. (1989). Succinct Static Data Structures. Ph.D. thesis, Carnegie Mellon University, Pittsburgh, PA, USA.

Louza, F.A., Gog, S., Zanotto, L., Araujo, G., Telles, G.P. (2016) Parallel Computation for the All-Pairs Suffix-Prefix Problem. In: Inenaga, S., Sadakane, K., Sakai, T. (eds) String Processing and Information Retrieval. SPIRE 2016\. Lecture Notes in Computer Science, vol 9954\. Springer Cham.

Navarro G. (2016) Compact Data Structures: A Practical Approach, Cambridge University Press.

Raman, R., Raman, V. and Satti, S. R. (2007) Succinct indexable dictionaries with applications to encoding k-ary trees, prefix sums and multisets. *ACM Transactions on Algorithms*, 3(4):43.

Rossi, M., Oliva, M., Langmead, B., Gagie, T., Boucher, C. (2022) MONI: A Pangenomic Index for Finding Maximal Exact Matches. In: Journal of Computational Biology, vol 29, pp. 169–187.

Yang, Z.-D., Kuo, H.-Y, Hsieh P.-W., Hung J.-H. (2024) Efficient construction and utilization of *k*\-ordered FM-indexes with kISS for ultra-fast read mapping in large genomes, *Bioinformatics*, vol 40, Issue 7\.  
