ugroup
======

Fast Boolean Algebra on unordered groups/sets.

Given two arrays, A and B with unordered elements, find fastest way to produce

    A * B       An array with items that are both in A and B.
    A + B       An array containing items in both A and B, but not duplicates.
    A - B       An array containing items in A that is not in B.
    
Worst case is O( N + M ).

The algorithm is limited mostly by the speed of the malloc function.  
It works best for 64 bit computers and with lot of available memory.  
It is also safer to use on group refering to a single array than two independent.  

Memory usage depends on the maximum and minimum value of pointers to the items.  

    size = (max - min) / sizeof(type) / (64/8)

An array with 1 million 32 bit integers require 32 MB memory.  
Testing on a Macbook laptop, it takes approximately 10 seconds to process this 1024 times.  
The algebra operations on groups referring to same array require only 125 KB or 3.9 %.  
For larger data structures, this percentage is even lower.  

##Group generators

Groups are generated by slicing up an array and combined.
This is easier to work with that creating functions for generating a single group.

See 'test-ugroup' for examples.
