#ifndef SORT_HPP
#define SORT_HPP

/*
The MIT License (MIT)

Copyright (c) 2015 Fabio Vaccari

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <algorithm>
#include <iterator>

namespace futil {

/**
    \brief Insertion sort.
    
    In place, stable, works with bidiretional iterators.
    Complexity is quadratic with random input,
    linear with partially ordered input.
    
    \param first start of range, inclusive.
    \param last end of range, exclusive.
    \param comp binary function returning bool implementing strict ordering.
*/
template <class BiIter, class BinOp>
inline
void insertion_sort(BiIter first, BiIter last, BinOp comp)
{
    BiIter i, j, k;
    
    for (i = first; i != last; ++i) {
        for (j = i; j != first; --j) {
            k = std::prev(j);
            if (!comp(*j, *k)) { break; }
            std::iter_swap(j, k);
        }
    }
}

namespace {

/**
    \brief Knuth sequence.
    
    \param n upper bound, n > 1.
    \return max element of knuth sequence less than n.
*/
template <class N>
inline
N knuth_seq(N n)
{
    N k{1};
    
    while (k < n/3) { k = 3*k + 1; }
    return k;
}

/**
    \brief Interleaved insertion sort.
    
    \param first start of range, inclusive.
    \param last end of range, exclusive.
    \param h interleave, h > 0
    \param comp binary function returning bool, strict ordering.
*/
template <class RandIter, class BinOp>
inline
void h_sort(RandIter first, RandIter last,
    typename std::iterator_traits<RandIter>::difference_type h,
    BinOp comp)
{
    RandIter i, j, k;
    
    for (i = first + h; i < last; ++i) {
        for (j = i; j - first >= h; j -= h) {
            k = j - h;
            if (comp(*j, *k)) { std::iter_swap(j, k); }
        }
    }
}

} // end of anonimous namespace

/**
    \brief Shell sort.
    
    Performs Shell-sort with Knuth's sequence.
    In place, not stable.
    Complexity is O(N^(3/2)) with random input.
    
    \param first start of range, inclusive.
    \param last end of range, exclusive.
    \param comp binary function returning bool implementing strict ordering.
*/
template <class RandIter, class BinOp>
inline
void shell_sort(RandIter first, RandIter last, BinOp comp)
{
    for (auto h = knuth_seq(last - first); h > 0; h /= 3) {
        h_sort(first, last, h, comp);
    }
}

/**
    \brief Bottom up merge sort.
    
    Not in place, stable.
    Requires an auxiliary array and random access iterators.
    Complexity is logaritmic guaranteed.
    
    \param first start of range, inclusive.
    \param last end of range, exclusive.
    \param aux auxiliary array. Size of auxiliary array should be
           at least last - first.
    \param comp binary function returning bool implementing strict ordering.
*/
template <class RandIter, class BinOp>
inline
void merge_sort(RandIter first, RandIter last, RandIter aux, BinOp comp)
{
    using Diff = typename std::iterator_traits<RandIter>::difference_type;
    
    RandIter lo, mid, hi;
    Diff dim = last - first;
    Diff cutoff = 16;
    
    // cut off short segments
    for (lo = first; lo < last; lo += cutoff) {
        hi = lo + cutoff;
        
        if (last < hi) { hi = last; }
        futil::insertion_sort(lo, hi, comp);
    }
    
    for (Diff sz = cutoff; sz < dim; sz *= 2) {
        for (lo = first; lo < last - sz; lo += 2*sz) {
            mid = lo + sz;
            hi = lo + 2*sz;
            
            if (last < hi) { hi = last; }
            std::merge(lo, mid, mid, hi, aux + (lo - first), comp);
        }
        std::copy(aux, aux + dim, first);
    }
}

} // end futil

#endif
