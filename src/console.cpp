
#include <iostream>
#include <sstream>
#include <fstream>
#include <trace-analysis/trace_util.hpp>
#include <boost/regex.hpp>

using namespace trace_analysis;

int main( int argc, char** argv )
{

  std::vector<std::string> ids;
  std::vector<experiment_result_t> results;
  
  while( true ) {

    std::string dir;
    std::cout << "Look in Directory> ";
    std::cin >> dir;
    
    std::vector<std::string> temp_ids;
    temp_ids = find_all_experiment_ids( dir );
    
    std::cout << "Found " << temp_ids.size() << " experiments" << std::endl;
    
    std::string regex_string;
    std::cout << "Filter regex> ";
    std::cin >> regex_string;
    boost::regex regex( regex_string );
    
    std::vector<std::string> potantial_ids;
    for( auto id : temp_ids ) {
      if( boost::regex_match( id, regex ) ) {
	potantial_ids.push_back( id );
	std::cout << "   passed: " << id << std::endl;
      }
    }
    std::cout << "Filtered down to " << potantial_ids.size() << " experiments" << std::endl;

    temp_ids.clear();
    std::cout << "Total Observations Cap> ";
    size_t max_obs;
    std::cin >> max_obs;

    for( auto id : potantial_ids ) {
      experiment_result_t res = load_results( dir, id );
      if( res.total_observations <=  max_obs ) {
	temp_ids.push_back( id );
      }
    }
    potantial_ids = temp_ids;
    std::cout << "Filtered down to " << potantial_ids.size() << " experiments" << std::endl;

    temp_ids.clear();
    std::cout << "Min Observations Allowed> ";
    size_t min_obs;
    std::cin >> min_obs;

    for( auto id : potantial_ids ) {
      experiment_result_t res = load_results( dir, id );
      if( res.total_observations >=  min_obs ) {
	temp_ids.push_back( id );
      }
    }
    potantial_ids = temp_ids;
    std::cout << "Filtered down to " << potantial_ids.size() << " experiments" << std::endl;

 
    std::string accept;
    std::cout << "Accept experiments[y/n]> ";
    std::cin >> accept;
    
    if( accept == "y" ) {
      ids.insert( ids.end(), potantial_ids.begin(), potantial_ids.end() );
      for( auto id : potantial_ids ) {
	results.push_back( load_results( dir, id ) );
      }	    
      std::cout << "   total ids: " << ids.size() << std::endl;
    }

    std::cout << "Load more[y/n]> ";
    std::cin >> accept;
    
    if( accept == "n" ) {
      break;
    }

  }

  // ok, we have loaded the wanted ids, now compute aggregate
  aggregate_results_t agg = aggregate_results( ids, results );
  std::cout << "Total Observations --  Mean: " 
	    << agg.mean_total_observations
	    << "  Var: " << agg.variance_total_observations
	    << "  StdErr: " << agg.stderr_total_observations
	    << std::endl;

  
  // Print out the aggregate results to a file
  std::string out_name;
  std::cout << "Ouput Filename> ";
  std::cin >> out_name;
  
  std::ofstream fout( out_name );
  fout << "{ \"aggregate_results\" : "
       << "{ \"mean\" : " << agg.mean_total_observations 
       << ", \"stderr\" : " << agg.stderr_total_observations
       << ", \"variance\" : " << agg.variance_total_observations
       << "},"
       << "  \"results\" : [";
  for( size_t i = 0; i < ids.size(); ++i ) {
    std::string id = ids[i];
    experiment_result_t res = results[i];
    fout << "{ \"id\" : \"" << id << "\","
	 << " \"observations\" : " << res.total_observations << ","
	 << " \"points\" : " << res.total_points << ","
	 << " \"trace\" : [";
    for( size_t j = 0; j < res.found_trace.size(); ++j ) {
      fout << res.found_trace[j];
      if( j + 1 < res.found_trace.size() ) {
	fout << ",";
      }
    }
    fout << "]}";
    if( i + 1 < ids.size() ) {
      fout << ",";
    }
  }
  fout << "]}" << std::endl;

  return 0;
}
