Objective:

The assignment involves generating and visualizing a map with various terrain types using C programming. The map consists of roads, grass, water, buildings (Pokémon Centers and Pokémon Marts), and boulders. The goal is to create an aesthetically pleasing and informative map using color-coded characters, which aligns with the constraints given to us in terms of minimum number of Constraints needed.

Code Highlights:

1.Custom Data Structures:
- Custom data structures are defined to store map data and terrain symbols.
- A queue is used to manage the generation of terrain features.

2. Map Generation:
- The map is generated with defined dimensions (21 rows by 80 columns).
- Boulders and initial road segments are placed around the map's edges.

3. Road Generation:
- Roads are created from random starting points and extended horizontally and vertically.
- Terrain Features:

4. Pokémon Centers and Pokémon Marts are placed along roads.
- Tall grass, short grass, and water are randomly distributed across the map.

5. Color and Formatting:
- ANSI escape codes are used to add color to the map characters.

6. Legend:
- A legend is included below the map to explain the meaning of map symbols.

7. Visualization:
- The final map is displayed in the console, visually representing the generated terrain.

8. Usage:
- To run the code, just type "make run" after going inside the folder
- To remove executable type "make clean"
- to only generate executable type "make", and sepeately see the map - "./PokeMap"


