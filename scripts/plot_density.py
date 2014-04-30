
import couchdb
import numpy
import math
import matplotlib
import matplotlib.pyplot as plt

colorwheel = ['r','c','g','y']
color_for_planner = {}
colorwheel_index = 0

def create_plot_data( density, planner, init_fraction, goal, results ):
    global color_for_planner
    global colorwheel
    global colorwheel_index

    trace_lengths = [ len(x["result"]["trace"]) for x in results ]
    if not planner in color_for_planner:
        color_for_planner[ planner ] = colorwheel[ colorwheel_index ]
        colorwheel_index += 1
        if colorwheel_index >= len( colorwheel ):
            colorwheel_index = 0
    color = color_for_planner[ planner ]
    return { 'label': planner,
             'color' : color,
             'planner' : planner,
             'density' : density,
             'initial_fraction' : init_fraction,
             'goal' : goal,
             'n' : len(trace_lengths),
             'mean' : numpy.mean( trace_lengths ),
             'var' : numpy.var( trace_lengths ),
             'stderr' : numpy.std( trace_lengths ) }


def plot( filename, data ):

    plt.figure()
    plt.hold(True)
    labeled_already = {}
    for d in data:
        if d['planner'] in labeled_already:
            plt.errorbar( d['density'], d['mean'], yerr=d['stderr'], fmt="o", color=d['color'], label=None)
        else:
            plt.errorbar( d['density'], d['mean'], yerr=d['stderr'], fmt="o", color=d['color'], label=d['label'])
            labeled_already[ d['planner' ] ] = True

    plt.xlabel( "Number of Observations" )
    plt.title( "Density versus Observations Needed" )
    plt.xlabel( "Density" )
    plt.ylabel( "Mean Observations" )
    plt.legend(bbox_to_anchor=(0.5, 1.05), loc=8, borderaxespad=0.)
    plt.tight_layout()
    plt.savefig( filename, bbox_inches='tight' )



def plot_by_density( filename, data ):
    lines = {}
    plt.figure()
    plt.hold(True)

    for x in data:
        if x['planner'] not in lines:
            lines[ x['planner'] ] = []
        lines[ x['planner'] ].append( x )
    for p in lines:
        line = [ (z['density'], z['mean'], z['stderr']) for z in sorted( lines[p], key=lambda l: l['density'] ) ]
        x = [ l[0] for l in line ]
        y = [ l[1] for l in line ]
        yerr = [ l[2] for l in line ]
        plt.errorbar( x,y, yerr=yerr, fmt="o--", label=p)

    plt.xlabel( "Number of Observations" )
    plt.title( "Density versus Observations Needed" )
    plt.xlabel( "Density" )
    plt.ylabel( "Mean Observations" )
    plt.legend(bbox_to_anchor=(0.5, 1.05), loc=8, borderaxespad=0.)
    plt.tight_layout()
    plt.savefig( filename, bbox_inches='tight' )


if __name__ == "__main__":

    couch = couchdb.Server()
    db = couch['rawseeds-experiment-results']

    plot_data = []
    planners = set([])
    densities = set([])
    inits = set([])
    goals = set([])
    
    
    # grab all of the known densitities
    for r in db.view( "testing_birds/density_reduced", group_level=4, reduce=True ):

        # grab key and resulting documents
        key = r.key
        density, planner_id, init, goal = key
        docs = db.view( "testing_birds/density_reduced", reduce=False )
        docs = docs[key]
        
        planners.add( planner_id )
        densities.add( density )
        inits.add(init)
        goals.add(goal)

        
        # store the plot data
        plot_data.append( create_plot_data( density, planner_id, init, goal, [ x.value for x in docs if x.value['result']['parameters']['centered_window'] == "false" ] ) )


    # ok, now plot all of the data
    for g in goals:
        output_filename = 'birds-density-noncentered-goal' + str(g) + '.eps'
        plot( output_filename, [ x for x in plot_data if x['goal'] == g] )

        # plot subsets by density
        for i in inits:
            plot_by_density( "birds-density-noncentered-goal" + str(g) + "-init" + str(i) + ".eps",
                             [x for x in plot_data if x['initial_fraction'] == i and x['goal'] == g ] )
