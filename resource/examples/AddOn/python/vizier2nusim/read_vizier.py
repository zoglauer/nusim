"""
Simple fn to read a vizier table into a dictionary
"""

def read_vizier_table(fname, verbose=1):
    """
    Read in Vizier table into a dictionary, each entry is row of data
    """

    data = {}
    header = []
    have_header = False
    data_start = False
    for line in open(fname, 'r').readlines():
        line = line.strip()
        if line == "" or line[0] == "#":
            continue
        if not have_header:
            if line.find("|") > 0:
                header = line.split("|")
                for h in header:
                    data[h] = []
                have_header = True
                if verbose > 1:
                    print "Found header"
                    print header
                continue
        #print line
        vals = line.split("|")
        if not data_start:
            if len(vals[0]) == 0:
                continue
            if vals[0][0] == "-":
                data_start = True
                if verbose > 1:
                    print "Found start of data line: ", line
        else:
            if len(vals) != len(header):
                print "Hmmm, expected %d values but line %s has %d" %\
                    (len(header), line, len(vals))
                continue
            for i in range(len(header)):
                try:
                    val = float(vals[i])
                except:
                    val = vals[i]
                data[header[i]].append(val)
    if verbose > 0: print "Read %d rows from %s" % (len(data[header[0]]), fname)
    return(data)
            
        
