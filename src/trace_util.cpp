
#include "trace_util.hpp"
#include <boost/exception/all.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>


using namespace boost::filesystem;


namespace trace_analysis {

  //=========================================================================

  std::vector<std::string> find_all_experiment_ids( const std::string& dir )
  {
    path root(dir);
    std::vector<std::string> ids;
    for( auto it = directory_iterator(root);
	 it != directory_iterator();
	 ++it ) {

      if( it->path().extension() == ".trace" ) {
	ids.push_back( it->path().stem().string() );
      }
    }
    
    return ids;
  }

  //=========================================================================

  experiment_result_t load_results( const std::string& dir,
				    const std::string& id )
  {
    experiment_result_t res;
    
    path root(dir);
    path trace_path( root / (id + ".trace" ) );
    std::ifstream is( trace_path.c_str() );
    std::string line;
    size_t obs_num;
    while( std::getline( is, line ) ) {
      std::istringstream iss( line );
      iss >> obs_num;
      std::string temp_string;
      iss >> temp_string >> temp_string;
      size_t found;
      iss >> found;
      res.found_trace.push_back( found );
    }
    
    res.total_observations = obs_num;
    res.total_points = res.found_trace.back();
    return res;
  }
  

  //=========================================================================

  aggregate_results_t aggregate_results
  ( const std::vector<std::string>& ids,
    const std::vector<experiment_result_t>& results )
  {
    aggregate_results_t res;
    res.results = results;
    res.experiment_ids = ids;
    double sum = 0;
    for( auto r : results ) {
      sum += r.total_observations;
    }
    res.mean_total_observations = sum / results.size();
    double var = 0;
    for( auto r : results ) {
      var += 
	( r.total_observations - res.mean_total_observations ) *
	( r.total_observations - res.mean_total_observations );
    }
    var /= ids.size();
    res.variance_total_observations = var;
    res.stderr_total_observations = sqrt( var ) / ( results.size() - 1 );
    return res;
  }


  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  //=========================================================================
  

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
      
      // create the resulting pair
      boost::property_tree::ptree node;

      // try to parse the value as JSON
      try {
	boost::property_tree::json_parser::read_json( raw_value, node );
      } catch ( boost::property_tree::json_parser::json_parser_error &e ) {
	node.put_value( raw_value );
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
	    g.put_child( _items[j].first,
			 _items[j].second );
	  }
	  g.put_child( _items[i].first,
		       _items[i].second );
	  groups.push_back( g );
	}

	// at this point, we have matched something so continue with next line
	continue;
      }


      // Check if current tag matches a previous key, in which case we
      // need to start over from after that key
      for( size_t j = 0; j < key_index; ++j ) {
	if( tag == group_constraints[j] ) {

	  // clean up the current matched sequence of indices
	  // by removing the mathced keys *after* the one we just found
	  for( size_t k = j + 1; k < key_index; ++j ) {
	    group_indices.pop_back();
	  }
	  
	  // found previous key, reset to now looking for the key
	  // after it!
	  key_index = j + 1;
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
