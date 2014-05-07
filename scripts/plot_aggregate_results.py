import json
import numpy
import matplotlib.pyplot as plt
import math

#=======================================================================

def load_aggregate_to_json( agg_filename ):
    with open( agg_filename, 'r' ) as f:
        return json.load(f)

#=======================================================================

def create_plot_data_for_single_aggregate( agg_json, trace_max_length ):
    
    # accumulate all of the traces into a vector to compute
    # means, variance, stderr
    trace_plots = []
    traces_all = []
    trace_means = []
    trace_vars = []
    trace_stderr = []
    
    for i in xrange(trace_max_length):
        traces_all.append( [] )
        trace_means.append(-1)
        trace_vars.append(-1)
        trace_stderr.append(-1)
    
    for res in agg_json['results']:
        this_trace = res['trace']
        last_obs = this_trace[-1]
        trace_plots.append( this_trace )
        for i in xrange(0,trace_max_length):
            if i < len(this_trace):
                traces_all[ i ].append( this_trace[i] )
            else:
                traces_all[ i ].append( last_obs )
                
    # ok, compute means, variance, and stderr for the traces
    for i in xrange( trace_max_length ):
        trace_means[ i ] = numpy.mean( traces_all[ i ] )
        trace_vars[ i ] = numpy.var( traces_all[ i ] )
        trace_stderr[ i ] = math.sqrt( trace_vars[ i ] ) / math.sqrt( len( traces_all[ i ] ) )

    # returnj these thngs
    return { "label" : agg_json['results'][0]['id'],
             "plots" : trace_plots, 
             "mean" : trace_means, 
             "var" : trace_vars, 
             "stderr" : trace_stderr,
             "time_mean" : agg_json[ "aggregate_results" ][ "mean" ],
             "time_var" : agg_json[ "aggregate_results" ][ "variance" ],
             "time_stderr" : agg_json[ "aggregate_results" ][ "stderr" ] }


#=======================================================================

def create_aggregate_results_plot( output_filename, max_obs, agg_filenames, labels = None ):
    plot_data = []
    for fn in agg_filenames:
        plot_data.append( create_plot_data_for_single_aggregate( load_aggregate_to_json( fn ), max_obs ) )

    colorwheel = ['r','c','g','y']
    counter = 0

    legends = []

    plt.hold(True)
    for pdata in plot_data:
        color = colorwheel[ counter % len(colorwheel) ]
        # for p in pdata['plots']:
        #     plt.plot( p, linestyle=':', linewidth=0.3, color=color )
        leg, = plt.plot( pdata['mean'], linestyle='-', color=color, linewidth=2.0, label=pdata['label'] )
        legends.append( leg )
        plt.plot( numpy.array( pdata['mean'] ) + numpy.array( pdata['stderr'] ), 'm--' )
        plt.plot( numpy.array( pdata['mean'] ) - numpy.array( pdata['stderr'] ), 'm--' )
        plt.axvline( x=pdata['time_mean'], color=color, linestyle="-", linewidth=2, label=pdata['label'] )
        plt.axvline( x=pdata['time_mean'] - pdata['time_stderr'], color='m', linestyle="--" )
        plt.axvline( x=pdata['time_mean'] + pdata['time_stderr'], color='m', linestyle="--" )
        counter = counter + 1

    if labels is None:
        plt.legend( legends, ['coverage','$\mathcal{X}^{L}$','$\mathcal{X}$'], loc=2)
    else:
        plt.legend( legends, labels, loc=2 )

    plt.xlabel( "Number of Observations" )
    plt.ylabel( "Points Found" )
    plt.tight_layout()
    plt.show()
    plt.savefig( output_filename, bbox_inches='tight' )

#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================
#=======================================================================




if __name__ == "__main__":
    plt.ion()
    import sys
    create_aggregate_results_plot( sys.argv[1],
                                   int(sys.argv[2]),
                                   sys.argv[4::2],
                                   sys.argv[3::2])
