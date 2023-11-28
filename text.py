def create_adjacency_list(Matrix):
    AdjList = {}

    # Define directions and their corresponding changes in row and column indices
    directions = {
        "N": (-1, 0), "S": (1, 0),
        "E": (0, 1), "W": (0, -1),
        "NE": (-1, 1), "NW": (-1, -1),
        "SE": (1, 1), "SW": (1, -1),
        "O": (0, 0)  # Not sure what "O" represents, assuming no movement
    }

    rows, cols = len(Matrix), len(Matrix[0])

    for row in range(rows):
        for col in range(cols):
            #AdjList[Matrix[row][col].index] = []
            AdjList[Matrix[row][col]] = []
            current_direction = Matrix[row][col].direction
            current_color = Matrix[row][col].color

            if current_direction == "O":
                #AdjList[Matrix[row][col].index].append(-1)
                AdjList[Matrix[row][col]].append(-1)
                continue

            dx, dy = directions[current_direction]

            k, l = row + dx, col + dy
            while 0 <= k < rows and 0 <= l < cols:
                if Matrix[k][l].color != current_color:
                    #AdjList[Matrix[row][col].index].append(Matrix[k][l].index)
                    AdjList[Matrix[row][col]].append(Matrix[k][l])
                k += dx
                l += dy

    return AdjList


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
        self.input_string = input_string
        self.color, self.direction = self.parse_input_string()
        self.visited = False
        self.index = ind

    def parse_input_string(self):
        parts = self.input_string.split("-")
        if len(parts) == 2:
            color = parts[0]
            direction = parts[1]
        else:
            color = None
            direction = parts[0]
        return color, direction

file_path = 'tiny.txt'
Matrix = read_file_and_create_2d_array(file_path)
AdjList = create_adjacency_list(Matrix)
print(AdjList)


