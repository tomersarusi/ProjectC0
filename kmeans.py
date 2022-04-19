import sys

epsilon = 0.001
def kmeans(K:int, max_iter:int, inputFile:str, outputFile:str)-> None:
    global dim
    datapoints = getDataFromFile(inputFile)
    dim = len(datapoints[0])
    kmeansList = [copyArr(datapoints[i]) for i in range(K)]

    isAnyMoreThanEpsilon = True
    curIter = 0
    while isAnyMoreThanEpsilon and curIter < max_iter:
        curIter += 1
        clusterSumList = [[0 for i in range(dim)] for j in range(K)]
        clusterSizeList = [0 for i in range(K)]
        for datapoint in datapoints:
            minClusterIndex = getMinClusterIndex(datapoint, kmeansList)
            clusterSizeList[minClusterIndex] += 1
            for j in range(dim):
                clusterSumList[minClusterIndex][j] += datapoint[j]

        isAnyMoreThanEpsilon = updateKmeans(kmeansList, clusterSumList, clusterSizeList)
    writeToFile(outputFile, kmeansList)

def writeToFile(filename:str, kmeansList:list) -> None:
    with open(filename, "w") as output:
        for i in range(len(kmeansList)):
            for j in range(dim):
                output.write(format("%.4f" % kmeansList[i][j]))
                if j < dim-1:
                    output.write(",")
            output.write("\n")
    

def updateKmeans(kmeansList:list, clusterSumList:list, clusterSizeList:list) -> bool:
    isAnyMoreThanEpsilon = False
    for i,kmean in enumerate(kmeansList):
        prevKmean = copyArr(kmean)
        kmeansList[i] = copyArr(clusterSumList[i])
        for j in range(dim):
            kmeansList[i][j] /= clusterSizeList[i]
        if dist(prevKmean, kmeansList[i]) >= epsilon:
            isAnyMoreThanEpsilon = True
    return isAnyMoreThanEpsilon



def getMinClusterIndex(datapoint:list, kmeansList:list) -> int:
    minDist = float("inf")
    minClusterIndex = -1
    for i, cluster in enumerate(kmeansList):
        curDist = dist(datapoint, cluster)
        if curDist < minDist:
            minDist = curDist
            minClusterIndex = i
    return minClusterIndex

def getDataFromFile(filename:str) -> list:
    datapoints = []
    with open(filename) as input:
        fileLines = input.readlines()
        for line in fileLines:
            datapoints.append([float(i) for i in line.strip().split(',')])
    return datapoints

def copyArr(arr:list) -> list:
    return [arr[i] for i in range(len(arr))]

def dist(x:list, u:list) -> float:
    return (sum((x[i] - u[i])**2 for i in range(len(x))))**0.5

K = -1
maxIter = 200
inputFile = ""
outputFile = ""
try:
    K = int(sys.argv[1])
    maxIter = 200
    if len(sys.argv) == 5:
        maxIter = int(sys.argv[2])
        inputFile = sys.argv[3]
        outputFile = sys.argv[4]
    elif len(sys.argv) == 4:
        inputFile = sys.argv[2]
        outputFile = sys.argv[3]
    else:
        raise Exception
except Exception as e:
    print("Invalid Input!")
    exit()

try:
    kmeans(K, maxIter, inputFile, outputFile)
except Exception as e:
    print("An Error Has Occured")
    exit()