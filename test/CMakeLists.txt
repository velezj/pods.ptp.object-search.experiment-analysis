
# a simple test of the find-groups
add_executable( p2l-trace-analysis-test-find-item-groups
  test-find-item-groups.cpp)
pods_use_pkg_config_packages( p2l-trace-analysis-test-find-item-groups p2l-trace-analysis)
pods_install_executables( p2l-trace-analysis-test-find-item-groups )
