
# a simple test of the find-groups
add_executable( object-search.trace-analysis-test-find-item-groups
  test-find-item-groups.cpp)
pods_use_pkg_config_packages( object-search.trace-analysis-test-find-item-groups object-search.trace-analysis)
pods_install_executables( object-search.trace-analysis-test-find-item-groups )
