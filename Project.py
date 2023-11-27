def read_file_and_create_2d_array(file_path):
    with open(file_path, 'r') as file:
        n, m = map(int, file.readline().strip().split())
        two_d_array = []
        index = 0 
        for _ in range(n):
            row_data = file.readline().strip().split()
            for i in range(m):
                row_data[i] = Arrow(row_data[i], index)
                index += 1
            two_d_array.append(row_data)
    return two_d_array

class Arrow:
    def __init__(self, input_string, ind):
        # Initialize the class with the input string
        self.input_string = input_string

        # Extract color and direction from the input string
        self.color, self.direction = self.parse_input_string()
        
        #if node has been visited
        self.visited = False

        #nodes distance to next move
        self.distance = None

        #index of that node
        self.index = ind

    

    def parse_input_string(self):
        # Split the input string (assuming a space separates color and direction)
        parts = self.input_string.split("-")

        # Ensure that there are at least two parts (color and direction)
        if len(parts) == 2:
            color = parts[0]
            direction = parts[1]
        else:
            color = None
            direction = parts[0]
        return color, direction

file_path = 'tiny.txt'  
Matrix = read_file_and_create_2d_array(file_path)

#Initialize adjacency list
AdjList = {}

#Loops to iterate through every single node
for row in range(len(Matrix)):
    for col in range(len(Matrix[0])):
        
        #create a key for every node
        AdjList[Matrix[row][col].index]  = []
        
    
        if Matrix[row][col].direction == "S":
            k = row + 1 
            while k < len(Matrix):
                if Matrix[k][col].color is not Matrix[row][col].color:
                    AdjList[Matrix[row][col].index].append(Matrix[k][col].index)           
                k += 1
        if Matrix[row][col].direction == "N":
            k = row - 1 
            while k >= 0:
                if Matrix[k][col].color is not Matrix[row][col].color:
                    AdjList[Matrix[row][col].index].append(Matrix[k][col].index)
                k -= 1
        if Matrix[row][col].direction == "E":
            k = col + 1 
            while k < len(Matrix[0]):
                if Matrix[row][k].color is not Matrix[row][col].color:
                    AdjList[Matrix[row][col].index].append(Matrix[row][k].index)
                k += 1
        if Matrix[row][col].direction == "W":
            k = col - 1 
            while k >= 0:
                if Matrix[row][k].color is not Matrix[row][col].color:
                    AdjList[Matrix[row][col].index].append(Matrix[row][k].index)
                k -= 1
        if Matrix[row][col].direction == "NE":
            k, l = row -1, col + 1  
            while k >= 0 and l < len(Matrix[0]):
                if Matrix[k][l].color is not Matrix[row][col].color:
                    AdjList[Matrix[row][col].index].append(Matrix[k][l].index)
                k -= 1
                l += 1
        if Matrix[row][col].direction == "NW":
            k, l = row - 1, col - 1  
            while k >= 0 and l >= 0:
                if Matrix[k][l].color is not Matrix[row][col].color:
                    AdjList[Matrix[row][col].index].append(Matrix[k][l].index)
                k -= 1
                l -= 1
        if Matrix[row][col].direction == "SW":
            k, l = row + 1, col - 1  
            while k >= len(Matrix) and l >= 0:
                if Matrix[k][l].color is not Matrix[row][col].color:
                    AdjList[Matrix[row][col].index].append(Matrix[k][l].index)
                k += 1
                l -= 1
        if Matrix[row][col].direction == "SE":
            k, l = row + 1, col + 1  
            while k >= len(Matrix) and l < len(Matrix[0]):
                if Matrix[k][l].color is not Matrix[row][col].color:
                    AdjList[Matrix[row][col].index].append(Matrix[k][l].index)
                k += 1
                l += 1





print(AdjList)



