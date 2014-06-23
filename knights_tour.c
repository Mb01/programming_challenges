#include <stdlib.h>
#include <stdio.h>

/* use backtracking to find first solution */
int _solve(int x, int y, int** board, int n){
    /* all possible components of a legal knight move */
    const int moves[4] = {-2,-1,1,2};
    /* base case: check if solution */
    if(board[x][y] == n * n){
        return 1;
    }
    /* try all move combinations */
    int i,j;
    for(i = 0; i < 4; i++){
        for(j = 0; j < 4; j++){
            /* check for legal knight move */
            if(abs(moves[i]) == abs(moves[j])){
                continue;
            }
            /* set position to try */
            int newx = x + moves[i];
            int newy = y + moves[j];
            /* check if move is on the board or into a already used space */
            if(newx < 0 || newx >= n || newy < 0 ||  newy >= n || board[newx][newy] != 0){
                continue;
            }
            /* set the co-ordinates of the new position to current move + 1 */
            board[newx][newy] = board[x][y] + 1;
            /* recurse and propogate solution if found */
            if( _solve(newx, newy, board, n) ){
                return 1;
            }
            /* undo change to board */
            board[newx][newy] = 0;
        }
    }
    /* all paths exhausted and nothing found */
    return 0;
}

/* wrapper for solving function */
int **solve(int n){
    /* allocate a board */
    int i;
    int ** board = malloc(sizeof(int*) * n);
    for(i = 0; i < n; i++){
        board[i] = malloc(sizeof(int) * n);
    }
    /* start at position 0,0 */
    int x = 0;
    int y = 0;
    /* start move index as 1 (either this or change _solve's base case) */
    board[x][y] = 1;
    _solve(x,y, board, n);
    return board;
}

/* simple printer funtion for board aka an array */
void prar(int ** ar, int n){
    int i,j;
    for(i = 0; i < n; i++){
        for(j = 0; j < n; j++){
            if(j != 0) putchar(' ');
            if(ar[i][j] < 10) putchar(' ');
            printf("%d", ar[i][j]);
            if(j == n - 1) putchar('\n');
        }
    }
}

/* driver */
int main(int argc, char** argv){
    int n = 8;
    if(argc >= 2){
        n = atoi(argv[1]);
    }
    int ** solution = solve(n);
    prar(solution, n);
}
