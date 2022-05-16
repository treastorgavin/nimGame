#include "nim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//MEMORY ALLOCATION FUNCTIONS

/**
 * @brief mallocs an array of board_size integers and returns a pointer to that malloced array
 * if malloc fails print a message to the standard error stream and exit 
 * 
 * @param board_size size of an array of integers (number of indicies in the array)
 * @return int* pointer to malloced array
 */
int *new_board(int board_size){
    int *ptr = malloc(board_size * sizeof(int));

    if(ptr == NULL){
        fprintf(stderr,"Malloc failed\n");
        exit(-1);
    }

    return ptr;
}

/**
 * @brief This function should malloc an array of size max_hash struct nodes, and initialize the nodes by setting the values of each moves value to -1, move value to NULL, nimsum value to -1
 * call the hash2board function with board_size, start_board and array index and store that variable in the board value at the same index in the array
 * check if malloc fails
 * 
 * @param max_hash size of array to malloc struct computed from board_size and start_board
 * @param board_size size of board from argc-2
 * @param start_board size of start board from board_size, argv+2
 * @return struct node* mk_nim_hash
 */
struct node *mk_nim_hash(int max_hash, int board_size, int *start_board){

    struct node *hash_array = malloc(max_hash *sizeof(struct node));

    if(hash_array == NULL){
        fprintf(stderr,"Malloc failed\n");
        exit(-1);
    }

    for(int i = 0; i < max_hash; i++){
        hash_array[i].moves = -1;
        hash_array[i].move = NULL;
        hash_array[i].nimsum = -1;

        int *boardVal = hash2board(board_size,start_board,i); //includes malloc for board

        hash_array[i].board = boardVal;
    }

    return hash_array;
}

/**
 * @brief frees the board array
 * 
 * @param board pointer to any board
 */
void free_board(int *board){
    free(board);
}

/**
 * @brief Free each move array and each board array in each element in the nim_hash and finally free nim_hash array itself
 * 
 * @param max_hash 
 * @param nim_hash 
 */
void free_nim_hash(int max_hash, struct node *nim_hash){
    
    for (int i = 0; i < max_hash; i++){
        free(nim_hash[i].move);
        free(nim_hash[i].board);
    }
    
    free(nim_hash);
}

// GAME FUNCTIONS

/**
 * @brief Creates a new board (new_board) a board array and initialize it with the integer equivalents (atoi) of the string array argv
 * the number of values to be converted in argv is equal to board_size
 * 
 * @param board_size number of elements that need to be converted from strings to ints
 * @param argv array of strings
 * @return int* 
 */
int *board_from_argv(int board_size, char **argv){

    int *new = new_board(board_size);

    for(int i = 0; i < board_size; i++){
        new[i] = atoi(argv[i]);
    }

    return new;

}

/**
 * @brief Returns a pointer to a new_board whos values are initialized with exactly the same values as board
 * 
 * @param board_sizem size of the board
 * @param board the board to copy
 * @return int* of the copied board
 */
int *copy_board(int board_size, int *board){
    
    int *board_copy = new_board(board_size);

    for(int i = 0; i < board_size; i++){
        board_copy[i] = board[i];
    }

    return board_copy;
}

/**
 * @brief This function will return a value of 1 if the board contains exactly one match and 0 otherwize
 * 
 * @param board_size 
 * @param board 
 * @return int 1 or 0
 */
int game_over( int board_size, int *board ){
    //go through the board array and see how many matches are left
    int numI = 0; // number of 1s in board

    for(int i = 0; i < board_size; i++){
        numI += board[i];
    }

    if(numI == 1){
        return 1;
    }else{
        return 0;
    }
}


/**
 * @brief This function will recursively join the nodes of the graph beginning with the node at index hash in the nim_hash array. It should check wether the moves variable is -1; if not, it can exit (the moves for the node have already been computer). Otherwise it should compute the total number of possible moves that can be made for the given board, and allocate a move array within the node to store those moves. It should initialize each possible move with the appropriate row and matches values. Then it should compute the outcome of applying that move and store the hash value of the resulting move in the moves hash variable. Finally, it should call itself recursivly on each destination node.
 * 
 * @param nim_hash array of nodes
 * @param hash (index in hash array)
 * @param board_size 
 * @param start_board 
 */
void join_graph(struct node *nim_hash, int hash, int board_size, int *start_board){
    
    //check to see if we can make a move
    if(nim_hash[hash].moves != -1){
        return;
    }

    // copy the board

    //the number of moves depends on the size of the board
    int numMoves = 0;

    int *curboard = hash2board(board_size, start_board, hash);

    for(int i = 0; i < board_size; i++){
        numMoves += curboard[i];
    }

    struct move *move_info = malloc(sizeof(struct move)*numMoves);

    int moveidx = 0;

    for(int i = 0; i < board_size; i++){
        for(int j = 0; j < curboard[i]; j++){

            move_info[moveidx].row = i;
            move_info[moveidx].matches = j+1;
            
            int *next_board = copy_board(board_size, curboard);
            
            next_board[i] -= j+1;

            int next_idx = board2hash(board_size,start_board,next_board);
            
            move_info[moveidx].hash = next_idx;

            join_graph(nim_hash,next_idx,board_size,start_board);

            free_board(next_board);

            moveidx++;
        }
    }

    nim_hash[hash].move = move_info;
    nim_hash[hash].moves = numMoves;
    nim_hash[hash].board = curboard;

    nim_hash[hash].nimsum = compute_nimsum(board_size, curboard);

}

//Last 20%
/**
 * @brief This function should compute the nimsum for a given board. The nimsum is defined as the bitwise exclusive OR of all the integer values in board, unless there are no values in board that are greater than 1. in the latter case, the nimsum is the negation of the regular nimsum
 * 
 * Add a call to the function computre_nimsum to join_graph to assign the nimsum value of each node visited
 * 
 * @param board_sizem 
 * @param board 
 * @return int 
 */
int compute_nimsum(int board_size, int *board){
    
    int nimsum = board[0];
    int check = 0;

    //checking if there is a value > 1
    for(int i = 0; i < board_size; i++){
        if(board[i] > 1){
            check = 1;
        }
    }

    //calculating nimsum

    //first case there are values greater than 1
    for(int i = 1; i < board_size; i++){
        nimsum = nimsum ^ board[i];
    }
    if(check != 1){
        // negation of regular nimsum
        // taking 2's compliment = 1's compliment (~) + 1
        nimsum = ~(nimsum);
    }

    return nimsum;
}
