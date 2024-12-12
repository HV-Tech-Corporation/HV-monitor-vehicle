## Client
A monitoring system that combines live data visualization with dynamic risk analysis, featuring a custom gauge dial and historical data chart.

## TEAM
- 장윤재 (Jang Yun Jae)
  - Work : Algorithm and Probability Calculation
- 조세기 (Jo Segi)
  - Work : Qt Development and Network Programming

## UI

![Best_readme](https://github.com/user-attachments/assets/b7d30c39-d69f-433e-8772-30b459983bfc) ![Risk_readme](https://github.com/user-attachments/assets/eae685d5-d9bb-49c0-b5f2-1f2911524fd2)



### 1. Stream Control

- **Play**: Starts the video stream.
- **Pause**: Pauses the video stream.
- **Rewind**: Rewind the video stream.

### 2. Best Shot and Risk Analysis Graph

- **Best Shot**: BestShot captures and stores images of detected objects by extracting the area within their boundary boxes, while also displaying key information about the objects, such as class and identification details.

- **Risk Analysis** Graph: Accident Cost Graph dynamically visualizes current road risk levels using a combination of a gauge-style dial and real-time trend graph.

### 3. Best Shot

- **Image Name**: The name of the captured frame.
- **Class**: The classification of the detected object ("bus", "car", "mortorcycle").
- **OCR**: Extracts and displays vehicle license plate numbers from the frame using OCR technology.

### 4. Risk Analysis Graph
- **Risk Dial and Status Label**: It provides visual information based on the current state through a dial and status labels.

- **Risk Analysis Graph**: The Risk Analysis Graph leverages advanced graph theory algorithms, including Minimum Spanning Tree (MST) and Minimum Spanning Forest (MSF), to calculate and visualize risk levels.

## Accident Cost Algorithm
### Class Definition and Initialization:
- The Graph class manages graph information and operations. It consists of the number of vertices, edge information, and arrays for parent and rank using the union-find data structure.
- Kruskal's Algorithm Function: Functions for constructing and calculating the Minimum Spanning Tree(MST).
- Adding Edges: The addEdge function adds edges to the graph. Edges are stored as tuples containing the start node, end node, and weight.
- Union-Find Data Structure: This structure is used to check for cycles in the graph and to connect two different vertices.
- The find function finds the root node of a given node, using path compression to improve efficiency.
- The union function merges the sets to which two nodes belong, ensuring that the smaller tree is placed under the larger tree based on rank.
### Calculating MST:
- Sort the list of edges in ascending order based on weight.
- Use Kruskal's MST algorithm to construct the minimum spanning tree, using the union-find data structure to prevent cycles.
- The union function merges the sets to which two nodes belong, ensuring that the smaller tree is placed under the larger tree based on rank.
- The find function finds the root node of a given node, using path compression to improve efficiency.
### Constructing MSF:
- When the given graph is not connected, the concept is to find the minimum spanning tree for each disconnected component. Simply, the minimum spanning forest is the collection of minimum spanning trees for each connected parts (components) of the graph.
- It consists of the minimum spanning trees for all connected parts of the graph. Find the optimal connection structure for each cluster or component and calculate the cumulative sum of the minimum connection values.
### Calculating MSF:
- Kruskal's algorithm: Sort the edges in ascending order based on weight, then add the lightest edges one by one without forming cycles.
- Each added edge forms a new tree or extends an existing tree.
### Weight Calculation:
- Calculate the nodes and costs for each connected component.
- Sum the weights of all components and output the number of connected nodes and components.
- The final calculated risk is determined using logarithmic, square root, and exponential functions. The coefficients 'C' and 'G' are used in the calculation formula to derive the final risk 'R'.
- This code uses Kruskal's MST algorithm, an important algorithm in graph theory, to find the minimum spanning tree in an actual graph, extend it to form a minimum spanning forest, and analyze the characteristics of the graph through additional weight calculations.
### Result:
This approach can be applied in various fields such as network design, communication, and optimal pathfinding.
