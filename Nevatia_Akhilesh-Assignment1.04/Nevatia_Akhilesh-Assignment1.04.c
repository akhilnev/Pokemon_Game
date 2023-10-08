#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>
#include <signal.h>
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



typedef struct {
    int x;  // x-coordinate of the PC
    int y;  // y-coordinate of the PC
} PC;


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

//typedef struct CurrMap CurrMap;
typedef struct CurrMap
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
        printf("Cannot move further up\n");
    }
    else if (newX > 400)
    {
        currMap->x = 400;
        printf("Cannot move further down\n");
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
        printf("Cannot move further west\n");
    }
    else if (newY > 400)
    {
        currMap->y = 400;
        printf("Cannot move further east\n");
    }
    else
    {
        currMap->y = newY;
    }
}

// CREATING PQ TO PLAY CONTINOUS GAME 1.4 

typedef struct {
    int x;
    int y;
    int weight;
    char c;
} PQElement;

typedef struct {
    PQElement *arr;
    int capacity;
    int size;
} MyPriorityQueue;

MyPriorityQueue* createMyPriorityQueue(int capacity) {
    MyPriorityQueue* pq = (MyPriorityQueue*)malloc(sizeof(MyPriorityQueue));
    pq->arr = (PQElement*)malloc(sizeof(PQElement) * (capacity + 1));
    pq->capacity = capacity;
    pq->size = 0;
    return pq;
}

void mySwap(PQElement* a, PQElement* b) {
    PQElement temp = *a;
    *a = *b;
    *b = temp;
}

void minMyHeapify(MyPriorityQueue* pq, int idx) {
    int smallest = idx;
    int left_child = 2 * idx;
    int right_child = 2 * idx + 1;

    if (left_child <= pq->size && pq->arr[left_child].weight < pq->arr[smallest].weight)
        smallest = left_child;

    if (right_child <= pq->size && pq->arr[right_child].weight < pq->arr[smallest].weight)
        smallest = right_child;

    if (smallest != idx) {
        mySwap(&pq->arr[idx], &pq->arr[smallest]);
        minMyHeapify(pq, smallest);
    }
}

void myEnqueue(MyPriorityQueue* pq, int x, int y, int weight, char c) {
    if (pq->size >= pq->capacity) {
        printf("Priority queue is full.\n");
        return;
    }

    pq->size++;
    pq->arr[pq->size].x = x;
    pq->arr[pq->size].y = y;
    pq->arr[pq->size].weight = weight;
    pq->arr[pq->size].c = c;

    int current = pq->size;
    while (current > 1 && pq->arr[current].weight < pq->arr[current / 2].weight) {
        mySwap(&pq->arr[current], &pq->arr[current / 2]);
        current = current / 2;
    }
}

PQElement myDequeue(MyPriorityQueue* pq) {
    if (pq->size == 0) {
        printf("Priority queue is empty.\n");
        PQElement empty;
        return empty;
    }

    PQElement min_element = pq->arr[1];
    pq->arr[1] = pq->arr[pq->size];
    pq->size--;

    minMyHeapify(pq, 1);

    return min_element;
}

void displayMyQueue(MyPriorityQueue* pq) {
    printf("Priority Queue elements (x, y, weight, c):\n");
    for (int i = 1; i <= pq->size; ++i)
        printf("(%d, %d, %d, %c) ", pq->arr[i].x, pq->arr[i].y, pq->arr[i].weight, pq->arr[i].c);
    printf("\n");
}


// CREATING PRIORITY QUEUE TO IMPLEMENT DIJKSTRA 

typedef struct {
    int row;
    int column;
    int weight;
} Element;

typedef struct {
    Element *arr;
    int capacity;
    int size;
} PriorityQueue;

PriorityQueue *createPriorityQueue(int capacity) {
    PriorityQueue *pq = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    pq->arr = (Element *)malloc(sizeof(Element) * (capacity + 1));  // 1-indexed array
    pq->capacity = capacity;
    pq->size = 0;
    return pq;
}

void swap(Element *a, Element *b) {
    Element temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(PriorityQueue *pq, int idx) {
    int smallest = idx;
    int left = 2 * idx;
    int right = 2 * idx + 1;

    if (left <= pq->size && pq->arr[left].weight < pq->arr[smallest].weight)
        smallest = left;

    if (right <= pq->size && pq->arr[right].weight < pq->arr[smallest].weight)
        smallest = right;

    if (smallest != idx) {
        swap(&pq->arr[idx], &pq->arr[smallest]);
        minHeapify(pq, smallest);
    }
}

void insert(PriorityQueue *pq, int row, int column, int weight){
    if (pq->size >= pq->capacity) {
        printf("Priority queue is full.\n");
        return;
    }

    pq->size++;
    int i = pq->size;
    pq->arr[i].row = row;
    pq->arr[i].column = column;
    pq->arr[i].weight = weight;

    while (i > 1 && pq->arr[i].weight < pq->arr[i / 2].weight) {
        swap(&pq->arr[i], &pq->arr[i / 2]);
        i /= 2;
    }
}

Element extractMin(PriorityQueue *pq) {
    if (pq->size == 0) {
        printf("Priority queue is empty.\n");
        Element nullElement = { -1, -1, -1 };
        return nullElement;
    }

    Element minElement = pq->arr[1];
    pq->arr[1] = pq->arr[pq->size];
    pq->size--;
    minHeapify(pq, 1);

    return minElement;
}

int isEmp(PriorityQueue *pq) {
    return pq->size == 0;
}

void freePriorityQueue(PriorityQueue *pq) {
    free(pq->arr);
    free(pq);
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

// FUNCTION TO GET WEIGHT OF EACH SYMBOL BASED ON HIKER 
int getHikerWeight(char symbol, int row, int column) { 
    
    int weight = -1;

    // Determine weight based on the symbol
    if (symbol == '%' || symbol == '.' || symbol == '~') {
        weight = INT_MAX;  // Represents infinity
    } else if (symbol == '#') {
        if(row==0 || row == 20 || column ==0 || column == 79) weight = INT_MAX;
        else weight = 10;
    } else if (symbol == 'M' || symbol == 'C') {
        weight = 50;
    } else if (symbol == ':') {
        weight = 15;
    } else if (symbol == '^') {
        weight = 10;
    } else {
        // Handle any other symbols not listed (error condition)
        weight = INT_MAX;
    }

    return weight;
}

int getRivalWeight(char symbol, int row, int column){
    int weight = -1;

    // Determine weight based on the symbol
    if (symbol == '%' || symbol == '.' || symbol == '~') {
        weight = INT_MAX;  // Represents infinity
    } else if (symbol == '#') {
        if(row==0 || row == 20 || column ==0 || column == 79) weight = INT_MAX;
        else weight = 10;
    } else if (symbol == 'M' || symbol == 'C') {
        weight = 50;
    } else if (symbol == ':') {
        weight = 20;
    } else if (symbol == '^') {
        weight = 10;
    } else {
        // Handle any other symbols not listed (error condition)
        weight = INT_MAX;
    }

    return weight;

}

// SWIMMER WEIGHT FUNCTION
int getSwimmerWeight(char symbol, int row, int column) { 
    
    int weight = -1;

    // Determine weight based on the symbol
    if (symbol == '%' || symbol == '.' ) {
        weight = INT_MAX;  // Represents infinity
    } else if(symbol == '~'){
        weight = 7;  
     }else if (symbol == '#') {
         weight = INT_MAX;
    } else if (symbol == 'M' || symbol == 'C') {
        weight = INT_MAX;
    } else if (symbol == ':') {
        weight = INT_MAX;
    } else if (symbol == '^') {
        weight = INT_MAX;
    } else {
        // Handle any other symbols not listed (error condition)
        weight = INT_MAX;
    }

    return weight;
}

// OTHER CHARACTERS WEIGHT FUNCTION

int getOtherWeight(char symbol, int row, int column) { 
    
    int weight = -1;

    // Determine weight based on the symbol
    if (symbol == '%' || symbol == '.' ) {
        weight = INT_MAX;  // Represents infinity
    } else if(symbol == '~'){
        weight = INT_MAX;  
     }else if (symbol == '#') {
        if(row==0 || row == 20 || column ==0 || column == 79) weight = INT_MAX;
        else weight = 10;
    } else if (symbol == 'M' || symbol == 'C') {
        weight = 50;
    } else if (symbol == ':') {
        weight = 20;
    } else if (symbol == '^') {
        weight = 10;
    } else {
        // Handle any other symbols not listed (error condition)
        weight = INT_MAX;
    }

    return weight;
}



// USED TO GET CONSTANT RUNNING GAME WITH CONTROL C TO EXIT
// Define a flag to indicate if the game should continue running
int game_running = 1;

// Signal handler for SIGINT (CTRL+C)
void sigint_handler(int signal) {
    printf("\nReceived SIGINT. Exiting...\n");
    game_running = 0;  // Set the flag to exit the game
}


int *printRmap(char map[21][80] ,int row , int col , int aroundr , int aroundc){

    printf("RIVAL MAP \n");
    // printing the hikermap using dijkstra's algorithm
    int rival[21][80];
    PriorityQueue *pq = createPriorityQueue(20000); // MAX POSSIBLE STORAGE 
    
    
    int visited[21][80];
    // initializing visited array to 0
    for(int i = 0 ; i < 21 ; i++){
        for(int j = 0 ; j < 80 ; j++){
            visited[i][j] = 0;
        }
    }

      for(int i = 0 ; i < 21 ; i++){
        for(int j = 0 ; j < 80 ; j++){
            rival[i][j] = INT_MAX; // EVERYTHING AT INFINITE DISTANCE INITIALLY 
        }
    }

    insert(pq,row,col,0);

    rival[row][col] = 0; // distance of the source from itself is 0

    //To implement an 8 - directional search
    int aroundx[] = {0, 1, 0, -1, 1, -1, 1, -1};
    int aroundy[] = {1, 0, -1, 0, -1, 1, 1, -1};

    // I AM HERE 
    while(!isEmp(pq)){
        Element minElement = extractMin(pq);
        int x = minElement.row;
        int y = minElement.column;
        int weight = minElement.weight;
        if(visited[x][y] == 1 || x < 0 || y > 79 || x > 20 || y < 0) continue;
        
        visited[x][y] = 1;
        for(int i = 0 ; i < 8 ; i++){

            int newWeight = (weight + getRivalWeight(map[x+aroundx[i]][y+aroundy[i]], x + aroundx[i], y + aroundy[i])) % 100;
            
            if(visited[x+aroundx[i]][y+aroundy[i]]== 0 && rival[x+aroundx[i]][y+aroundy[i]] > newWeight) {
                //hiker[x+aroundx[i]][y+aroundy[i]] =((weight + getHikerWeight(map[x+aroundx[i]][y+aroundy[i]] ,x + aroundx[i], y + aroundy[i]))%100);
                //printf("%c",map[x+aroundx[i]][y+aroundy[i]]);
                rival[x+aroundx[i]][y+aroundy[i]] = (newWeight < 0) ? INT_MAX : newWeight;
                insert(pq,x+aroundx[i],y+aroundy[i],rival[x+aroundx[i]][y+aroundy[i]]);
            }
            
        }
    }

    // print the hiker array as a 2d array 

    for(int i = 0 ; i < 21 ; i++){
        for(int j = 0 ; j < 80 ; j++){
            if(rival[i][j] == INT_MAX) printf("   ");
            else if(rival[i][j]==0) printf("00 ");
            else printf("%2d ",rival[i][j]);
        }
        printf("\n");
    }

    int maxD = -1;
    int val[2] = {1,1};
    for(int i =0 ; i< 8 ; i++){
        if(rival[aroundr+aroundx[i]][aroundc+aroundy[i]]!= INT_MAX && rival[aroundr+aroundx[i]][aroundc+aroundy[i]] > maxD){
            maxD = rival[aroundr+aroundx[i]][aroundc+aroundy[i]];
            val[0] = aroundr+aroundx[i];
            val[1] = aroundc+aroundy[i];
        }
    }
    

    return val; // return value to which we need to go as array pointer 

}


int *printHmap(char map[21][80] ,int row , int col , int aroundr , int aroundc){


    printf("HIKER MAP \n");
    // printing the hikermap using dijkstra's algorithm
    int hiker[21][80];
    PriorityQueue *pq = createPriorityQueue(20000); // MAX POSSIBLE STORAGE 
    
    
    int visited[21][80];
    // initializing visited array to 0
    for(int i = 0 ; i < 21 ; i++){
        for(int j = 0 ; j < 80 ; j++){
            visited[i][j] = 0;
        }
    }

      for(int i = 0 ; i < 21 ; i++){
        for(int j = 0 ; j < 80 ; j++){
            hiker[i][j] = INT_MAX; // EVERYTHING AT INFINITE DISTANCE INITIALLY 
        }
    }

    insert(pq,row,col,0);

    hiker[row][col] = 0; // distance of the source from itself is 0

    //To implement an 8 - directional search
    int aroundx[] = {0, 1, 0, -1, 1, -1, 1, -1};
    int aroundy[] = {1, 0, -1, 0, -1, 1, 1, -1};

    // I AM HERE 
    while(!isEmp(pq)){
        Element minElement = extractMin(pq);
        int x = minElement.row;
        int y = minElement.column;
        int weight = minElement.weight;
        if(visited[x][y] == 1 || x < 0 || y > 79 || x > 20 || y < 0) continue;
        
        visited[x][y] = 1;
        for(int i = 0 ; i < 8 ; i++){

            int newWeight = (weight + getHikerWeight(map[x+aroundx[i]][y+aroundy[i]], x + aroundx[i], y + aroundy[i])) % 100;
            
            if(visited[x+aroundx[i]][y+aroundy[i]]== 0 && hiker[x+aroundx[i]][y+aroundy[i]] > newWeight) {
                //hiker[x+aroundx[i]][y+aroundy[i]] =((weight + getHikerWeight(map[x+aroundx[i]][y+aroundy[i]] ,x + aroundx[i], y + aroundy[i]))%100);
                //printf("%c",map[x+aroundx[i]][y+aroundy[i]]);
                hiker[x+aroundx[i]][y+aroundy[i]] = (newWeight < 0) ? INT_MAX : newWeight;
                insert(pq,x+aroundx[i],y+aroundy[i],hiker[x+aroundx[i]][y+aroundy[i]]);
            }
            
        }
    }

    // print the hiker array as a 2d array 

    for(int i = 0 ; i < 21 ; i++){
        for(int j = 0 ; j < 80 ; j++){
            if(hiker[i][j] == INT_MAX) printf("   ");
            else if(hiker[i][j]==0) printf("00 ");
            else printf("%2d ",hiker[i][j]);
        }
        printf("\n");
    }

    

    printf("\n");
    printf("\n");


    int maxD = -1;
    int val[2] = {1,1};
    for(int i =0 ; i< 8 ; i++){
        if(hiker[aroundr+aroundx[i]][aroundc+aroundy[i]]!= INT_MAX && hiker[aroundr+aroundx[i]][aroundc+aroundy[i]] > maxD){
            maxD = hiker[aroundr+aroundx[i]][aroundc+aroundy[i]];
            val[0] = aroundr+aroundx[i];
            val[1] = aroundc+aroundy[i];
        }
    }
    

    return val; // return value to which we need to go as array pointer 




 }

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


char **printmap(char gate, int index , int mapx , int mapy )
{
    PC playerCharacter;
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
        //PLACING THE PC 
        int count = rand()%10;
        int var = 0;
        while (horzy != 79)
        {
            int dirn = (rand() % 2); // as we only want to go ahead in terms of cols
            if ((map[horzx + dx[dirn]][horzy + dy[dirn]]) != '%')
            {
                var++;
                horzx += dx[dirn];
                horzy += dy[dirn];
                map[horzx][horzy] = '#';
                if(var == count){
                    map[horzx][horzy] = '@'; // PLACING PLAYER CHARACTER RANDOMLY ON THE ROAD SIDE 
                    // Declare a PC object
                    playerCharacter.x = horzx;  // Set x-coordinate
                    playerCharacter.y = horzy;  // Set y-coordinate
                }
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


    int d = sqrt(pow(200 - mapx, 2) + pow(200 - mapy, 2));
    
    int prob = (((-45*d)/200) + 50);

    int value = rand()%101; // between 1 to 100 ( included)

    int aroundx[] = {0, 1, 0, -1, 1, -1, 1, -1};
    int aroundy[] = {1, 0, -1, 0, -1, 1, 1, -1};

    if(value <= prob){

    int Py = rand() % 70 + 6;
    int Px = rand() % 10 + 5;


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



    // MAKING THE WORLD EDGES HAVE APPROPRIATE BOUNDRIES AND HAVE NO GATES
        if (mapx == 0)
        {
            for (int j = 0; j < 80; j++)
            {
                if (map[0][j] == '#')
                {
                    map[0][j] = '%';
                }
            }
        }
        else if (mapx == 400)
        {

            for (int j = 0; j < 80; j++)
            {
                if (map[20][j] == '#')
                {
                    map[20][j] = '%';
                }
            }
        }

        if (mapy == 0)
        {
            for (int i = 0; i < 21; i++)
            {
                if (map[i][0] == '#')
                {
                    map[i][0] = '%';
                }
            }
        }
        else if (mapy== 400)
        {
            for (int i = 0; i < 21; i++)
            {
                if (map[i][79] == '#')
                {
                    map[i][79] = '%';
                }
            }
        }


    // ADDING NPC'S AND THEIR MOVEMENT TOO 

     int numtrainers = 10;  // Default number of trainers

    // Prompt the user for the number of trainers
    printf("Enter the number of trainers (--numtrainers), or press Enter for default (10): ");
    
    // Read the number of trainers from the user
    int input_result = scanf("%d", &numtrainers);

    // If the user entered a number, use that number

    //Now using the num trainers to add multiple trainers accordingly 

    // Add all trainers to a PQ with a cost of 0 initially and then keep increasing based on current time + terraincost 

    int totalTrainers = 0;
    int count = 0;

    MyPriorityQueue *pq = createMyPriorityQueue(20);


    while(totalTrainers<numtrainers){
        count =count %6;
        // where to add the specific NPC
        if(count==0){

            int i = 3+ rand()%17;
            int j = 3+ rand()%76;
            while(!(map[i][j]!='h' && map[i][j]!='r' && map[i][j]!='p' && map[i][j]!='w' && map[i][j]!='s' && map[i][j]!='e' && map[i][j]!='#' && getHikerWeight(map[i][j],i,j)!= INT_MAX)){
                i = 3+ rand()%17;
                j = 3+ rand()%76;
            }
            myEnqueue(pq,i,j,0,map[i][j]); // inserts where the character is and we have initialized the character to be at 0 distance from itself
            map[i][j] = 'h';
            
        }
        else if(count == 1){
            int i = 3+ rand()%17;
            int j = 3+ rand()%76;
            while(!(map[i][j]!='h' && map[i][j]!='r' && map[i][j]!='p' && map[i][j]!='w' && map[i][j]!='s' && map[i][j]!='e' && map[i][j]!='#' && getRivalWeight(map[i][j],i,j)!= INT_MAX)){
                i = 3+ rand()%17;
                j = 3+ rand()%76;
            }
            myEnqueue(pq,i,j,0,map[i][j]); // inserts where the character is and we have initialized the character to be at 0 distance from itself
            map[i][j] = 'r';
        }
        else if(count ==2){
            // need to make cost method for each of these NPC types
            int i = 3+ rand()%17;
            int j = 3+ rand()%76;
            while(!(map[i][j]!='h' && map[i][j]!='r' && map[i][j]!='p' && map[i][j]!='w' && map[i][j]!='s' && map[i][j]!='e' && map[i][j]!='#' && getOtherWeight(map[i][j],i,j)!= INT_MAX)){
                i = 3+ rand()%17;
                j = 3+ rand()%76;
            }
            myEnqueue(pq,i,j,0,map[i][j]); // inserts where the character is and we have initialized the character to be at 0 distance from itself
            map[i][j] = 'p';



        }else if(count ==3){

                // need to make cost method for each of these NPC types
            int i = 3+ rand()%17;
            int j = 3+ rand()%76;
            while(!(map[i][j]!='h' && map[i][j]!='r' && map[i][j]!='p' && map[i][j]!='w' && map[i][j]!='s' && map[i][j]!='e' && map[i][j]!='#' && getOtherWeight(map[i][j],i,j)!= INT_MAX)){
                i = 3+ rand()%17;
                j = 3+ rand()%76;
            }
            myEnqueue(pq,i,j,0,map[i][j]); // inserts where the character is and we have initialized the character to be at 0 distance from itself
            map[i][j] = 'w';

        }else if(count ==4){

                // need to make cost method for each of these NPC types
            int i = 3+ rand()%17;
            int j = 3+ rand()%76;
            while(!(map[i][j]!='h' && map[i][j]!='r' && map[i][j]!='p' && map[i][j]!='w' && map[i][j]!='s' && map[i][j]!='e' && map[i][j]!='#' && getOtherWeight(map[i][j],i,j)!= INT_MAX)){
                i = 3+ rand()%17;
                j = 3+ rand()%76;
            }
            myEnqueue(pq,i,j,0,map[i][j]); // inserts where the character is and we have initialized the character to be at 0 distance from itself

            map[i][j] = 's';

        }else if(count ==5 ){

                // need to make cost method for each of these NPC types
            int i = 3+ rand()%17;
            int j = 3+ rand()%76;
            while(!(map[i][j]!='h' && map[i][j]!='r' && map[i][j]!='p' && map[i][j]!='w' && map[i][j]!='s' && map[i][j]!='e' && map[i][j]!='#' && (getOtherWeight(map[i][j],i,j)!= INT_MAX))){
                i = 3+ rand()%17;
                j = 3+ rand()%76;
            }
            myEnqueue(pq,i,j,0,map[i][j]); // inserts where the character is and we have initialized the character to be at 0 distance from itself
            map[i][j] = 'e';

        }



        totalTrainers++;
        count++;
    }

    // Now we have our priority queue with all initial weights organized

    // Register the signal handler for SIGINT (CTRL+C)
    signal(SIGINT, sigint_handler);
    int currentTime = 1;
    int pflag = 0;
    int wanderx[] = {-1,0,1,0};
    int wandery[] = {0,1,0,-1};
    int choosewander = rand()%4;
    int wx = wanderx[choosewander];
    int wy = wandery[choosewander];
    int expx[] = {-1,0,1,0};
    int expy[] = {0,1,0,-1};
    int chooseexp = rand()%4;
    int ex = expx[chooseexp];
    int ey = expy[chooseexp];

    // Game loop
    while (game_running) {
        // Process the events occuring in the prirority queue
        

        while (!isEmp(pq)) {
            PQElement minElement = myDequeue(pq);
            int x = minElement.x;
            int y = minElement.y;
            char oldc = minElement.c;
            int weight = minElement.weight;
            if(map[x][y]=='h'){
                //gradient descent wrt hiker map
       
            int *val = printHmap(map,playerCharacter.x,playerCharacter.y,x,y);
            int newx = val[0];
            val++;
            int newy = val[0];
            map[x][y] = oldc;
            weight = currentTime + getHikerWeight(map[newx][newy],newx,newy);
            myEnqueue(pq,newx,newy,weight,map[newx][newy]);
            map[newx][newy] = 'h';

            }else if(map[x][y]=='r'){

                 //gradient descent wrt hiker map
       
            int *val = printHmap(map,playerCharacter.x,playerCharacter.y,x,y);
            int newx = val[0];
            val++;
            int newy = val[0];
            map[x][y] = oldc;
            weight = currentTime + getRivalWeight(map[newx][newy],newx,newy);
            myEnqueue(pq,newx,newy,weight,map[newx][newy]);
            map[newx][newy] = 'r';

        }else if(map[x][y]=='p'){
            
            if(pflag==0){
                if(getOtherWeight(map[x][y+1],x,y+1)!=INT_MAX){
                    map[x][y] = oldc;
                    weight = currentTime + getOtherWeight(map[x][y+1],x,y+1);
                    myEnqueue(pq,x,y+1,weight,map[x][y+1]);
                    map[x][y+1] = 'p';
                }else{
                    pflag = 1;
                    map[x][y] = oldc;
                    weight = currentTime + getOtherWeight(map[x][y-1],x,y-1);
                    myEnqueue(pq,x,y-1,weight,map[x][y-1]);
                    map[x][y-1] = 'p';
                }
            }else{
                if(getOtherWeight(map[x][y-1],x,y-1)!=INT_MAX){
                    map[x][y] = oldc;
                    weight = currentTime + getOtherWeight(map[x][y-1],x,y-1);
                    myEnqueue(pq,x,y-1,weight,map[x][y-1]);
                    map[x][y-1] = 'p';
                }else{
                    pflag = 0;
                    map[x][y] = oldc;
                    weight = currentTime + getOtherWeight(map[x][y+1],x,y+1);
                    myEnqueue(pq,x,y+1,weight,map[x][y+1]);
                    map[x][y+1] = 'p';
                }

            }
        }else if(map[x][y]=='w'){

            map[x][y] = oldc;
            if(map[x+wx][y+wy]==oldc){
                weight = currentTime + getOtherWeight(map[x+wx][y+wy],x+wx,y+wy);
                myEnqueue(pq,x+wx,y+wy,weight,map[x+wx][y+wy]);
                map[x+wx][y+wy] = 'w';
            }else{
                while(map[x+wx][y+wy]!=oldc){
                    choosewander = rand()%4;
                    wx = wanderx[choosewander];
                    wy = wandery[choosewander];
                }
                weight = currentTime + getOtherWeight(map[x+wx][y+wy],x+wx,y+wy);
                myEnqueue(pq,x+wx,y+wy,weight,map[x+wx][y+wy]);
                map[x+wx][y+wy] = 'w';

            }

        }else if(map[x][y]=='s'){
            // Wait for action to come to them and stay stationary
            // obviously we change the weight based on current time being updated 
            weight = currentTime + getOtherWeight(map[x][y],x,y);
            myEnqueue(pq,x,y,weight,map[x][y]);
        }else if(map[x][y]=='e'){

            map[x][y] = oldc;
            if(getOtherWeight(map[x+ex][y+ey],x+ex,y+ey)!=INT_MAX){
                weight = currentTime + getOtherWeight(map[x+ex][y+ey],x+ex,y+ey);
                myEnqueue(pq,x+ex,y+ey,weight,map[x+ex][y+ey]);
                map[x+ex][y+ey] = 'e';
            }else{
                while(getOtherWeight(map[x+ex][y+ey],x+ex,y+ey)==INT_MAX){
                    chooseexp = rand()%4;
                    ex = expx[chooseexp];
                    ey = expy[chooseexp];
                }
                weight = currentTime + getOtherWeight(map[x+ex][y+ey],x+ex,y+ey);
                myEnqueue(pq,x+ex,y+ey,weight,map[x+ex][y+ey]);
                map[x+ex][y+ey] = 'e';

            }

        }
        currentTime++;   

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

        // Pause to allow observation of updates
        usleep(250000);  // Sleep for 250 milliseconds (4 frames per second)


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

    
    //printHmap(map, playerCharacter.x, playerCharacter.y);

    //printRmap(map, playerCharacter.x, playerCharacter.y);
    

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
















// MOVING AROUND TO DIFFERENT MAPS FUNCTION 


void MapToMap()
{

    char **mapArray[401][401];

    // Initialize each element to NULL
    for (int i = 0; i < 401; i++)
    {
        for (int j = 0; j < 401; j++)
        {
            if (i == 200 && j == 200)
                mapArray[i][j] = printmap(' ', -1,200,200); // 0, 0 in the map
            else
                mapArray[i][j] = NULL;
        }
    }

    printf("map x-coordinate : %d \n",0);
    printf("map y-coordinate : %d \n ",0);

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
                mapArray[x][y] = printmap('s', gateIndex , currentMap.x, currentMap.y); // need to modify this to take in specific gate posiions to generate
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

            printf("map x-coordinate : %d \n",currentMap.x - 200);
            printf("map y-coordinate : %d \n ",currentMap.y - 200);
        }
        else if (userInput == 's')
        {

            char **map = mapArray[currentMap.x][currentMap.y];
            // Go through the first row and find which row index was # assigned on
            int gateIndex = -1;
            for (int j = 0; j < 80; j++)
            {
                if (map[20][j] == '#')
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
                mapArray[x][y] = printmap('n', gateIndex,currentMap.x,currentMap.y); // need to modify this to take in specific gate posiions to generate
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

            printf("map x-coordinate : %d \n",currentMap.x - 200);
            printf("map y-coordinate : %d \n ",currentMap.y - 200);
        }
        else if (userInput == 'e')
        {

            char **map = mapArray[currentMap.x][currentMap.y];
            // Go through the first row and find which row index was # assigned on
            int gateIndex = -1;
            for (int i = 0; i < 21; i++)
            {
                if (map[i][79] == '#')
                {
                    gateIndex = i;
                    break;
                }
            }

            // find appropriate row at which gate exists

            updateY(&currentMap, currentMap.y + 1);
            int x = currentMap.x;
            int y = currentMap.y;

            if (mapArray[x][y] == NULL)
            {
                mapArray[x][y] = printmap('w', gateIndex,currentMap.x,currentMap.y); // need to modify this to take in specific gate posiions to generate
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

           printf("map x-coordinate : %d \n",currentMap.x - 200);
            printf("map y-coordinate : %d \n ",currentMap.y - 200);
        }
        else if (userInput == 'w')
        {

            char **map = mapArray[currentMap.x][currentMap.y];
            // Go through the first row and find which row index was # assigned on
            int gateIndex = -1;
            for (int i = 0; i < 21; i++)
            {
                if (map[i][0] == '#')
                {
                    gateIndex = i;
                    break;
                }
            }

            updateY(&currentMap, currentMap.y - 1);
            int x = currentMap.x;
            int y = currentMap.y;

            if (mapArray[x][y] == NULL)
            {
                mapArray[x][y] = printmap('e', gateIndex,currentMap.x,currentMap.y); // need to modify this to take in specific gate posiions to generate
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

            printf("map x-coordinate : %d \n",currentMap.x - 200);
            printf("map y-coordinate : %d \n ",currentMap.y - 200);
        }
        else if (userInput == 'f')
        {
            int x;
            int y;
            printf("Enter x coordinate of map you want to go to ");
            scanf(" %d", &x); // Note the space before %c to skip whitespace
            printf("Enter y coordinate of map you want to go to ");
            scanf(" %d", &y); // Note the space before %c to skip whitespace
            // As our center internally is 200,200 and we want out position with respect to that
            x += 200;
            y += 200;
            updateX(&currentMap, x);
            updateY(&currentMap, y);
            x = currentMap.x;
            y = currentMap.y;

            if (mapArray[x][y] == NULL)
            {
                mapArray[x][y] = printmap(' ', -1,currentMap.x,currentMap.y); // need to modify this to take in specific gate posiions to generate
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

        printf("map x-coordinate : %d \n",currentMap.x - 200);
        printf("map y-coordinate : %d \n ",currentMap.y - 200);

        }
        else if (userInput == 'q')
        {
            printf("#########QUITTING############\n");
        }
        else
        {
            printf("Please generate a valid input");
        }

        // MAKING THE WORLD EDGES HAVE APPROPRIATE BOUNDRIES AND HAVE NO GATES
        if (currentMap.x == 0)
        {
            for (int j = 0; j < 80; j++)
            {
                if (mapArray[currentMap.x][currentMap.y][0][j] == '#')
                {
                    mapArray[currentMap.x][currentMap.y][0][j] = '%';
                }
            }
        }
        else if (currentMap.x == 400)
        {

            for (int j = 0; j < 80; j++)
            {
                if (mapArray[currentMap.x][currentMap.y][20][j] == '#')
                {
                    mapArray[currentMap.x][currentMap.y][20][j] = '%';
                }
            }
        }
        if (currentMap.y == 0)
        {
            for (int i = 0; i < 21; i++)
            {
                if (mapArray[currentMap.x][currentMap.y][i][0] == '#')
                {
                    mapArray[currentMap.x][currentMap.y][i][0] = '%';
                }
            }
        }
        else if (currentMap.y == 400)
        {
            for (int i = 0; i < 21; i++)
            {
                if (mapArray[currentMap.x][currentMap.y][i][79] == '#')
                {
                    mapArray[currentMap.x][currentMap.y][i][79] = '%';
                }
            }
        }

    } while (userInput != 'q');
}



int main(int argc, char *argv[])
{

    printLegend();

    MapToMap();


    return 0;
}
