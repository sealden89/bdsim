import numpy as np

def GenerateBins(nBins, minValue, maxValue):
    v = np.sort(np.random.rand(nBins-1))*(maxValue-minValue) + minValue
    v = np.insert(v,0,minValue)
    v = np.append(v,maxValue)
    return v

def WriteBins(data, filename):
    f = open(filename, "w")
    f.write("\n".join(map(str,data)) + "\n")
    f.close()
                
def main():
    x = GenerateBins(23,-0.5,0.5)
    WriteBins(x, "bins-x.txt")
    y = GenerateBins(14,-0.2,0.5)
    WriteBins(y, "bins-y.txt")
    z = GenerateBins(101,0,3.5)
    WriteBins(z, "bins-z.txt")

if __name__ == "__main__":
    main()
