
#include <trace-analysis/trace_util.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

using namespace trace_analysis;
using namespace boost::property_tree;


int main( int argc, char **argv ) 
{

  bool verbose = false;

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
  ss << "+A+ 4" << std::endl;
  ss << "+D+ 1" << std::endl;
  ss << "+E+ 2" << std::endl;
  ss << "+D+ 3" << std::endl;
  ss << "+E+ 4" << std::endl;
  



  // create a trace_t from it
  trace_t trace;
  trace.load( ss );

  // print out the read lines
  if( verbose ) {
    std::cout << "TRACE" << std::endl;
    for( size_t i = 0; i < trace.items().size(); ++i ) {
      std::cout << trace.items()[i].first << std::endl;
      json_parser::write_json( std::cout, trace.items()[i].second );
      std::cout << std::endl;
    }
  }

  //-----------------------------------------------------------------
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

  //-----------------------------------------------------------------
  // test out some groups
  std::vector<std::string> g2;
  g2.push_back( "+A+" );
  g2.push_back( "+C+" );

  std::vector< ptree > groups2 = trace.find_item_groups( g2 );
  
  std::cout << "Group 2: ";
  for( size_t i = 0; i < g2.size(); ++i ) {
    std::cout << g2[i] << " ";
  }
  std::cout << std::endl;
  for( size_t i = 0; i < groups2.size(); ++i ) {
    ptree g = groups2[i];
    json_parser::write_json( std::cout, g );
    std::cout << std::endl;
  }


  //-----------------------------------------------------------------
  // test out some groups
  std::vector<std::string> g3;
  g3.push_back( "+A+" );
  g3.push_back( "+B+" );
  g3.push_back( "+C+" );

  std::vector< ptree > groups3 = trace.find_item_groups( g3 );
  
  std::cout << "Group 3: ";
  for( size_t i = 0; i < g3.size(); ++i ) {
    std::cout << g3[i] << " ";
  }
  std::cout << std::endl;
  for( size_t i = 0; i < groups3.size(); ++i ) {
    ptree g = groups3[i];
    json_parser::write_json( std::cout, g );
    std::cout << std::endl;
  }

  //-----------------------------------------------------------------
  // test out some groups
  std::vector<std::string> g4;
  g4.push_back( "+A+" );
  g4.push_back( "+D+" );
  g4.push_back( "+E+" );

  std::vector< ptree > groups4 = trace.find_item_groups( g4 );
  
  std::cout << "Group 4: ";
  for( size_t i = 0; i < g4.size(); ++i ) {
    std::cout << g4[i] << " ";
  }
  std::cout << std::endl;
  for( size_t i = 0; i < groups4.size(); ++i ) {
    ptree g = groups4[i];
    json_parser::write_json( std::cout, g );
    std::cout << std::endl;
  }


  return 0;

}
