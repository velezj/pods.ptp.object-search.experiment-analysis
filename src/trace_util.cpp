
#include "trace_util.hpp"
#include <boost/exception/all.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>



namespace trace_analysis {


  //=========================================================================

  trace_t::trace_t()
    : _items()
  {}

  //=========================================================================

  void
  trace_t::load( std::istream& is )
  {
    std::string line;
    while( std::getline( is, line ) ) {
      std::istringstream iss( line );
      std::string tag;
      std::string raw_value;
      iss >> tag;
      std::getline( iss, raw_value );
      std::istringstream value_stream( raw_value );

      //std::cout << "RAW: tag=" << tag << " valuw=" << raw_value << std::endl;
      
      // create the resulting pair
      boost::property_tree::ptree node;

      // try to parse the value as JSON
      try {
	boost::property_tree::json_parser::read_json( value_stream, node );
	// std::cout << "..trace [" << tag << "] ";
	// boost::property_tree::xml_parser::write_xml( std::cout, node );
	// std::cout << std::endl;
      } catch ( boost::property_tree::json_parser::json_parser_error &e ) {

	// ok, try to foirce a JSON object 
	std::stringstream ss;
	ss << "{ \"" << tag << "\" : " << raw_value << " }";
	try {
	  boost::property_tree::json_parser::read_json( ss, node );
	  // std::cout << "..trace [" << tag << "] {coerced} ";
	  // boost::property_tree::xml_parser::write_xml( std::cout, node );
	  // std::cout << std::endl;
	} catch ( boost::property_tree::json_parser::json_parser_error &e2 ) {
	  std::cout << boost::diagnostic_information( e ) << std::endl;
	  node = boost::property_tree::ptree( raw_value );
	  // std::cout << "..trace [" << tag << "] {malformed} ";
	  // boost::property_tree::xml_parser::write_xml( std::cout, node );
	  // std::cout << std::endl;
	}
      }

      // ok , add to the items list
      std::pair<std::string, boost::property_tree::ptree> item( tag, node );
      _items.push_back( item );
    }
  }

  //=========================================================================

  std::vector<boost::property_tree::ptree>
  trace_t::find_item_groups( const std::vector<std::string>& group_constraints)
  {
    assert( group_constraints.empty() == false );
    if( group_constraints.empty() ) {
      BOOST_THROW_EXCEPTION( std::invalid_argument("Cannot call find_item_groups with empty query") );
    }

    bool verbose = false;

    // resutls, to be populated
    std::vector< boost::property_tree::ptree > groups;
    
    // Ok, wqe will sweep through all of the trace lines and 
    // linearly look for the sequence of keys (the group_constraints).
    // We keep track of the current key we are looking for as an index, which
    // will reset back to 0 if we failed to find a full sequence.
    //
    // We keep track of the current sequence found by index in group_indices
    size_t key_index = 0;
    std::vector< size_t > group_indices;
    for( size_t i = 0; i < _items.size(); ++i ) {
    
      // get the tag of the current line being searched
      std::string tag = _items[i].first;

      if( verbose ) {
	std::cout << "[" << i << " | " << key_index << "] tag: " << tag << "  match: <";
	for( size_t j = 0; j < group_indices.size(); ++j ) {
	  std::cout << group_indices[j] << ",";
	}
	std::cout << ">" << std::endl;
      }
      
      // if the tag is the key we want, either incremenet the key index
      // or not depending on whether it is the last index
      if( tag == group_constraints[ key_index ] ) {
	if( key_index + 1 < group_constraints.size() ) {
	  ++key_index;
	  group_indices.push_back( i );
	} else {
	  
	  // in order to find all squeneces with the last kjey, 
	  // we do *not* change the key_index here, but we do
	  // add the found sequence to the list of resutls
	  boost::property_tree::ptree g;
	  for( size_t j = 0; j < group_indices.size(); ++j ) {
	    g.put_child( _items[group_indices[j]].first,
			 _items[group_indices[j]].second );
	  }
	  g.put_child( _items[i].first,
		       _items[i].second );
	  groups.push_back( g );
	  
	  if( verbose ) {
	    std::cout << "--> match <";
	    for( size_t j = 0; j < group_indices.size(); ++j ) {
	      std::cout << group_indices[j] << ",";
	    }
	    std::cout << i << ">" << std::endl;
	  }
	}

	// at this point, we have matched something so continue with next line
	continue;
      }


      // Check if current tag matches a previous key, in which case we
      // need to start over from after that key
      for( size_t j = 0; j < key_index; ++j ) {
	if( tag == group_constraints[j] ) {

	  
	  // now, we want to jump back to the next line after
	  // the previous matched
	  // and start looking for a new match
	  // NOTE: we *do not* add one , since hte for loop
	  //       will incremenet i after this!
	  i = group_indices[ j ];

	  // ok, clear the group indices found up to the
	  // matched previous key
	  for( size_t k = j; k < key_index; ++k ) {
	    group_indices.pop_back();
	  }

	  // we want to find the previous key again
	  key_index = j;

	  if( verbose ) {
	    std::cout << "!reset downto i=" << i << std::endl;
	  }

	  break;
	}
      }
    }
    
    // return the found groups
    return groups;
  }

  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  


}
