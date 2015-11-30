#include <vector>

template <typename T>
class circular_vector {
  public:
    using size_type = typename std::vector<T>::size_type;

    /**
     * Create a circular vector of size @a size,
     * with each item having value @a value.
     */
    explicit circular_vector(size_type size, const T& value = T())
    : pos_zero_(0),
      steps_count_(0),
      size_(size),
      vec_(size, value)
    {}
    
    T& get(int offset) {
      const auto pos = pos_for_offset(offset);
      return vec_[pos];
    }
    
    const T& get(int offset) const {
      const auto pos = pos_for_offset(offset);
      return vec_[pos];
    }

    T& get_at_offset_from_start(int offset) {
      return get(get_offset_from_start(offset));
    }

    const T& get_at_offset_from_start(int offset) const {
      return get(get_offset_from_start(offset));
    }

    /** Cause get(-1) to return whatever get(0) currently returns.
     */
    void step() {
      ++steps_count_;

      ++pos_zero_;
      if(pos_zero_ >= (int)size_)
        pos_zero_ = 0; 
    }

    size_type size() const {
      return size_;
    }
    
  private:
    /** Get a position relative to pos_zero_,
     * wrapping around if necessary.
     */    
    size_type pos_for_offset(int offset) const {
      if (offset >= (int)size_) {
        std::cerr << "offset too large: " << offset << std::endl;
      }

      int pos = pos_zero_ + offset;
      if(pos >= (int)size_)
        return 0 + (size_ - pos);
      else if (pos < 0)
        return size_ + pos;
      else
        return pos;
    }

    /** For instance, get the 5th item,
     * regardless of how many times we have called step().
     * This can fail if the @a offset is greater than size().
     */
    int get_offset_from_start(int offset) const {
     const int current_offset = offset - steps_count();
      if (std::abs(current_offset) > (int)size()) {
        std::cerr << "Item has already been discarded. size=" << size() << ", current_offset=" << current_offset << std::endl;
        return 0;
      }

      return current_offset;
    }

    /** Returns which index get(0) now represents.
     */
    int steps_count() const {
      return steps_count_;
    }

    int pos_zero_;
    int steps_count_;
    unsigned int size_;
    std::vector<T> vec_;
};
