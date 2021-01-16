A C++ Burrows Wheeler Transform Library
=======================================

This is a C++ implementation of the Burrows wheeler transform.

Overview
--------
### Burrows Wheeler Transform

The Burrows Wheeler transform is a near 1 to 1 transformation. It has many uses, in particular data compressiond and data retrieval. It's used in bzip and the DNA aligner bowtie.

Concisely, the transformation is the concatenation of the last elements of the cyclic rotations of the original sequence.

Usage
-----
The implentation uses Templates and tries to be similar to the standard C++ sort function. Because the transformation has one extra EOL character a key pointing to where the EOL should be is returned. Likewise the third argument of the decoder requires that key.

Complexity
----------
Put simply the complexity is O(NlogN2). With dictionaries that do not repeat many value the complexity would be O(NlogN).

Design Considerations
---------------------
There are papers describing better algorithms. If this is your field, I am happy for suggestions.
