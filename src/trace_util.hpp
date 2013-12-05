
#if !defined( __P2L_TRACE_ANALYSIS_trace_util_HPP__ )
#define __P2L_TRACE_ANALYSIS_trace_util_HPP__

#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <iosfwd>

namespace trace_analysis {


  //=========================================================================

  // Description:
  // Enumerates all of the experiment files in a given directory
  std::vector<std::string> find_all_experiment_ids(const std::string& dir );
  
  //=========================================================================

  // Description:
  // A structure with the "results" of an experiment
  struct experiment_result_t
  {
    size_t total_observations;
    size_t total_points;
    std::vector<size_t> found_trace;
  };

  // Description:
  // Aggregated results for a set of experiments
  struct aggregate_results_t
  {
    std::vector<experiment_result_t> results;
    std::vector<std::string> experiment_ids;
    double mean_total_observations;
    double stderr_total_observations;
    double variance_total_observations;
  };

  //=========================================================================
  
  // Description:
  // Loads and parses the results for an experiment
  experiment_result_t load_results( const std::string& dir,
				    const std::string& id );

  //=========================================================================
  
  // Description:
  // Aggregate a set of results
  aggregate_results_t aggregate_results
  ( const std::vector<std::string>& ids,
    const std::vector<experiment_result_t>& results );

  //=========================================================================

  // Description:
  // A trace_t class which allows you to programatically query
  // traces easily.
  class trace_t
  {
  public:

    // Description:
    // Creates a trace_t object.
    // Initially empty, should call load(...).
    trace_t();

    // Description:
    // Loads a data fro ma trace stream.
    void load( std::istream& is );

    // Description:
    // Returns a list of matching sections of the trace.
    // The matches are returns which contain the given elements
    // in sequence.  The returned ptree's have keys the same as the 
    // elements in the sequence, and values with the lines from the
    // trace file.
    // 
    // All but the last element are treated as constraints that must be met
    // but not fully enumerated. The last element will be fully enumerated
    //
    // For aexample, given the following trace:
    //    +A+ 1
    //    +B+ 2.2
    //    +B+ 4.2
    //    +C+ { "foo" : 1 }
    //    +A+ 2
    //    +B+ 3.2
    //    +C+ { "foo" : 3.2 }
    //    +A+ 3
    //    +C+ { "foo" : 32.2 }
    //
    // We can ask for hte following itme groups:
    //
    //   [ "+A+", "+B+" ]
    //     => [ { "+A+" : 1,
    //            "+B+" : 2.2 },
    //          { "+A+" : 1,
    //            "+B+" : 4.2 },
    //          { "+A+" : 2,
    //            "+B+" : 3.2 } ]
    //
    //   [ "+A+", "+B+", "+C+" ]
    //     => [ { "+A+" : 1,
    //            "+B+" : 2.2 -OR- 4.2
    //            "+C+" : { "foo" : 1 } },
    //          { "+A+" : 1,
    //            "+B+" : 3.2
    //            "+C+" : { "foo" : 3.2 } },
    //          
    std::vector<boost::property_tree::ptree>
    find_item_groups( const std::vector<std::string>& group_constraints );


  protected:

    // Description:
    // a parsed list of items from a trace file.
    // The list is ordered, with the pair being the
    // tag line ( the first identifier of a line ) and the
    // value being a ptree version of the line, as parsed by JSON.
    std::vector<std::pair<std::string,boost::property_tree::ptree> > _items;

  };

  
  //=========================================================================

  //=========================================================================

}

#endif

