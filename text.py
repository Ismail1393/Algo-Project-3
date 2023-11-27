from collections import deque

# Function to perform Breadth-First Search
def bfs(start, graph, rows, cols):
    queue = deque()
    queue.append(start)
    visited = set()
    parent = {}

    while queue:
        current = queue.popleft()
        if current == (rows - 1, cols - 1):  # Reached the bull's-eye
            return construct_path(parent, current)

        visited.add(current)
        neighbors = graph[current]
        for neighbor in neighbors:
            if neighbor not in visited:
                queue.append(neighbor)
                parent[neighbor] = current

# Function to construct the path
def construct_path(parent, end):
    path = []
    while end:
        prev = parent[end]
        direction = get_direction(prev, end)
        path.append(direction)
        end = prev
    return path[::-1]

# Function to determine direction between two nodes
def get_direction(start, end):
    directions = [(0, 1, 'E'), (0, -1, 'W'), (1, 0, 'S'), (-1, 0, 'N'),
                  (-1, -1, 'NW'), (-1, 1, 'NE'), (1, -1, 'SW'), (1, 1, 'SE')]

    for dx, dy, direction in directions:
        if (start[0] + dx, start[1] + dy) == end:
            return str(direction)

# Read input from file
with open('tiny.txt', 'r') as file:
    rows, cols = map(int, file.readline().split())
    maze = [file.readline().strip().split() for _ in range(rows)]

# Create graph
graph = {}
for i in range(rows):
    for j in range(cols):
        if maze[i][j] != 'O':
            neighbors = []
            directions = {'N': (-1, 0), 'E': (0, 1), 'S': (1, 0), 'W': (0, -1),
                          'NE': (-1, 1), 'SE': (1, 1), 'SW': (1, -1), 'NW': (-1, -1)}

            row, col = i, j
            color, direction = maze[i][j].split('-')
            while True:
                dx, dy = directions[direction]
                row, col = row + dx, col + dy

                if 0 <= row < rows and 0 <= col < cols:
                    if maze[row][col].startswith(('R', 'B')):
                        neighbors.append((row, col))
                    else:
                        break
                else:
                    break

            graph[(i, j)] = neighbors

# Find path using BFS
start = (0, 0)
path = bfs(start, graph, rows, cols)

# Write output to file
with open('output.txt', 'w') as output_file:
    output_file.write(' '.join(path))
