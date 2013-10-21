
#include <trace-analysis/trace_utils.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

using namespace trace_analysis;
using namespace boost::property_tree;


int main( int argc, char **argv ) 
{

  // create a test trace
  std::stringstream ss;
  ss << "+A+ 1" << std::endl;
  ss << "+B+ 2.2" << std::endl;
  ss << "+B+ 4.2" << std::endl;
  ss << "+C+ { \"foo\" : 1 }" << std::endl;
  ss << "+A+ 2" << std::endl;
  ss << "+B+ 3.2" << std::endl;
  ss << "+C+ { \"foo\" : 3.2 }" << std::endl;
  ss << "+A+ 3" << std::endl;
  ss << "+C+ { \"foo\" : 32.2 }" << std::endl;

  // create a trace_t from it
  trace_t trace;
  trace.load( ss );
  
  // test out some groups
  std::vector<std::string> g1;
  g1.push_back( "+A+" );
  g1.push_back( "+B+" );

  std::vector< ptree > groups1 = trace.find_item_groups( g1 );
  
  std::cout << "Group 1: ";
  for( size_t i = 0; i < g1.size(); ++i ) {
    std::cout << g1[i] << " ";
  }
  std::cout << std::endl;
  for( size_t i = 0; i < groups1.size(); ++i ) {
    ptree g = groups1[i];
    json_parser::write_json( std::cout, g );
    std::cout << std::endl;
  }

  return 0;

}
