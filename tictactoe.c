/* author: Mark Bowden 2014 */
#include <stdlib.h>
#include <stdio.h>

typedef struct Node Node;
struct Node{
    int player; /* here x = 1 or o = -1, to move */
    int **board; /* a 3x3 matrix */
};

/* win for: x is 1, o is -1, undecided is 0 ( ! and draw is 2 ! )*/
int check_win(int **board);
/* changes the player of a struct Node */
void toggle_player(Node* node);
/* initializes and returns an array with a start of game state */
int** make_empty_board();
/* print a 2d array */
void prar2d(int** arr, int nrows, int ncols);
/* uses input from stdin to create a position */
void make_node_from_input(Node* node);
/* determines whether a position has a forced win */
int search(Node* node);

/* board is like this */
/*  - x -  0 1 0      */
/*  x - -  1 0 0      */
/*  - o -  0 -1 0     */

/* returns whether there is a win at the current node and for who 1 -> x wins -1 o wins */
int search(Node* node){
    /* check base case */
    int color = check_win(node->board); /* color is the winning or losing or drawing -ness of a line */
    if(color){
        return color;/* this is reached by terminal nodes */
    }
    /* for each open position */
    int x, y;
    for(x = 0; x < 3; x++){
        for(y= 0; y < 3; y++){
            if(node->board[x][y] == 0){
                /* change board and state then pass to a lower node */
                node->board[x][y] = node->player;
                toggle_player(node);
                /* recurse */
                color = search(node);
                /* undo changes */
                node->board[x][y] = 0;
                toggle_player(node);
                /* pick a move that wins */
                if(color == node->player){
                    return color;
                }
            }
        }
    }
    return 2;/* It's a draw */
}

/* win for: x is 1, o is -1, undecided is 0 ( ! and draw is 2 ! )*/
int check_win(int **board){
    int x, y, rowsum, colsum[3];
    int open_or_draw = 2;
    /* if this remains throughout it's a draw */
    for(x = 0; x < 3; x++){
        rowsum = 0;
        for(y = 0; y < 3; y++){
            /* zero out colsum the first time only*/
            if(x == 0){
                colsum[y] = 0;
            }
            colsum[y] += board[x][y];
            rowsum += board[x][y];
            /* check colsum the last time through */
            if(x == 3){
                if(colsum[y] == 3){
                    return 1;
                }
                if(colsum[y] == -3){
                    return -1;
                }
            }
            /* if there is any open spaces, it's not a draw */
            if(board[x][y] == 0){
                open_or_draw = 0;
            }
        }/* END y LOOP*/
        /* check rowsum after each row */
        if(rowsum == 3){
            return 1;
        }
        if(rowsum == -3){
            return -1;
        }
    }/* END x LOOP */
    return open_or_draw;
}

void toggle_player(Node* node){
    node->player = node->player == 1 ? -1 : 1;
}

/* print a 2d square array */
void prar2d(int** arr, int nrows, int ncols){
    int x,y;
    for(x = 0; x < nrows; x++){
        int ok = 0;
        for(y = 0; y < ncols; y++){
            if(ok) printf(" ");
            ok = 1;
            printf("%d", arr[x][y]);
        }
        printf("\n");
    }
} 

int** make_empty_board(){
    int x, **board;
    board = malloc(sizeof(int*) * 3);
    for(x = 0; x < 3; x++){
        board[x] = calloc(3, sizeof(int));
    }
    return board;
}

void make_node_from_input(Node* node){
    printf("Player 1 (x) or 2 (o)?  to play? enter 1 or 2 ");
    int input;
    scanf("%d", &input);
    node->player = input;
    node->board = make_empty_board();
    int x,y;
    printf("Enter the 9 positions as 1 for x -1 for o and 0 for open. Put at least one space or newline before each position\n");
    for(x = 0; x < 3; x++){
        for(y = 0; y < 3; y++){
            scanf("%d", &input);
            node->board[x][y] = input;
        }
    }
}

int main(int argc, char** argv){
    Node* node = malloc(sizeof(Node));
    printf("TIC TAC TOE\n\n");
    printf("Enter position? Y/N y/n  ");
    char input[20];
    scanf("%s", input);
    if(input[0] == 'Y' || input[0] == 'y'){
        make_node_from_input(node);
    }
    else{
    node->player = 1;
    node->board = make_empty_board();
    }
    printf("1 is win for x, -1 is win for o, 2 is a draw:\n result: %d\n", search(node));
    return 0;
}
