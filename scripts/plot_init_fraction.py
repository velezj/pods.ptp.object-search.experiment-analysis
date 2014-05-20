import couchdb
import numpy
import math
import matplotlib
import matplotlib.pyplot as plt


colorwheel = ['b','g','r','c','m','y','k']
seen_color_keys = {}
colorwheel_index = 0


def color_for_key( k ):
    global colorwheel
    global seen_color_keys
    global colorwheel_index
    key = tuple(k[3:])
    if key in seen_color_keys:
        h = seen_color_keys[ key ]
    else:
        h = colorwheel_index
        seen_color_keys[ key ] = h
        colorwheel_index = colorwheel_index + 1
    return colorwheel[ h % len(colorwheel) ]

def label_id_for_key( k ):
    planner, model, centered = k[3:]
    if planner.find("coverage") >= 0:
        return tuple( [ "coverage", centered ] )
    else:
        return tuple(k[3:])

def label_for_key( k ):
    planner_orig, model, centered = k[3:]
    planner = planner_orig

    # rewrite known palmnnner names
    if planner_orig.find('entropy') >= 0:
        planner = 'Ent'
    if planner_orig.find('coverage') >= 0:
        planner = 'Coverage'
    if planner_orig.find('shortest') >= 0:
        toks = planner_orig.split("_")
        planner = 'SamplePath$^{' + toks[-2] + '}$'
    
    # specify gridding here
    toks = planner_orig.split('_')
    if toks[-1].find('grid') >= 0:
        #print "tok = " + toks[-1] + "  :: " + toks[-1][:-4]
        grid = float( toks[-1][:-4] )
        planner = planner + '$_{' + str(grid) + '}$'
    
    # rewrite known model names
    if model.find('igmm') >= 0:
        model = '$\mathcal{X}^L$'
    if model.find('mean') >= 0:
        toks = model.split("_")
        #print "Model Toks[-1] = " + toks[-1]
        model = '$\mathcal{X}_{' + toks[-1] + '}$'
    if planner_orig.find('coverage') >= 0:
        model = ""
    
    if centered == "true":
        return planner + " " + model + " centered"
    return planner + " " + model


if __name__ == "__main__":

    couch = couchdb.Server()
    db = couch['rawseeds-experiment-results']
    view_name = "plot_init_fraction/plot_data"

    # ampping from identifier to plot objects (figures)
    plot_axes_map = {}
    plot_fig_map = {}

    # set of labels already used, mapped by plot_id 
    plot_labeled_already = {}
    
    # grab all of the known results (this is a reduced view which actually 
    # computes the statistics of the trace length!)
    for r in db.view( view_name, group_level=6, reduce=True ):

        # grab key and resulting documents
        key = r.key
        init_fraction, goal_find, world, planner, model, centered = key
        stats = r.value

        # skip certain planners/world pairs
        if planner.find("200grid") >= 0:
            continue
        if planner.find("shortest") >= 0:
            continue
        if centered == "false":
            continue
        
        # get the identifier for the plot to add data to
        plot_id = tuple( [goal_find, world] )
        axes = None
        if plot_id not in plot_axes_map:
            fig = plt.figure()
            fig.set_tight_layout(True)
            fig.hold(True)
            plot_fig_map[ plot_id ] = fig
            plot_axes_map[ plot_id ] = plot_fig_map[ plot_id ].gca()
            plot_axes_map[ plot_id ].set_xlim( 0.0, 1.0 )
        axes = plot_axes_map[ plot_id ]

        # get the label and color
        l = label_for_key( key )
        lid = label_id_for_key( key )
        if plot_id not in plot_labeled_already:
            plot_labeled_already[ plot_id ] = set([])
        if lid in plot_labeled_already[ plot_id ]:
            l = None
        else:
            plot_labeled_already[ plot_id ].add( lid )
        c = color_for_key( key )
        
        # ok, now add the error bar data point from result
        n = float(stats['count'])
        s = float(stats['sum'])
        ss = float(stats['sumsqr'])
        x = [ float(init_fraction) * float(init_fraction) ]
        y = [ s / n ]
        yerr = [ math.sqrt( ss/n - (s/n * s/n) ) / math.sqrt(n) ]
        axes.errorbar( x, y, yerr=yerr, label=l, color=c )

    # ok, now tight leyout all figures and save
    for k,fig in plot_fig_map.iteritems():
        ax = plot_axes_map[k]
        ax.set_title( "Observations Mean $\pm$ StdErr by Initial Fraction" )
        ax.set_xlabel( "Initial Fraction" )
        ax.set_ylabel( "# Observations" )
        ax.legend(bbox_to_anchor=(0.5, 1.05), loc=8, borderaxespad=0.)
        fig.tight_layout()
        filename = "initfraction-plots/initfraction-goal" + str(k[0]) + "-" + str(k[1]) + "-001.eps"
        fig.savefig( filename, bbox_inches='tight' )

