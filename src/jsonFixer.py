import json
import glob

def fixSingleFile(fname):
    with open(fname) as infile:
        data = json.load(infile)

    try:
        shotMethod = data.pop('playStyles')
        data['shotMethod']= shotMethod

        with open(fname, 'w') as outfile:
            json.dump(data, outfile)
        print(f"FIXED     {fname}")
        
    except KeyError:
        print(f"NO CHANGE {fname}")
        
if __name__=="__main__":
    
    filenameDir = "out/gamePlay/v1.0"
    filenames = {n for n in glob.glob(f"{filenameDir}/*.json")}
    
    for fname in filenames:
        
        fixSingleFile(fname)