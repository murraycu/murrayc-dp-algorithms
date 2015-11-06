// Build with:
// g++ -g -Wall -Werror -Wextra -Wpedantic -Wshadow --std=c++11 test.cc -o prog

#include <cstdlib>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>


/*
static
void indent(int level)
{
  std::cout << "level: " << level;
  for(int l = 0; l < level; ++l) {
    std::cout << "  ";
  }
}
*/

template<typename T>
void print_vec(const std::vector<T>& vec)
{
  for(auto num : vec)
  {
    std::cout << num << ", ";
  }
}

class DependencyResolution
{
public:

  typedef std::unordered_set<std::string> type_package_names; //package names

  // Map of package names to their dependencies:
  typedef std::unordered_map<std::string, type_package_names> type_packages;

  explicit DependencyResolution(const type_packages& packages)
  : packages_(packages),
    sequence_(0),
    finish_(false)
  {}

  std::vector<std::string> get_build_sequence(const std::string& package_to_build);

private:
  void dfs(const std::string& package_name);

  void clear();

  //These could be pure virtual methods in a generic DFS base class:
  void process_node_pre(const std::string& package_name);
  void process_edge(const std::string& parent_package_name, const std::string& package_name);
  void process_node_post(const std::string& package_name);

  bool is_discovered(const std::string& package_name) const;
  bool is_processed(const std::string& package_name) const;

  void set_discovered(const std::string& package_name);
  void set_processed(const std::string& package_name);

  type_packages packages_;

  type_package_names discovered_;
  type_package_names processed_;

  int sequence_;
  std::unordered_map<std::string, int> discovered_sequence_;
  std::unordered_map<std::string, int> processed_sequence_;
  bool finish_;
};

void DependencyResolution::clear()
{
  sequence_ = 0;
  discovered_sequence_.clear();
  processed_sequence_.clear();

  discovered_.clear();

  finish_ = false;
}

std::vector<std::string> DependencyResolution::get_build_sequence(
  const std::string& package_to_build) {

  clear();

  dfs(package_to_build);

/*
  for(const auto& iter : package_depths_)
  {
    std::cout << "package: " << iter.first << ", depth: " << iter.second << std::endl;
  }
*/

  std::vector<std::string> result;
  for(const auto& the_pair : processed_sequence_) {
    result.emplace_back(the_pair.first);
  }
  std::sort(result.begin(), result.end(),
   [this] (const std::string& a, const std::string& b)
   {
     return processed_sequence_[a] < processed_sequence_[b];
   });

  return result;
}

void DependencyResolution::process_edge(const std::string& /* parent_package_name */, const std::string& package_name) {

  if(is_discovered(package_name) && !is_processed(package_name)) {
    std::cout << "Circular dependency: " << package_name << std::endl;
    finish_ = true;
  }
}

bool DependencyResolution::is_discovered(const std::string& package_name) const {
  return discovered_.count(package_name);
}

bool DependencyResolution::is_processed(const std::string& package_name) const {
  return processed_.count(package_name);
}

void DependencyResolution::set_discovered(const std::string& package_name) {
  //std::cout << "set_discovered(): " << package_name << std::endl;
  discovered_.emplace(package_name);

  //Store the entry time:
  discovered_sequence_[package_name] = sequence_;
  sequence_++;
}

void DependencyResolution::set_processed(const std::string& package_name) {
  //std::cout << "set_processed(): " << package_name << std::endl;
  processed_.emplace(package_name);

  //Store the exit time:
  processed_sequence_[package_name] = sequence_;
  sequence_++;
}

void DependencyResolution::process_node_pre(const std::string& /* package_name */) {
}

void DependencyResolution::process_node_post(const std::string& package_name) {
  //std::cout << "package: " << package_name << std::endl;
  processed_.emplace(package_name);
}

void DependencyResolution::dfs(const std::string& package_name) {
  //std::cout << "dfs: " << package_name << std::endl;

  set_discovered(package_name);

  //Get the dependencies for the package:
  const auto iter_package = packages_.find(package_name);
  if(iter_package == packages_.end()) {
    std::cout << "Unlisted dependency: " << package_name << std::endl;
    finish_ = true;
    return;
  }

  const auto& dependency_names = iter_package->second;

  for(const auto& dependency : dependency_names) {
    //std::cout << "dependency: " << dependency << std::endl;
    if(!is_discovered(dependency)) {
      process_edge(package_name, dependency);
      dfs(dependency);
    } else if (!is_processed(dependency)) {
      process_edge(package_name, dependency);
    } else {
      //std::cout << "Already processed: " << dependency << std::endl;
    }

    //Finish early if necessary:
    if(finish_)
      break;
  }

  set_processed(package_name);
}

int main()
{
  const DependencyResolution::type_packages packages{
    {"gtkmm", {"glibmm", "pangomm", "atkmm", "libsigc++", "gtk+"}},
    {"libsigc++", {}},
    {"glib", {}},
    {"pango", {"glib"}},
    {"atk", {"glib"}},
    //This circular dependency will be found: {"atk", {"glib", "gtk+"}},
    {"gtk+", {"glib", "pango", "atk"}},
    {"pangomm", {"glibmm", "libsigc++", "pango"}}, //Unnecessary libsigc++ dependency.
    {"glibmm", {"libsigc++", "glib"}},
    {"atkmm", {"glibmm", "atk"}},
    {"libxml++", {"glibmm", "libxml", "libsigc++"}} //Unnecessary libsigc++ dependency.
  };

/*
  const DependencyResolution::type_packages packages{
    {"gtkmm", {"alibsigc++", "glibmm"}},
    {"glibmm", {"alibsigc++"}},
    {"alibsigc++", {}}
  };
*/

  //Use Depth-First-Search to get a topological sorting for the graph's nodes,
  //marking each node (package name) with an increasing sequence _after_ we have
  //fully processed it - meaning after we have discovered and processed all
  //its children.

  //For instance, the start node will only be marked as processed
  //after all its child nodes have been marked as processed.
  //Therefore, the node (package name) with the lowest sequence number will
  //be the first one we should build.
  //
  //This works because, if we reach a node again via another path
  //(if another package depends on a package that we've already processed),
  //then that package will already have a lower sequence number from when it
  //was processed.
  DependencyResolution resolution(packages);

  const auto package_name = "gtkmm";
  const auto sequence = resolution.get_build_sequence(package_name);

  std::cout << "Build sequence for: " << package_name << ":" << std::endl;
  for (const auto& package : sequence) {
    std::cout << package << std::endl;
  }

  return EXIT_SUCCESS;
}


