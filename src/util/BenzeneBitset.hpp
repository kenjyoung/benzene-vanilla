//----------------------------------------------------------------------------
/** @file BenzeneBitset.hpp
    STL std::bitset extended with is_subset_of() and is_less_than().
    
    @todo Uses a lot of gcc builtins. Make this more portable?
 */
//----------------------------------------------------------------------------

#ifndef BENZENE_BITSET
#define BENZENE_BITSET

#include <cstddef>              
#include <string>
#include <bits/functexcept.h>   

#include "Benzene.hpp"
#include "BenzeneException.hpp"

_BEGIN_BENZENE_NAMESPACE_

//----------------------------------------------------------------------------

#define _GLIBCXX_BITSET_BITS_PER_WORD  (__CHAR_BIT__ * sizeof(unsigned long))
#define _GLIBCXX_BITSET_WORDS(__n) \
 ((__n) < 1 ? 0 : ((__n) + _GLIBCXX_BITSET_BITS_PER_WORD - 1) \
                  / _GLIBCXX_BITSET_BITS_PER_WORD)

/**
 *  Base class, general case.  It is a class invariant that _Nw will be
 *  nonnegative.
 *
 *  See documentation for bitset.
 */
template<size_t _Nw>
struct _Base_bitset
{
    typedef unsigned long _WordT;
    
    /// 0 is the least significant word.
    _WordT 		_M_w[_Nw];
    
    _Base_bitset()
    { _M_do_reset(); }
    
    _Base_bitset(unsigned long __val)
    {
	_M_do_reset();
	_M_w[0] = __val;
    }
    
    static size_t
    _S_whichword(size_t __pos )
    { return __pos / _GLIBCXX_BITSET_BITS_PER_WORD; }
    
    static size_t
    _S_whichbyte(size_t __pos )
    { return (__pos % _GLIBCXX_BITSET_BITS_PER_WORD) / __CHAR_BIT__; }
    
    static size_t
    _S_whichbit(size_t __pos )
    { return __pos % _GLIBCXX_BITSET_BITS_PER_WORD; }
    
    static _WordT
    _S_maskbit(size_t __pos )
    { return (static_cast<_WordT>(1)) << _S_whichbit(__pos); }
    
    _WordT&
    _M_getword(size_t __pos)
    { return _M_w[_S_whichword(__pos)]; }
    
    _WordT
    _M_getword(size_t __pos) const
    { return _M_w[_S_whichword(__pos)]; }
    
    _WordT&
    _M_hiword()
    { return _M_w[_Nw - 1]; }
    
    _WordT
    _M_hiword() const
    { return _M_w[_Nw - 1]; }
    
    void
    _M_do_and(const _Base_bitset<_Nw>& __x)
    {
	for (size_t __i = 0; __i < _Nw; __i++)
            _M_w[__i] &= __x._M_w[__i];
    }
    
    void
    _M_do_or(const _Base_bitset<_Nw>& __x)
    {
	for (size_t __i = 0; __i < _Nw; __i++)
            _M_w[__i] |= __x._M_w[__i];
    }
    
    void
    _M_do_xor(const _Base_bitset<_Nw>& __x)
    {
	for (size_t __i = 0; __i < _Nw; __i++)
            _M_w[__i] ^= __x._M_w[__i];
    }
    
    void
    _M_do_sub(const _Base_bitset<_Nw>& __x)
    {
        for (size_t __i = 0; __i < _Nw; __i++)
            _M_w[__i] &= ~__x._M_w[__i];
    }
    
    void
    _M_do_left_shift(size_t __shift);
    
    void
    _M_do_right_shift(size_t __shift);
    
    void
    _M_do_flip()
    {
	for (size_t __i = 0; __i < _Nw; __i++)
            _M_w[__i] = ~_M_w[__i];
    }

    void
    _M_do_set()
    {
	for (size_t __i = 0; __i < _Nw; __i++)
            _M_w[__i] = ~static_cast<_WordT>(0);
    }

    void
    _M_do_reset()
    { __builtin_memset(_M_w, 0, _Nw * sizeof(_WordT)); }
    
    bool
      _M_is_equal(const _Base_bitset<_Nw>& __x) const
    {
	for (size_t __i = 0; __i < _Nw; ++__i)
            if (_M_w[__i] != __x._M_w[__i])
                return false;
	return true;
    }

    //////////////////////////////////////////
    // added by broderic
    bool
    _M_is_subset_of(const _Base_bitset<_Nw>& __x) const
    {
        for (size_t __i = 0; __i < _Nw; ++__i)
            if (_M_w[__i] & ~__x._M_w[__i])
                return false;
        return true;
    }

    bool
    _M_is_less_than(const _Base_bitset<_Nw>& __x) const
    {
        for (size_t __i = 0; __i < _Nw; ++__i)
            if (_M_w[__i] != __x._M_w[__i])
                return (_M_w[__i] < __x._M_w[__i]);
        return false;
    }

    //////////////////////////////////////////

    size_t
    _M_are_all_aux() const
    {
	for (size_t __i = 0; __i < _Nw - 1; __i++)
            if (_M_w[__i] != ~static_cast<_WordT>(0))
                return 0;
	return ((_Nw - 1) * _GLIBCXX_BITSET_BITS_PER_WORD
		+ __builtin_popcountl(_M_hiword()));
    }
    
    bool
    _M_is_any() const
    {
	for (size_t __i = 0; __i < _Nw; __i++)
            if (_M_w[__i] != static_cast<_WordT>(0))
                return true;
	return false;
    }
    
    size_t
    _M_do_count() const
    {
	size_t __result = 0;
	for (size_t __i = 0; __i < _Nw; __i++)
            __result += __builtin_popcountl(_M_w[__i]);
	return __result;
    }
    
    unsigned long
    _M_do_to_ulong() const;

    // find first "on" bit
    size_t
    _M_do_find_first(size_t __not_found) const;

    // find the next "on" bit that follows "prev"
    size_t
    _M_do_find_next(size_t __prev, size_t __not_found) const;
};

// Definitions of non-inline functions from _Base_bitset.
template<size_t _Nw>
void
_Base_bitset<_Nw>::_M_do_left_shift(size_t __shift)
{
    if (__builtin_expect(__shift != 0, 1))
	{
            const size_t __wshift = __shift / _GLIBCXX_BITSET_BITS_PER_WORD;
            const size_t __offset = __shift % _GLIBCXX_BITSET_BITS_PER_WORD;

            if (__offset == 0)
                for (size_t __n = _Nw - 1; __n >= __wshift; --__n)
                    _M_w[__n] = _M_w[__n - __wshift];
            else
                {
                    const size_t __sub_offset = (_GLIBCXX_BITSET_BITS_PER_WORD 
                                                 - __offset);
                    for (size_t __n = _Nw - 1; __n > __wshift; --__n)
                        _M_w[__n] = ((_M_w[__n - __wshift] << __offset)
                                     | (_M_w[__n - __wshift - 1] >> __sub_offset));
                    _M_w[__wshift] = _M_w[0] << __offset;
                }

            std::fill(_M_w + 0, _M_w + __wshift, static_cast<_WordT>(0));
	}
}

template<size_t _Nw>
void
_Base_bitset<_Nw>::_M_do_right_shift(size_t __shift)
{
    if (__builtin_expect(__shift != 0, 1))
	{
            const size_t __wshift = __shift / _GLIBCXX_BITSET_BITS_PER_WORD;
            const size_t __offset = __shift % _GLIBCXX_BITSET_BITS_PER_WORD;
            const size_t __limit = _Nw - __wshift - 1;

            if (__offset == 0)
                for (size_t __n = 0; __n <= __limit; ++__n)
                    _M_w[__n] = _M_w[__n + __wshift];
            else
                {
                    const size_t __sub_offset = (_GLIBCXX_BITSET_BITS_PER_WORD
                                                 - __offset);
                    for (size_t __n = 0; __n < __limit; ++__n)
                        _M_w[__n] = ((_M_w[__n + __wshift] >> __offset)
                                     | (_M_w[__n + __wshift + 1] << __sub_offset));
                    _M_w[__limit] = _M_w[_Nw-1] >> __offset;
                }
	  
            std::fill(_M_w + __limit + 1, _M_w + _Nw, static_cast<_WordT>(0));
	}
}

template<size_t _Nw>
size_t
_Base_bitset<_Nw>::_M_do_find_first(size_t __not_found) const
{
    for (size_t __i = 0; __i < _Nw; __i++)
	{
            _WordT __thisword = _M_w[__i];
            if (__thisword != static_cast<_WordT>(0))
                return (__i * _GLIBCXX_BITSET_BITS_PER_WORD
                        + __builtin_ctzl(__thisword));
	}
    // not found, so return an indication of failure.
    return __not_found;
}

template<size_t _Nw>
size_t
_Base_bitset<_Nw>::_M_do_find_next(size_t __prev, size_t __not_found) const
{
    // make bound inclusive
    ++__prev;

    // check out of bounds
    if (__prev >= _Nw * _GLIBCXX_BITSET_BITS_PER_WORD)
	return __not_found;

    // search first word
    size_t __i = _S_whichword(__prev);
    _WordT __thisword = _M_w[__i];

    // mask off bits below bound
    __thisword &= (~static_cast<_WordT>(0)) << _S_whichbit(__prev);

    if (__thisword != static_cast<_WordT>(0))
	return (__i * _GLIBCXX_BITSET_BITS_PER_WORD
		+ __builtin_ctzl(__thisword));

    // check subsequent words
    __i++;
    for (; __i < _Nw; __i++)
	{
            __thisword = _M_w[__i];
            if (__thisword != static_cast<_WordT>(0))
                return (__i * _GLIBCXX_BITSET_BITS_PER_WORD
                        + __builtin_ctzl(__thisword));
	}
    // not found, so return an indication of failure.
    return __not_found;
} // end _M_do_find_next

  /**
   *  Base class, specialization for a single word.
   *
   *  See documentation for bitset.
   */
template<>
struct _Base_bitset<1>
{
    typedef unsigned long _WordT;
    _WordT _M_w;

    _Base_bitset(void)
        : _M_w(0)
    { }

    _Base_bitset(unsigned long __val)
        : _M_w(__val)
    { }

    static size_t
    _S_whichword(size_t __pos )
    { return __pos / _GLIBCXX_BITSET_BITS_PER_WORD; }

    static size_t
    _S_whichbyte(size_t __pos )
    { return (__pos % _GLIBCXX_BITSET_BITS_PER_WORD) / __CHAR_BIT__; }

    static size_t
    _S_whichbit(size_t __pos )
    {  return __pos % _GLIBCXX_BITSET_BITS_PER_WORD; }

    static _WordT
    _S_maskbit(size_t __pos )
    { return (static_cast<_WordT>(1)) << _S_whichbit(__pos); }

    _WordT&
    _M_getword(size_t)
    { return _M_w; }

    _WordT
    _M_getword(size_t) const
    { return _M_w; }

    _WordT&
    _M_hiword()
    { return _M_w; }

    _WordT
    _M_hiword() const
    { return _M_w; }

    void
    _M_do_and(const _Base_bitset<1>& __x)
    { _M_w &= __x._M_w; }

    void
    _M_do_or(const _Base_bitset<1>& __x)
    { _M_w |= __x._M_w; }

    void
    _M_do_xor(const _Base_bitset<1>& __x)
    { _M_w ^= __x._M_w; }
    
    void
    _M_do_sub(const _Base_bitset<1>& __x)
    { _M_w &= ~__x._M_w; }
    
    void
    _M_do_left_shift(size_t __shift)
    { _M_w <<= __shift; }

    void
    _M_do_right_shift(size_t __shift)
    { _M_w >>= __shift; }


    //////////////////////////////////////////
    // added by broderic
    bool
    _M_is_subset_of(const _Base_bitset<1>& __x) const
    { return !(_M_w & ~__x._M_w); }

    bool
    _M_is_less_than(const _Base_bitset<1>& __x) const
    { return _M_w < __x._M_w; }

    //////////////////////////////////////////

    void
    _M_do_flip()
    { _M_w = ~_M_w; }

    void
    _M_do_set()
    { _M_w = ~static_cast<_WordT>(0); }

    void
    _M_do_reset()
    { _M_w = 0; }

    bool
    _M_is_equal(const _Base_bitset<1>& __x) const
    { return _M_w == __x._M_w; }

    size_t
    _M_are_all_aux() const
    { return __builtin_popcountl(_M_w); }

    bool
    _M_is_any() const
    { return _M_w != 0; }

    size_t
    _M_do_count() const
    { return __builtin_popcountl(_M_w); }

    size_t
    _M_do_find_first(size_t __not_found) const
    {
        if (_M_w != 0)
            return __builtin_ctzl(_M_w);
        else
            return __not_found;
    }

    // find the next "on" bit that follows "prev"
    size_t
    _M_do_find_next(size_t __prev, size_t __not_found) const
    {
	++__prev;
	if (__prev >= ((size_t) _GLIBCXX_BITSET_BITS_PER_WORD))
            return __not_found;

	_WordT __x = _M_w >> __prev;
	if (__x != 0)
            return __builtin_ctzl(__x) + __prev;
	else
            return __not_found;
    }
};

/**
 *  Base class, specialization for no storage (zero-length %bitset).
 *
 *  See documentation for bitset.
 */
template<>
struct _Base_bitset<0>
{
    typedef unsigned long _WordT;

    _Base_bitset()
    { }

    _Base_bitset(unsigned long)
    { }

    static size_t
    _S_whichword(size_t __pos )
    { return __pos / _GLIBCXX_BITSET_BITS_PER_WORD; }

    static size_t
    _S_whichbyte(size_t __pos )
    { return (__pos % _GLIBCXX_BITSET_BITS_PER_WORD) / __CHAR_BIT__; }

    static size_t
    _S_whichbit(size_t __pos )
    {  return __pos % _GLIBCXX_BITSET_BITS_PER_WORD; }

    static _WordT
    _S_maskbit(size_t __pos )
    { return (static_cast<_WordT>(1)) << _S_whichbit(__pos); }

    // This would normally give access to the data.  The bounds-checking
    // in the bitset class will prevent the user from getting this far,
    // but (1) it must still return an lvalue to compile, and (2) the
    // user might call _Unchecked_set directly, in which case this /needs/
    // to fail.  Let's not penalize zero-length users unless they actually
    // make an unchecked call; all the memory ugliness is therefore
    // localized to this single should-never-get-this-far function.
    _WordT&
    _M_getword(size_t) const
    { 
	throw BenzeneException("_Base_bitset::_M_getword");
	return *new _WordT; 
    }

    _WordT
    _M_hiword() const
    { return 0; }

    void
    _M_do_and(const _Base_bitset<0>&)
    { }

    void
    _M_do_or(const _Base_bitset<0>&)
    { }

    void
    _M_do_xor(const _Base_bitset<0>&)
    { }
    
    void
    _M_do_sub(const _Base_bitset<0>&)
    { }
    
    void
    _M_do_left_shift(size_t)
    { }

    void
    _M_do_right_shift(size_t)
    { }

    void
    _M_do_flip()
    { }

    void
    _M_do_set()
    { }

    void
    _M_do_reset()
    { }

    // Are all empty bitsets equal to each other?  Are they equal to
    // themselves?  How to compare a thing which has no state?  What is
    // the sound of one zero-length bitset clapping?
    bool
    _M_is_equal(const _Base_bitset<0>&) const
    { return true; }

    //////////////////////////////////////////
    // added by broderic
    bool
    _M_is_subset_of(const _Base_bitset<0>&) const
    { return true; }

    bool
    _M_is_less_than(const _Base_bitset<0>&) const
    { return false; }

    //////////////////////////////////////////

    size_t
    _M_are_all_aux() const
    { return 0; }

    bool
    _M_is_any() const
    { return false; }

    size_t
    _M_do_count() const
    { return 0; }

    // Normally "not found" is the size, but that could also be
    // misinterpreted as an index in this corner case.  Oh well.
    size_t
    _M_do_find_first(size_t) const
    { return 0; }

    size_t
    _M_do_find_next(size_t, size_t) const
    { return 0; }
};


// Helper class to zero out the unused high-order bits in the highest word.
template<size_t _Extrabits>
struct _Sanitize
{
    static void _S_do_sanitize(unsigned long& __val)
    { __val &= ~((~static_cast<unsigned long>(0)) << _Extrabits); }
};

template<>
struct _Sanitize<0>
{ static void _S_do_sanitize(unsigned long) {} };

/**
 *  @brief  The %bitset class represents a @e fixed-size sequence of bits.
 *
 *  @ingroup Containers
 *
 *  (Note that %bitset does @e not meet the formal requirements of a
 *  <a href="tables.html#65">container</a>.  Mainly, it lacks iterators.)
 *
 *  The template argument, @a Nb, may be any non-negative number,
 *  specifying the number of bits (e.g., "0", "12", "1024*1024").
 *
 *  In the general unoptimized case, storage is allocated in word-sized
 *  blocks.  Let B be the number of bits in a word, then (Nb+(B-1))/B
 *  words will be used for storage.  B - Nb%B bits are unused.  (They are
 *  the high-order bits in the highest word.)  It is a class invariant
 *  that those unused bits are always zero.
 *
 *  If you think of %bitset as "a simple array of bits," be aware that
 *  your mental picture is reversed:  a %bitset behaves the same way as
 *  bits in integers do, with the bit at index 0 in the "least significant
 *  / right-hand" position, and the bit at index Nb-1 in the "most
 *  significant / left-hand" position.  Thus, unlike other containers, a
 *  %bitset's index "counts from right to left," to put it very loosely.
 *
 *  This behavior is preserved when translating to and from strings.  For
 *  example, the first line of the following program probably prints
 *  "b('a') is 0001100001" on a modern ASCII system.
 *
 *  @code
 *     #include <bitset>
 *     #include <iostream>
 *     #include <sstream>
 *
 *     using namespace std;
 *
 *     int main()
 *     {
 *         long         a = 'a';
 *         bitset<10>   b(a);
 *
 *         cout << "b('a') is " << b << endl;
 *
 *         ostringstream s;
 *         s << b;
 *         string  str = s.str();
 *         cout << "index 3 in the string is " << str[3] << " but\n"
 *              << "index 3 in the bitset is " << b[3] << endl;
 *     }
 *  @endcode
 *
 *  Also see http://gcc.gnu.org/onlinedocs/libstdc++/ext/sgiexts.html#ch23
 *  for a description of extensions.
 *
 *  Most of the actual code isn't contained in %bitset<> itself, but in the
 *  base class _Base_bitset.  The base class works with whole words, not with
 *  individual bits.  This allows us to specialize _Base_bitset for the
 *  important special case where the %bitset is only a single word.
 *
 *  Extra confusion can result due to the fact that the storage for
 *  _Base_bitset @e is a regular array, and is indexed as such.  This is
 *  carefully encapsulated.
 */
template<size_t _Nb>
class benzene_bitset
    : private _Base_bitset<_GLIBCXX_BITSET_WORDS(_Nb)>
{
private:
    typedef _Base_bitset<_GLIBCXX_BITSET_WORDS(_Nb)> _Base;
    typedef unsigned long _WordT;

    void
    _M_do_sanitize()
    {
        _Sanitize<_Nb % _GLIBCXX_BITSET_BITS_PER_WORD>::
	    _S_do_sanitize(this->_M_hiword());
    }

public:
    /**
     *  This encapsulates the concept of a single bit.  An instance of this
     *  class is a proxy for an actual bit; this way the individual bit
     *  operations are done as faster word-size bitwise instructions.
     *
     *  Most users will never need to use this class directly; conversions
     *  to and from bool are automatic and should be transparent.  Overloaded
     *  operators help to preserve the illusion.
     *
     *  (On a typical system, this "bit %reference" is 64 times the size of
     *  an actual bit.  Ha.)
     */
    class reference
    {
	friend class benzene_bitset;

	_WordT *_M_wp;
	size_t _M_bpos;
	
	// left undefined
	reference();
	
    public:
	reference(benzene_bitset& __b, size_t __pos)
	{
            _M_wp = &__b._M_getword(__pos);
            _M_bpos = _Base::_S_whichbit(__pos);
	}

	~reference()
	{ }

	// For b[i] = __x;
	reference&
	operator=(bool __x)
	{
            if (__x)
                *_M_wp |= _Base::_S_maskbit(_M_bpos);
            else
                *_M_wp &= ~_Base::_S_maskbit(_M_bpos);
            return *this;
	}

	// For b[i] = b[__j];
	reference&
	operator=(const reference& __j)
	{
            if ((*(__j._M_wp) & _Base::_S_maskbit(__j._M_bpos)))
                *_M_wp |= _Base::_S_maskbit(_M_bpos);
            else
                *_M_wp &= ~_Base::_S_maskbit(_M_bpos);
            return *this;
	}

	// Flips the bit
	bool
	operator~() const
	{ return (*(_M_wp) & _Base::_S_maskbit(_M_bpos)) == 0; }

	// For __x = b[i];
	operator bool() const
	{ return (*(_M_wp) & _Base::_S_maskbit(_M_bpos)) != 0; }

	// For b[i].flip();
	reference&
	flip()
	{
            *_M_wp ^= _Base::_S_maskbit(_M_bpos);
            return *this;
	}
    };
    friend class reference;

    // 23.3.5.1 constructors:
    /// All bits set to zero.
    benzene_bitset()
    { }

    /// Initial bits bitwise-copied from a single word (others set to zero).
    benzene_bitset(unsigned long __val)
        : _Base(__val)
    { _M_do_sanitize(); }

    /**
     *  @brief  Use a subset of a string.
     *  @param  __s  A string of '0' and '1' characters.
     *  @param  __position  Index of the first character in @a s to use;
     *                    defaults to zero.
     *  @throw  std::out_of_range  If @a pos is bigger the size of @a s.
     *  @throw  std::invalid_argument  If a character appears in the string
     *                                 which is neither '0' nor '1'.
     */
    template<class _CharT, class _Traits, class _Alloc>
    explicit
    benzene_bitset(const std::basic_string<_CharT, _Traits, _Alloc>& __s,
           size_t __position = 0)
	: _Base()
    {
        if (__position > __s.size())
	    throw BenzeneException("benzene_bitset::bitset initial position "
				     "not valid");
        _M_copy_from_string(__s, __position,
                            std::basic_string<_CharT, _Traits, _Alloc>::npos);
    }

    /**
     *  @brief  Use a subset of a string.
     *  @param  __s  A string of '0' and '1' characters.
     *  @param  __position  Index of the first character in @a s to use.
     *  @param  __n    The number of characters to copy.
     *  @throw  std::out_of_range  If @a pos is bigger the size of @a s.
     *  @throw  std::invalid_argument  If a character appears in the string
     *                                 which is neither '0' nor '1'.
     */
    template<class _CharT, class _Traits, class _Alloc>
    benzene_bitset(const std::basic_string<_CharT, _Traits, _Alloc>& __s,
           size_t __position, size_t __n)
	: _Base()
    {
        if (__position > __s.size())
	    throw BenzeneException("benzene_bitset::bitset initial position "
				     "not valid");
        _M_copy_from_string(__s, __position, __n);
    }
      
    // 23.3.5.2 bitset operations:
    //@{
    /**
     *  @brief  Operations on bitsets.
     *  @param  __rhs  A same-sized bitset.
     *
     *  These should be self-explanatory.
     */
    benzene_bitset<_Nb>&
    operator&=(const benzene_bitset<_Nb>& __rhs)
    {
	this->_M_do_and(__rhs);
	return *this;
    }

    benzene_bitset<_Nb>&
    operator|=(const benzene_bitset<_Nb>& __rhs)
    {
	this->_M_do_or(__rhs);
	return *this;
    }

    benzene_bitset<_Nb>&
    operator^=(const benzene_bitset<_Nb>& __rhs)
    {
	this->_M_do_xor(__rhs);
	return *this;
    }
    
    benzene_bitset<_Nb>&
    operator-=(const benzene_bitset<_Nb>& __rhs)
    {
        this->_M_do_sub(__rhs);
        return *this;
    }
    
    /////////////////////////////////////////////////
    // added by broderic
    bool is_subset_of(const benzene_bitset<_Nb>& __rhs) const
    {
        return this->_M_is_subset_of(__rhs);
    }

    /** More of a tiebreaker than a true less than comparison. */
    bool is_less_than(const benzene_bitset<_Nb>& __rhs) const
    {
        return this->_M_is_less_than(__rhs);
    }
    /////////////////////////////////////////////////

    //@}
      
    //@{
    /**
     *  @brief  Operations on bitsets.
     *  @param  __position  The number of places to shift.
     *
     *  These should be self-explanatory.
     */
    benzene_bitset<_Nb>&
    operator<<=(size_t __position)
    {
	if (__builtin_expect(__position < _Nb, 1))
            {
                this->_M_do_left_shift(__position);
                this->_M_do_sanitize();
            }
	else
            this->_M_do_reset();
	return *this;
    }

    benzene_bitset<_Nb>&
    operator>>=(size_t __position)
    {
	if (__builtin_expect(__position < _Nb, 1))
            {
                this->_M_do_right_shift(__position);
                this->_M_do_sanitize();
            }
	else
            this->_M_do_reset();
	return *this;
    }
    //@}
      
    //@{
    /**
     *  These versions of single-bit set, reset, flip, and test are
     *  extensions from the SGI version.  They do no range checking.
     *  @ingroup SGIextensions
     */
    benzene_bitset<_Nb>&
    _Unchecked_set(size_t __pos)
    {
	this->_M_getword(__pos) |= _Base::_S_maskbit(__pos);
	return *this;
    }

    benzene_bitset<_Nb>&
    _Unchecked_set(size_t __pos, int __val)
    {
	if (__val)
            this->_M_getword(__pos) |= _Base::_S_maskbit(__pos);
	else
            this->_M_getword(__pos) &= ~_Base::_S_maskbit(__pos);
	return *this;
    }

    benzene_bitset<_Nb>&
    _Unchecked_reset(size_t __pos)
    {
	this->_M_getword(__pos) &= ~_Base::_S_maskbit(__pos);
	return *this;
    }

    benzene_bitset<_Nb>&
    _Unchecked_flip(size_t __pos)
    {
	this->_M_getword(__pos) ^= _Base::_S_maskbit(__pos);
	return *this;
    }

    bool
    _Unchecked_test(size_t __pos) const
    { return ((this->_M_getword(__pos) & _Base::_S_maskbit(__pos))
              != static_cast<_WordT>(0)); }
    //@}
      
    // Set, reset, and flip.
    /**
     *  @brief Sets every bit to true.
     */
    benzene_bitset<_Nb>&
    set()
    {
	this->_M_do_set();
	this->_M_do_sanitize();
	return *this;
    }

    /**
     *  @brief Sets a given bit to a particular value.
     *  @param  __position  The index of the bit.
     *  @param  __val  Either true or false, defaults to true.
     *  @throw  std::out_of_range  If @a pos is bigger the size of the %set.
     */
    benzene_bitset<_Nb>&
    set(size_t __position, bool __val = true)
    {
	if (__position >= _Nb)
            throw BenzeneException("benzene_bitset::set");
	return _Unchecked_set(__position, __val);
    }

    /**
     *  @brief Sets every bit to false.
     */
    benzene_bitset<_Nb>&
    reset()
    {
	this->_M_do_reset();
	return *this;
    }

    /**
     *  @brief Sets a given bit to false.
     *  @param  __position  The index of the bit.
     *  @throw  std::out_of_range  If @a pos is bigger the size of the %set.
     *
     *  Same as writing @c set(pos,false).
     */
    benzene_bitset<_Nb>&
    reset(size_t __position)
    {
	if (__position >= _Nb)
            throw BenzeneException("benzene_bitset::reset");
	return _Unchecked_reset(__position);
    }
      
    /**
     *  @brief Toggles every bit to its opposite value.
     */
    benzene_bitset<_Nb>&
    flip()
    {
	this->_M_do_flip();
	this->_M_do_sanitize();
	return *this;
    }

    /**
     *  @brief Toggles a given bit to its opposite value.
     *  @param  __position  The index of the bit.
     *  @throw  std::out_of_range  If @a pos is bigger the size of the %set.
     */
    benzene_bitset<_Nb>&
    flip(size_t __position)
    {
	if (__position >= _Nb)
            throw BenzeneException("benzene_bitset::flip");
	return _Unchecked_flip(__position);
    }
      
    /// See the no-argument flip().
    benzene_bitset<_Nb>
    operator~() const
    { return benzene_bitset<_Nb>(*this).flip(); }

    //@{
    /**
     *  @brief  Array-indexing support.
     *  @param  __position  Index into the %bitset.
     *  @return  A bool for a 'const %bitset'.  For non-const bitsets, an
     *           instance of the reference proxy class.
     *  @note  These operators do no range checking and throw no exceptions,
     *         as required by DR 11 to the standard.
     *
     *  _GLIBCXX_RESOLVE_LIB_DEFECTS Note that this implementation already
     *  resolves DR 11 (items 1 and 2), but does not do the range-checking
     *  required by that DR's resolution.  -pme
     *  The DR has since been changed:  range-checking is a precondition
     *  (users' responsibility), and these functions must not throw.  -pme
     */
    reference
    operator[](size_t __position)
    { return reference(*this,__position); }

    bool
    operator[](size_t __position) const
    { return _Unchecked_test(__position); }
    //@}

    /**
     *  @brief Returns a character interpretation of the %bitset.
     *  @return  The string equivalent of the bits.
     *
     *  Note the ordering of the bits:  decreasing character positions
     *  correspond to increasing bit positions (see the main class notes for
     *  an example).
     */
    template<class _CharT, class _Traits, class _Alloc>
    std::basic_string<_CharT, _Traits, _Alloc>
    to_string() const
    {
        std::basic_string<_CharT, _Traits, _Alloc> __result;
        _M_copy_to_string(__result);
        return __result;
    }

    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    // 434. bitset::to_string() hard to use.
    template<class _CharT, class _Traits>
    std::basic_string<_CharT, _Traits, std::allocator<_CharT> >
    to_string() const
    { return to_string<_CharT, _Traits, std::allocator<_CharT> >(); }

    template<class _CharT>
    std::basic_string<_CharT, std::char_traits<_CharT>,
                      std::allocator<_CharT> >
    to_string() const
                      {
                          return to_string<_CharT, std::char_traits<_CharT>,
                              std::allocator<_CharT> >();
}

    std::basic_string<char, std::char_traits<char>, std::allocator<char> >
    to_string() const
    {
	return to_string<char, std::char_traits<char>,
            std::allocator<char> >();
}

    // Helper functions for string operations.
    template<class _CharT, class _Traits, class _Alloc>
    void
    _M_copy_from_string(const std::basic_string<_CharT,
                        _Traits, _Alloc>& __s,
                        size_t, size_t);

template<class _CharT, class _Traits, class _Alloc>
void
_M_copy_to_string(std::basic_string<_CharT, _Traits, _Alloc>&) const;

/// Returns the number of bits which are set.
size_t
count() const
{ return this->_M_do_count(); }

/// Returns the total number of bits.
size_t
size() const
{ return _Nb; }

//@{
/// These comparisons for equality/inequality are, well, @e bitwise.
bool
operator==(const benzene_bitset<_Nb>& __rhs) const
{ return this->_M_is_equal(__rhs); }

bool
operator!=(const benzene_bitset<_Nb>& __rhs) const
{ return !this->_M_is_equal(__rhs); }
//@}
      
/**
 *  @brief Tests the value of a bit.
 *  @param  __position  The index of a bit.
 *  @return  The value at @a pos.
 *  @throw  std::out_of_range  If @a pos is bigger the size of the %set.
 */
bool
test(size_t __position) const
{
    BenzeneAssert(__position < _Nb);
    //if (__position >= _Nb)
    //    throw BenzeneException("benzene_bitset::test");
    return _Unchecked_test(__position);
}

// _GLIBCXX_RESOLVE_LIB_DEFECTS
// DR 693. std::bitset::all() missing.
/**
 *  @brief Tests whether all the bits are on.
 *  @return  True if all the bits are set.
 */
bool
all() const
{ return this->_M_are_all_aux() == _Nb; }

/**
 *  @brief Tests whether any of the bits are on.
 *  @return  True if at least one bit is set.
 */
bool
any() const
{ return this->_M_is_any(); }

/**
 *  @brief Tests whether any of the bits are on.
 *  @return  True if none of the bits are set.
 */
bool
none() const
{ return !this->_M_is_any(); }

//@{
/// Self-explanatory.
benzene_bitset<_Nb>
operator<<(size_t __position) const
{ return benzene_bitset<_Nb>(*this) <<= __position; }

benzene_bitset<_Nb>
operator>>(size_t __position) const
{ return benzene_bitset<_Nb>(*this) >>= __position; }
//@}
      
/**
 *  @brief  Finds the index of the first "on" bit.
 *  @return  The index of the first bit set, or size() if not found.
 *  @ingroup SGIextensions
 *  @sa  _Find_next
 */
size_t
_Find_first() const
{ return this->_M_do_find_first(_Nb); }

/**
 *  @brief  Finds the index of the next "on" bit after prev.
 *  @return  The index of the next bit set, or size() if not found.
 *  @param  __prev  Where to start searching.
 *  @ingroup SGIextensions
 *  @sa  _Find_first
 */
size_t
_Find_next(size_t __prev ) const
{ return this->_M_do_find_next(__prev, _Nb); }
};

// Definitions of non-inline member functions.
template<size_t _Nb>
template<class _CharT, class _Traits, class _Alloc>
void
benzene_bitset<_Nb>::
_M_copy_from_string(const std::basic_string<_CharT, _Traits,
                    _Alloc>& __s, size_t __pos, size_t __n)
{
    reset();
    const size_t __nbits = std::min(_Nb, std::min(__n, __s.size() - __pos));
    for (size_t __i = __nbits; __i > 0; --__i)
        {
	    switch(__s[__pos + __nbits - __i])
                {
                case '0':
                    break;
                case '1':
                    _Unchecked_set(__i - 1);
                    break;
                default:
                    throw BenzeneException("benzene_bitset::_M_copy_from_string");
                }
        }
}

template<size_t _Nb>
template<class _CharT, class _Traits, class _Alloc>
void
benzene_bitset<_Nb>::
_M_copy_to_string(std::basic_string<_CharT, _Traits, _Alloc>& __s) const
{
    __s.assign(_Nb, '0');
    for (size_t __i = _Nb; __i > 0; --__i)
        if (_Unchecked_test(__i - 1))
	    __s[_Nb - __i] = '1';
}

// 23.3.5.3 bitset operations:
//@{
/**
 *  @brief  Global bitwise operations on bitsets.
 *  @param  __x  A bitset.
 *  @param  __y  A bitset of the same size as @a x.
 *  @return  A new bitset.
 *
 *  These should be self-explanatory.
 */
template<size_t _Nb>
inline benzene_bitset<_Nb>
operator&(const benzene_bitset<_Nb>& __x, const benzene_bitset<_Nb>& __y)
{
    benzene_bitset<_Nb> __result(__x);
    __result &= __y;
    return __result;
}

template<size_t _Nb>
inline benzene_bitset<_Nb>
operator|(const benzene_bitset<_Nb>& __x, const benzene_bitset<_Nb>& __y)
{
    benzene_bitset<_Nb> __result(__x);
    __result |= __y;
    return __result;
}

template <size_t _Nb>
inline benzene_bitset<_Nb>
operator^(const benzene_bitset<_Nb>& __x, const benzene_bitset<_Nb>& __y)
{
    benzene_bitset<_Nb> __result(__x);
    __result ^= __y;
    return __result;
}

template <size_t _Nb>
inline benzene_bitset<_Nb>
operator-(const benzene_bitset<_Nb>& __x, const benzene_bitset<_Nb>& __y)
{
    benzene_bitset<_Nb> __result(__x);
    __result -= __y;
    return __result;
}
//@}

template <class _CharT, class _Traits, size_t _Nb>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& __os,
           const benzene_bitset<_Nb>& __x)
{
    std::basic_string<_CharT, _Traits> __tmp;
    __x._M_copy_to_string(__tmp);
    return __os << __tmp;
}
//@}

#undef _GLIBCXX_BITSET_WORDS
#undef _GLIBCXX_BITSET_BITS_PER_WORD

//----------------------------------------------------------------------------

_END_BENZENE_NAMESPACE_

#endif /* BENZENE_BITSET */
