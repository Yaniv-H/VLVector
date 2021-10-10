#ifndef _VL_VECTOR_H_
#define _VL_VECTOR_H_

#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cmath>

#define DEFAULT_STATIC_CAPACITY 16UL
#define ERR_OUT_OF_RANGE "Trying to reach index bigger then the vector's size"
#define UPSIZE_CAP(size, k) (3 * (size + k)) / 2;

template<typename T, size_t StaticCapacity = DEFAULT_STATIC_CAPACITY>
class VLVector {

  size_t _size;
  size_t _capacity;
  T _static_storage[StaticCapacity];
  T *_heap_storage = nullptr;
  /**
   * Allocate new storage and update capacity. This function does not free
   * the allocated memory, freeing the memory will be done by other
   * functions according to the situation
   * @param num_of_adding_elem The number of elements to add
   * @return Pointer to the allocated memory
   */
  T *alloc_new_heap_storage (size_t num_of_adding_elem)
  {
    _capacity = UPSIZE_CAP(_size, num_of_adding_elem);
    T *new_heap_storage;
    try
    { new_heap_storage = new T[_capacity]; }
    catch (const std::bad_alloc &ba)
    {
      if (_size > StaticCapacity)
      { delete[] _heap_storage; }
      throw;
    }
    return new_heap_storage;
  }

 public:
  typedef T value_type;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef value_type *iterator;
  typedef const value_type *const_iterator;
  typedef std::reverse_iterator<iterator> reverse_iterator;
  typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

  /**
   * Default Constructor, initialize new empty VLVector
   */
  VLVector () : _size (0), _capacity (StaticCapacity)
  {}

  /**
   * Copy Constructor: constructs a VLVector with a copy of each of the
   * elements in rhs, in the same order.
   * @param rhs The VLVector to copy from
   */
  VLVector (const VLVector<T, StaticCapacity> &rhs)
      : _size (rhs._size), _capacity (rhs._capacity)
  {
    //ניתן להניח כי בעת בניית וקטור חדש דרך בנאי ההעתקה/דרך השמה, ניתן
    // להניח שהטיפוסים של הוקטורים זהים וגודל הזיכרון הסטטי שלהם זהה
    if (_size > StaticCapacity)
    { _heap_storage = new T[_capacity]; }
    std::copy (rhs.begin (), rhs.end (), this->begin ());
  }

  /**
   * Sequence based Constructor, construct VLVector by copy the elements in
   * the range [first,last), in the same order
   * @tparam InputIterator The given iterator type of the given sequence
   * @param first The first position of the sequence
   * @param last The last position of the sequence
   */
  template<typename InputIterator>
  VLVector (const InputIterator first, const InputIterator last)
      : _size (std::distance (first, last)), _capacity (StaticCapacity)
  {
    if (_size > StaticCapacity)
    {
      _capacity = UPSIZE_CAP(_size, 0);
      _heap_storage = new T[_capacity];
    }
    std::copy (first, last, begin ());
  }

  /**
   * Single-value initialized constructor, construct VLVector with count
   * elements of the given init_val
   * @param count The size of the constructed VLVector
   * @param init_val The initialization value of the elements
   */
  VLVector (size_t count, const_reference init_val)
      : _size (count), _capacity (StaticCapacity)
  {
    if (_size > StaticCapacity)
    {
      _capacity = UPSIZE_CAP(_size, 0);
      _heap_storage = new T[_capacity];
    }
    std::fill_n (begin (), count, init_val);
  }

  /**
   * Destructor of VLVector
   */
  ~VLVector ()
  {
    if (_size > StaticCapacity)
    { delete[] _heap_storage; }
  }

  /**
   * @return The size of the VLVector
   */
  size_t size () const noexcept
  { return _size; }

  /**
 * @return The capacity of the VLVector
 */
  size_t capacity () const noexcept
  { return _capacity; }

  /**
   * Check if the VLVector is empty (it's size equal to zero)
   * @return True if it is, otherwise false
   */
  bool empty () const noexcept
  { return (_size == 0); }

  /**
   * Check if the given index is within the vector's range, if it is return
   * the element at the given index, otherwise throw an exception
   * @param ind The given index
   * @return if the given index is within the vector's range return the
   * element at the given index
   */
  reference at (size_t ind)
  {
    if (_size <= ind)
    {
      if (_size > StaticCapacity)
      { delete[] _heap_storage; }
      throw std::out_of_range (ERR_OUT_OF_RANGE);
    }
    return (begin ())[ind];
  }

  /**
   * Const version of the at function, operates the same
   * @param ind The given index
   * @return if the given index is within the vector's range return the const
   * element at the given index
   */
  const_reference at (size_t ind) const
  {
    if (_size <= ind)
    {
      throw std::out_of_range (ERR_OUT_OF_RANGE);
    }
    return (begin ())[ind];
  }

  /**
   * Appends the given element value to the end of the container.
   * @param element The given element
   */
  void push_back (const_reference element)
  {
    if (_size + 1 > _capacity)
    { // need to allocate new heap storage and copy (from Stack or from Heap)
      T *new_heap_storage = alloc_new_heap_storage (1);
      std::copy (begin (), end (), new_heap_storage);
      if (_size > StaticCapacity)
      { delete[] _heap_storage; }
      _heap_storage = new_heap_storage;
    }
    ++_size;
    begin ()[_size - 1] = element;
  }

  /**
   *  Inserts the given element before pos
   * @param pos iterator that points at some element of the VLVector
   * @param element The given elements to insert
   * @return iterator to the inserted element
   */
  iterator insert (const_iterator pos, const_reference element)
  {
    iterator insert_pos;
    if (_size + 1 > _capacity)
    { // need to allocate new heap storage and copy (from Stack or from Heap)
      T *new_heap_storage = alloc_new_heap_storage (1);
      insert_pos = std::copy (cbegin (), pos, new_heap_storage);
      *insert_pos = element;
      std::copy (pos, cend (), insert_pos + 1);
      if (_size > StaticCapacity)
      { delete[] _heap_storage; }
      _heap_storage = new_heap_storage;
    }
    else
    { // need to insert at the current storage (Stack or Heap)
      insert_pos = std::move_backward (pos, cend (), end () + 1) - 1;
      *insert_pos = element;
    }
    ++_size;
    return insert_pos;
  }

  /**
   * Inserts elements from range [first, last) before pos.
   * @tparam InputIterator The given iterator type of the given sequence
   * @param pos iterator that points at some element of the VLVector
   * @param first The first position of the sequence
   * @param last The last position of the sequence
   * @return iterator that points to the first element that inserted
   */
  template<typename InputIterator>
  iterator insert (const_iterator pos, const InputIterator first,
                   const InputIterator last)
  {
    size_t sequence_length = std::distance (first, last);
    if (sequence_length == 0)
    { return begin () + std::distance (cbegin (), pos); }

    iterator insert_pos;
    iterator end_of_insert_pos;
    if (_size + sequence_length > _capacity)
    { // need to allocate new heap storage and copy (from Stack or from Heap)
      T *new_heap_storage = alloc_new_heap_storage (sequence_length);
      insert_pos = std::copy (cbegin (), pos, new_heap_storage);
      end_of_insert_pos = std::copy (first, last, insert_pos);
      std::copy (pos, cend (), end_of_insert_pos);
      if (_size > StaticCapacity)
      { delete[] _heap_storage; }
      _heap_storage = new_heap_storage;
    }
    else
    { // need to insert at the current storage (Stack or Heap)
      T *copied_sequence = new T[sequence_length];
      std::copy (first, last, copied_sequence);
      insert_pos = std::move_backward (pos, cend (), end () + sequence_length)
                   - sequence_length;
      std::copy (copied_sequence, copied_sequence + sequence_length,
                 insert_pos);
      delete[] copied_sequence;
    }
    _size += sequence_length;
    return insert_pos;
  }

  /**
   * Removes the last element of the VLVector
   */
  void pop_back ()
  {
    if (_size == 0)
    { return; }
    if (_size == StaticCapacity + 1)
    {
      std::copy (begin (), end () - 1, _static_storage);
      delete[] _heap_storage;
      _heap_storage = nullptr;
      _capacity = StaticCapacity;
    }
    --_size;
  }

  /**
   * Removes the element at pos
   * @param pos iterator within the range of the VLVector
   * @return iterator to the right of the removed element
   */
  iterator erase (const_iterator pos)
  {
    if (pos == cend ())
    { return end (); }

    iterator right_to_removed_pos;
    if (_size - 1 == StaticCapacity)
    { // from heap to stack
      right_to_removed_pos = std::copy (cbegin (), pos, _static_storage);
      std::copy (pos + 1, cend (), right_to_removed_pos);
      delete[] _heap_storage;
      _heap_storage = nullptr;
      _capacity = StaticCapacity;
    }

    else
    { // stack to stack or heap to heap
      right_to_removed_pos = begin () + std::distance (cbegin (), pos);
      std::move (pos + 1, cend (), right_to_removed_pos);
    }

    --_size;
    return right_to_removed_pos;
  }

  /**
   * Removes the elements in the range [first, last)
   * @tparam InputIterator The given iterator type of the given sequence
   * @param first The first position of the sequence
   * @param last The last position of the sequence
   * @return iterator following the last removed element
   */
  template<typename InputIterator>
  iterator erase (const InputIterator first, const InputIterator last)
  {
    const_iterator first_pos = &(*first);
    const_iterator last_pos = &(*last);
    size_t num_of_elem = std::distance (first, last);
    iterator right_to_remove_pos;

    if (StaticCapacity < _size && _size - num_of_elem <= StaticCapacity)
    { // from heap to stack
      right_to_remove_pos = std::copy (cbegin (), first_pos, _static_storage);
      std::copy (last_pos, cend (), right_to_remove_pos);
      delete[] _heap_storage;
      _heap_storage = nullptr;
      _capacity = StaticCapacity;
    }

    else
    { // stack to stack or heap to heap
      right_to_remove_pos = begin () + std::distance (cbegin (), first_pos);
      std::move (last_pos, cend (), right_to_remove_pos);
    }
    _size -= num_of_elem;
    return right_to_remove_pos;
  }

  /**
  * Erases all elements from the VLVector
  */
  void clear () noexcept
  {
    if (_size > StaticCapacity)
    {
      delete[] _heap_storage;
      _heap_storage = nullptr;
      _capacity = StaticCapacity;
    }
    _size = 0;
  }

  /**
   * @return pointer to the underlying array serving as element storage
   */
  T *data () noexcept
  {
    if (_size > StaticCapacity)
    { return _heap_storage; }
    else
    { return (T *) _static_storage; }
  }

  /**
   * Const version of the data function - modifying the element is not allowed
   * @return pointer to the underlying array serving as element storage
   */
  const T *data () const noexcept
  {
    if (_size > StaticCapacity)
    { return _heap_storage; }
    else
    { return (T *) _static_storage; }
  }

  /**
   * Check if a given element exist in the VLVector
   * @param element The given element
   * @return True if exist, otherwise false
   */
  bool contains (const_reference element) const noexcept
  {
    for (const_reference cur_elem: *this)
    {
      if (cur_elem == element)
      { return true; }
    }
    return false;
  }

  /**
   * Copy assignment operator. Replaces the contents with a copy of the
   * contents of a given VLVector
   * @param rhs The given VLVector
   * @return Reference to *this
   */
  VLVector<T, StaticCapacity> &
  operator= (const VLVector<T, StaticCapacity> &rhs)
  {
    if (this == &rhs)
    { return *this; }

    if (_size > StaticCapacity)
    {
      delete[] _heap_storage;
      _heap_storage = nullptr;
    }

    _size = rhs._size;
    _capacity = rhs._capacity;
    if (_size > StaticCapacity)
    { _heap_storage = new T[_capacity]; }

    std::copy (rhs.begin (), rhs.end (), begin ());
    return *this;
  }

  /**
   * Returns a reference to the element at specified index. No bounds
   * checking is performed.
   * @param ind The given index
   * @return Reference to the requested element.
   */
  reference operator[] (size_t ind)
  { return (begin ())[ind]; }

  /**
   * Const version of operator[]. No modification of the return element is
   * allowed
   * @param ind The given index
   * @return Reference to the requested element.
   */
  const_reference operator[] (size_t ind) const
  { return (begin ())[ind]; }

  /**
   *  Checks if the contents of this and rhs are equal, that is, they have the
   *  same number of elements and each element in this compares equal with the
   *  element in rhs at the same position.
   * @param rhs The given VLVector to compare with
   * @return True if the condition describe above are true, otherwise false
   */
  bool operator== (const VLVector<T, StaticCapacity> &rhs) const noexcept
  {
    if (_size != rhs._size)
    { return false; }
    return std::equal (begin (), end (), rhs.begin ());
  }

  /**
   * Checks if the contents of this and rhs are not equal. equal means that
   * they have the same number of elements and each element in this compares
   * equal with the element in rhs at the same position.
   * @param rhs The given VLVector to compare with
   * @return The opposite boolean value that operator== returns
   */
  bool operator!= (const VLVector &rhs) const noexcept
  {
    return !(operator== (rhs));
  }

  /**
   * @return iterator that points to the first element of the VLVector
   */
  iterator begin () noexcept
  { return data (); }

  /**
   * Const version of begin function. No modification of the pointed element
   * is allowed
   * @return const iterator that points to the first element of the VLVector
   */
  const_iterator begin () const noexcept
  { return data (); }

  /**
 * @return const iterator that points to the first element of the VLVector
 */
  const_iterator cbegin () const noexcept
  { return data (); }

  /**
   * @return iterator that points to the end of elements in the VLVector
   */
  iterator end () noexcept
  { return data () + _size; }

  /**
   * Const version of end function. No modification of the value pointed by
   * the iterator is allowed
   * @return const iterator that points to the end of elements in the VLVector
   */
  const_iterator end () const
  { return data () + _size; }

  /**
  * @return const iterator that points to the end of elements in the VLVector
  */
  const_iterator cend () const
  { return data () + _size; }

  /**
   * @return A reverse iterator pointing to the last element in the vector
   */
  reverse_iterator rbegin () noexcept
  { return reverse_iterator (end ()); }

  /**
   * Const version of rbegin. No modification of the value pointed by
   * the iterator is allowed
   * @return const reverse iterator pointing to the last element in the vector
   */
  const_reverse_iterator rbegin () const noexcept
  { return const_reverse_iterator (end ()); }

  /**
   * @return const reverse iterator pointing to the last element in the vector
   */
  const_reverse_iterator crbegin () const noexcept
  { return const_reverse_iterator (cend ()); }

  /**
   * @return A reverse iterator pointing to the theoretical element preceding
   * the first element in the vector
   */
  reverse_iterator rend () noexcept
  { return reverse_iterator (begin ()); }

  /**
  * Const version of rend function. No modification of the value pointed by
  * the iterator is allowed
  * @return const reverse iterator pointing to the theoretical element
  * preceding the first element in the vector
  */
  const_reverse_iterator rend () const noexcept
  { return const_reverse_iterator (begin ()); }

  /**
  * @return const reverse iterator pointing to the theoretical element
  * preceding the first element in the vector
  */
  const_reverse_iterator crend () const noexcept
  { return const_reverse_iterator (cbegin ()); }

};

#endif //_VL_VECTOR_H_
