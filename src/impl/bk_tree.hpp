#ifndef BK_TREE_HPP
#define BK_TREE_HPP
 
#include <map>
#include <memory>
#include <type_traits>
 
namespace MB
{
 
template <typename T, typename Unit, typename Metric> class bktree;
 
namespace detail
{
 
template <typename T, typename Unit, typename Metric>
class bktree_node
{
  friend class bktree<T, Unit, Metric>;
  typedef bktree_node<T, Unit, Metric> node_type;
  typedef std::unique_ptr<node_type> node_ptr_type;
     
  T value_;
  std::map<Unit, node_ptr_type> children_;
 
  bktree_node(const T &value)
      : value_(value)
  {
  }
 
  bool insert(const T& value, const Metric& distance)
  {
    bool inserted = false;
    Unit dist = distance(value, this->value_);
    if (dist > 0) {
      // Not already here 
      auto it = children_.find(dist);
      if (it == children_.end()) {
        // Attach a new edge here
        children_.insert(std::make_pair(dist, node_ptr_type(new node_type(value))));
        inserted = true;
      }
      else {
        // Follow existing edge
        inserted = it->second->insert(value, distance);
      }
      return inserted;
    }
    return inserted; // Always false
  }

  template <class OutputIt>
  OutputIt find(const T &value, const Unit& limit, const Metric& distance, OutputIt it) const
  {
    Unit dist = distance(value, this->value_);
    if (dist <= limit) {
      // Found one
      *it++ = std::make_pair(this->value_, dist);
    }
    for (auto iter = children_.begin(); iter != children_.end(); ++iter) {
      // Follow edges between dist + limit and dist - limit
      if (dist - limit <= iter->first && dist + limit >= iter->first) {
        iter->second->find(value, limit, distance, it);
      }
    }
    return it;
  }
};
 
}; // namespace detail
 
template <typename T, typename Unit, typename Metric>
class bktree
{
 private:
  typedef typename detail::bktree_node<T, Unit, Metric>::node_type node_type;
  typedef typename detail::bktree_node<T, Unit, Metric>::node_ptr_type node_ptr_type;
 
  node_ptr_type head_;
  const Metric distance_;
  size_t size_;
 
 public:
  bktree(const Metric& distance = Metric())
      : head_(nullptr), distance_(distance), size_(0L)
  {
    static_assert(std::is_integral<Unit>::value, "Integral unit type required.");
  }
 
  bool insert(const T &value)
  {
    bool inserted = false;
    if (head_ == nullptr) {
      // Inserting the first value
      head_ = node_ptr_type(new node_type(value));
      size_ = 1;
      inserted = true;
    }
    else if (head_->insert(value, distance_)) {
      ++size_;
      inserted = true;
    }
    return inserted;
  }
 
  template <class OutputIt>
  OutputIt find(const T& value, const Unit& limit, OutputIt it) const
  {
    return head_->find(value, limit, distance_, it);
  }
 
  size_t size() const
  {
    return size_;
  }
};
 
}; // namespace MB
 
#endif // BK_TREE_HPP
