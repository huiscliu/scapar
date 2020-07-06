# SCAPAR: A Scalable Text Parser for Data Sciences

In data sciences industry, they are large amounts of data to process. Here SCAPAR is developed to handle these data. Data formats are designed and proper reading routines are implemented. The library is designed for Linux, Unix and Mac systems. The code is written by C.

# Build
The simplest way to install is to run commands:
```
 ./configure
 make
 sudo make install
```

The default destination is **/usr/local/scapar/**. However, the destination can be changed by using command:
```
 ./configure --prefix=DES_PATH
```

By this, the library will be installed to **DES_PATH**.

# Performance
large.c in example/ parses a large keyword file, which contains 75 million lines of data. SCAPAR can parse
the file in around 25 seconds. Its performance is around 3 million lines of data per second.

gen_perf.c generates perf.c. Command:
```
./gen_perf n
```
will generate perf.c that contains **n** integers, **n** floating-point, **n** vectors of integers, and **n** vectors of floating-point numbers. The perf.c will register all these parameters and parse these parameters from perf.dat (also generated by gen_perf).
