AdjList
Path = []
def Explore(Matrix,r,c):
    switch(Matrix[0][0].direction):
        case 'S':
            start = r,c
            end = len(Matrix),c
        case 'N':
            start = r,c
            end = 0,c
        case 'E':
            start = r,c
            end = r,len(Matrix[0])
        case 'W':
            start = r,c
            end = r,0
        case 'NW':
            start = r,c
            end = 0,0
        case 'NE':
            start = r,c
            end = 0,len(Matrix[0])
        case 'SE':
            start = r,c
            end = len(Matrix),len(Matrix[0])
        case 'SW':
            start = r,c
            end = len(Matrix),0
        case 'O':
            return []
        for i,j = (start to end):
            if (Matrix[r][c].color != Matrix[i][j].color) and not Matrix[i][j].isVisited:
                AdjList[Matrix[r][c]] += [Matrix[i][j]]
        Matrix[r][c].isVisited = True
        for node in AdjList[Matrix[r][c]]:
            Explore(node)
                path.append([distance, Matrix[r][c].direction])
    
        