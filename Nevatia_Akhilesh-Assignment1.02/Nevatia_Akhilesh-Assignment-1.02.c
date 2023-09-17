#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COLOR_RESET "\x1B[0m"
#define COLOR_GREEN "\x1B[32m"
#define COLOR_BLUE "\x1B[34m"
#define COLOR_RED "\x1B[31m"
#define COLOR_YELLOW "\x1B[33m"
#define COLOR_CYAN "\x1B[36m"
#define COLOR_MAGENTA "\x1B[35m"

void printColoredChar(char character)
{
    switch (character)
    {
    case '.':
        printf(COLOR_GREEN "%c" COLOR_RESET, character);
        break;
    case '#':
        printf(COLOR_BLUE "%c" COLOR_RESET, character);
        break;
    case ':':
        printf(COLOR_RED "%c" COLOR_RESET, character);
        break;
    case '~':
        printf(COLOR_CYAN "%c" COLOR_RESET, character);
        break;
    case '^':
        printf(COLOR_YELLOW "%c" COLOR_RESET, character);
        break;
    case 'C':
        printf(COLOR_MAGENTA "%c" COLOR_RESET, character);
        break;
    case 'M':
        printf(COLOR_MAGENTA "%c" COLOR_RESET, character);
        break;
    default:
        printf("%c", character);
    }
}

void printLegend()
{
    printf("Legend:\n");
    printf(COLOR_GREEN " . " COLOR_RESET ": Short Grass\n");
    printf(COLOR_BLUE " # " COLOR_RESET ": Road\n");
    printf(COLOR_RED " : " COLOR_RESET ": Tall Grass\n");
    printf(COLOR_CYAN " ~ " COLOR_RESET ": Water\n");
    printf(COLOR_YELLOW " ^ " COLOR_RESET ": Tree \n");
    printf(COLOR_MAGENTA " C " COLOR_RESET ": Pokémon Center\n");
    printf(COLOR_MAGENTA " M " COLOR_RESET ": Pokémon Mart\n");
}

// Keeping a track of which map is the current map using custom made Struct which updates and handles edge cases
typedef struct
{
    int x;
    int y;
    void (*updateX)(struct CurrMap *, int);
    void (*updateY)(struct CurrMap *, int);
} CurrMap;

// Function to update the x coordinate with bounds checking
void updateX(CurrMap *currMap, int newX)
{
    if (newX < 0)
    {
        currMap->x = 0;
        printf("Cannot move further up");
    }
    else if (newX > 400)
    {
        currMap->x = 400;
        printf("Cannot move further down");
    }
    else
    {
        currMap->x = newX;
    }
}

// Function to update the y coordinate with bounds checking
void updateY(CurrMap *currMap, int newY)
{
    if (newY < 0)
    {
        currMap->y = 0;
        printf("Cannot move further west");
    }
    else if (newY > 400)
    {
        currMap->y = 400;
        printf("Cannot move further east");
    }
    else
    {
        currMap->y = newY;
    }
}

// Define a custom class to store x, y coordinates, and a character value
struct CustomData
{
    int x;
    int y;
    char value;
};

// Define a structure for the queue node that holds CustomData objects
struct QueueNode
{
    struct CustomData data;
    struct QueueNode *next;
};

// Define a structure for the queue itself
struct Queue
{
    struct QueueNode *front;
    struct QueueNode *rear;
};

// Function to create an empty queue
struct Queue *createQueue()
{
    struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
    if (!queue)
    {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    queue->front = queue->rear = NULL;
    return queue;
}

// Function to enqueue a CustomData object to the rear of the queue
void enqueue(struct Queue *queue, struct CustomData data)
{
    struct QueueNode *newNode = (struct QueueNode *)malloc(sizeof(struct QueueNode));
    if (!newNode)
    {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    newNode->data = data;
    newNode->next = NULL;

    if (queue->rear == NULL)
    {
        queue->front = queue->rear = newNode;
        return;
    }

    queue->rear->next = newNode;
    queue->rear = newNode;
}

// Function to dequeue a CustomData object from the front of the queue
struct CustomData dequeue(struct Queue *queue)
{
    if (queue->front == NULL)
    {
        printf("Queue is empty\n");
        exit(EXIT_FAILURE);
    }

    struct QueueNode *temp = queue->front;
    struct CustomData data = temp->data;
    queue->front = queue->front->next;

    // If front becomes NULL, set rear to NULL as well
    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(temp);
    return data;
}

// Function to check if the queue is empty

int isEmpty(struct Queue *queue)
{
    return queue->front == NULL;
}

char **printmap(char gate, int index)
{

    int rows = 21;
    int cols = 80;
    char map[rows][cols]; // dimensions of the map we want to generate

    time_t t;

    srand((unsigned)time(&t)); // created seed based on current time
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (i == 0 || i == (rows - 1) || j == 0 || j == (cols - 1))
            {
                map[i][j] = '%'; // boulders
            }
            else
            {
                map[i][j] = '.'; // placeholders
                continue;
            }
        }
    }

    // PLACING ROADS FROM EAST TO WEST AND NORTH TO SOUTH

    // PLACING ROADS FROM EAST TO WEST OR RANDOM BASED ON GATE VALUE
    if (gate == 'w')
    {

        int dx[] = {0, 1};
        int dy[] = {1, 0};

        int Hstartx = index;
        int horzx = Hstartx;
        int horzy = 0;
        map[horzx][horzy] = '#';
        horzy++; // initial step to get out of boulders
        map[horzx][horzy] = '#';
        // int Hstarty = rand()%80;

        while (horzy != 79)
        {
            int dirn = (rand() % 2); // as we only want to go ahead in terms of cols
            if ((map[horzx + dx[dirn]][horzy + dy[dirn]]) != '%')
            {
                horzx += dx[dirn];
                horzy += dy[dirn];
                map[horzx][horzy] = '#';
            }
            else
            {
                horzy++;
                map[horzx][horzy] = '#';
            }
        }
        if (horzy == 79)
        {
            map[horzx][horzy] = '#'; // exitgate for horz path
        }
    }
    else if (gate == 'e')
    {

        int dx[] = {0, 1};
        int dy[] = {-1, 0};

        int Hstartx = index;
        int horzx = Hstartx;
        int horzy = 79;
        map[horzx][horzy] = '#';
        horzy--; // initial step to get out of boulders
        map[horzx][horzy] = '#';
        // int Hstarty = rand()%80;

        while (horzy != 0)
        {
            int dirn = (rand() % 2); // as we only want to go ahead in terms of cols
            if ((map[horzx + dx[dirn]][horzy + dy[dirn]]) != '%')
            {
                horzx += dx[dirn];
                horzy += dy[dirn];
                map[horzx][horzy] = '#';
            }
            else
            {
                horzy--;
                map[horzx][horzy] = '#';
            }
        }
        if (horzy == 0)
        {
            map[horzx][horzy] = '#'; // exitgate for horz path
        }
    }
    else
    {

        int dx[] = {0, 1};
        int dy[] = {1, 0};

        int Hstartx = 1 + rand() % 19;
        int horzx = Hstartx;
        int horzy = 0;
        map[horzx][horzy] = '#';
        horzy++; // initial step to get out of boulders
        map[horzx][horzy] = '#';
        // int Hstarty = rand()%80;

        while (horzy != 79)
        {
            int dirn = (rand() % 2); // as we only want to go ahead in terms of cols
            if ((map[horzx + dx[dirn]][horzy + dy[dirn]]) != '%')
            {
                horzx += dx[dirn];
                horzy += dy[dirn];
                map[horzx][horzy] = '#';
            }
            else
            {
                horzy++;
                map[horzx][horzy] = '#';
            }
        }
        if (horzy == 79)
        {
            map[horzx][horzy] = '#'; // exitgate for horz path
        }
    }

    if (gate == 'n')
    {

        int Hstarty = index;
        int verty = Hstarty;
        int vertx = 0;
        map[vertx][verty] = '#';
        vertx++; // next line moving
        map[vertx][verty] = '#';

        int cx[] = {0, 1, 0};
        int cy[] = {-1, 0, 1};

        while (vertx != 20)
        {
            int dirn = (rand() % 3); // as we only want to go ahead in terms of cols
            if ((map[vertx + cx[dirn]][verty + cy[dirn]]) != '%')
            {
                vertx += cx[dirn];
                verty += cy[dirn];
                map[vertx][verty] = '#';
            }
            else
            {
                vertx++;
                map[vertx][verty] = '#';
            }
        }
        if (vertx == 20)
        {
            map[vertx][verty] = '#'; // so that exit gate exists in the south side
        }
    }
    else if (gate == 's')
    {

        printf("hi");
        int Hstarty = index;
        int verty = Hstarty;
        int vertx = 20; // last row go upi from there
        map[vertx][verty] = '#';
        vertx--; // next line moving
        map[vertx][verty] = '#';

        int cx[] = {0, -1, 0};
        int cy[] = {-1, 0, 1};

        while (vertx != 0)
        {
            int dirn = (rand() % 3); // as we only want to go ahead in terms of cols
            if ((map[vertx + cx[dirn]][verty + cy[dirn]]) != '%')
            {
                vertx += cx[dirn];
                verty += cy[dirn];
                map[vertx][verty] = '#';
            }
            else
            {
                vertx--;
                map[vertx][verty] = '#';
            }
        }
        if (vertx == 0)
        {
            map[vertx][verty] = '#'; // so that exit gate exists in the south side
        }
    }
    else
    {

        int Hstarty = 1 + rand() % 78;

        int verty = Hstarty;
        int vertx = 0;
        map[vertx][verty] = '#';
        vertx++; // next line moving
        map[vertx][verty] = '#';

        int cx[] = {0, 1, 0};
        int cy[] = {-1, 0, 1};

        while (vertx != 20)
        {
            int dirn = (rand() % 3); // as we only want to go ahead in terms of cols
            if ((map[vertx + cx[dirn]][verty + cy[dirn]]) != '%')
            {
                vertx += cx[dirn];
                verty += cy[dirn];
                map[vertx][verty] = '#';
            }
            else
            {
                vertx++;
                map[vertx][verty] = '#';
            }
        }
        if (vertx == 20)
        {
            map[vertx][verty] = '#'; // so that exit gate exists in the south side
        }
    }

    // ATTACHING POKEMON CENTERS TO THE ROADS

    int Py = rand() % 70 + 6;
    int Px = rand() % 10 + 5;

    int aroundx[] = {0, 1, 0, -1, 1, -1, 1, -1};
    int aroundy[] = {1, 0, -1, 0, -1, 1, 1, -1};

    int free = 0;

    while (free == 0)
    {
        for (int i = 0; i < 8; i++)
        {
            int changex = Px + aroundx[i];
            int changey = Py + aroundy[i];
            if (map[changex][changey] == '.')
            {
                continue;
            }
            else
            {
                Py = rand() % 70 + 6;
                Px = rand() % 10 + 5;
                break;
            }
        }
        free = 1;
    }

    map[Px][Py] = 'C';
    map[Px + 1][Py] = 'C';
    map[Px][Py + 1] = 'C';
    map[Px + 1][Py + 1] = 'C';
    Py = Py + 2;
    while (map[Px][Py] != '#')
    {
        map[Px][Py] = '#';
        Px++;
    }

    // ATTACHING POKEMON MART TO THE ROAD

    int My = rand() % 70 + 6;
    int Mx = rand() % 10 + 5;

    free = 0;

    while (free == 0)
    {
        for (int i = 0; i < 8; i++)
        {
            int changex = Mx + aroundx[i];
            int changey = My + aroundy[i];
            if (map[changex][changey] == '.')
            {
                continue;
            }
            else
            {
                My = rand() % 70 + 6;
                Mx = rand() % 10 + 5;
                break;
            }
        }
        free = 1;
    }

    map[Mx][My] = 'M';
    map[Mx + 1][My] = 'M';
    map[Mx][My + 1] = 'M';
    map[Mx + 1][My + 1] = 'M';
    My = My + 2;
    while (map[Mx][My] != '#')
    {
        map[Mx][My] = '#';
        Mx++;
    }

    // FILLING MAP UP WITH DIFFERENT TERRAINS USING CUSTOM MADE QUEUE

    struct Queue *queue = createQueue();

    // FILLING UP TALL GRASS

    for (int i = 0; i < 3; i++)
    {
        int Ry = rand() % 80;
        int Rx = rand() % 21;

        while (map[Rx][Ry] != '.')
        {
            Ry = rand() % 80;
            Rx = rand() % 21;
        }
        struct CustomData data = {Rx, Ry, ':'};
        enqueue(queue, data);
    }

    // WATER IN THE QUEUE

    for (int i = 0; i < 2; i++)
    {
        int Ry = rand() % 80;
        int Rx = rand() % 21;

        while (map[Rx][Ry] != '.')
        {
            Ry = rand() % 80;
            Rx = rand() % 21;
        }
        struct CustomData data = {Rx, Ry, '~'};
        enqueue(queue, data);
    }

    // PUTTING SHORT GRASS IN THE QUEUE AT 2 LOCATIONS
    for (int i = 0; i < 3; i++)
    {
        int Ry = rand() % 80;
        int Rx = rand() % 21;

        while (map[Rx][Ry] != '.')
        {
            Ry = rand() % 80;
            Rx = rand() % 21;
        }
        struct CustomData data = {Rx, Ry, '^'};
        enqueue(queue, data);
    }

    // GOING THROUGH THE FILLED QUEUE AND EXPANDING REGIONS TILL OTHER REGION DISCOVERED

    while (!(isEmpty(queue)))
    {

        struct CustomData node = dequeue(queue);
        int x = node.x;
        int y = node.y;
        int terrain = node.value;
        for (int i = 0; i < 8; i++)
        {
            if (map[x + aroundx[i]][y + aroundy[i]] == '.')
            {
                map[x + aroundx[i]][y + aroundy[i]] = terrain;
                struct CustomData InputData = {x + aroundx[i], y + aroundy[i], terrain};
                enqueue(queue, InputData);
            }
        }
    }

    for (int i = 0; i < 10; i++)
    {

        int row = rand() % 15 + 3;
        int col = rand() % 70 + 5;

        if (map[row][col] != '#' && map[row][col] != 'M' && map[row][col] != 'C')
        {
            map[row][col] = '%';
        }
    }

    // PRINTING BOARD FORMULATED

    for (int i = 0; i < rows; i++)
    {

        for (int j = 0; j < cols; j++)
        {
            // SWITCH TREE AND SHORT GRASS SYMBOLS WITH TREE AS SHORT GRASS WAS USED AS PLACE HOLDER IN CODE INITIALLY
            if (map[i][j] == '^')
            {

                printColoredChar('.');
            }
            else if (map[i][j] == '.')
            {

                printColoredChar('^');
            }
            else
            {
                printColoredChar(map[i][j]);
            }
        }
        printf("\n");
    }

    // Allocate memory for a 2D char array
    char **mapArray = (char **)malloc(21 * sizeof(char *));
    for (int i = 0; i < 21; i++)
    {
        mapArray[i] = (char *)malloc(80 * sizeof(char));
        for (int j = 0; j < 80; j++)
        {
            mapArray[i][j] = map[i][j];
        }
    }

    return mapArray;
}

void MapToMap()
{

    char **mapArray[401][401];

    // Initialize each element to NULL
    for (int i = 0; i < 401; i++)
    {
        for (int j = 0; j < 401; j++)
        {
            if (i == 200 && j == 200)
                mapArray[i][j] = printmap(' ', -1); // 0, 0 in the map
            else
                mapArray[i][j] = NULL;
        }
    }

    // Stores co-ordingates of the starting map we are it using custom struct which we have defined
    CurrMap currentMap;
    currentMap.x = 200;
    currentMap.y = 200;
    char userInput;

    do
    {
        printf("Enter a character (or 'q' to quit): ");
        scanf(" %c", &userInput); // Note the space before %c to skip whitespace

        // Process the user input
        if (userInput == 'n')
        {
            char **map = mapArray[currentMap.x][currentMap.y];
            // Go through the first row and find which row index was # assigned on
            int gateIndex = -1;
            for (int j = 0; j < 80; j++)
            {
                if (map[0][j] == '#')
                {
                    gateIndex = j;
                    break;
                }
            }

            updateX(&currentMap, currentMap.x - 1);
            int x = currentMap.x;
            int y = currentMap.y;
            // gives the coordintes we want to store the pointer of
            if (mapArray[x][y] == NULL)
            {
                mapArray[x][y] = printmap('s', gateIndex); // need to modify this to take in specific gate posiions to generate
            }
            else
            {

                char **centralMap = mapArray[x][y];

                for (int i = 0; i < 21; i++)
                {
                    for (int j = 0; j < 80; j++)
                    {
                        if (centralMap[i][j] == '^')
                        {

                            printColoredChar('.');
                        }
                        else if (centralMap[i][j] == '.')
                        {

                            printColoredChar('^');
                        }
                        else
                        {
                            printColoredChar(centralMap[i][j]);
                        }
                    }
                    printf("\n");
                }
            }
        }
        else if (userInput == 'q')
        {
            printf("Quitting.....");
            // Add your processing code here
        }else if(userInput == 's'){

            char **map = mapArray[currentMap.x][currentMap.y];
            // Go through the first row and find which row index was # assigned on
            int gateIndex = -1;
            for (int j = 0; j < 80; j++)
            {
                if (map[79][j] == '#')
                {
                    gateIndex = j;
                    break;
                }
            }

            // stores last row's gate index to pass on

            updateX(&currentMap, currentMap.x + 1);
            int x = currentMap.x;
            int y = currentMap.y;

            if (mapArray[x][y] == NULL)
            {
                mapArray[x][y] = printmap('n', gateIndex); // need to modify this to take in specific gate posiions to generate
            }
            else {

                char **centralMap = mapArray[x][y];

                for (int i = 0; i < 21; i++)
                {
                    for (int j = 0; j < 80; j++)
                    {
                        if (centralMap[i][j] == '^')
                        {

                            printColoredChar('.');
                        }
                        else if (centralMap[i][j] == '.')
                        {

                            printColoredChar('^');
                        }
                        else
                        {
                            printColoredChar(centralMap[i][j]);
                        }
                    }
                    printf("\n");
                }
            }
        }
        else{
            printf("Please generate a valid input");
        }
    } while (userInput != 'q');

    printf("Exiting the program.\n");
}

int main(int argc, char *argv[])
{

    printLegend();

    MapToMap();

    // printmap();

    return 0;
}
