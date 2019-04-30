/**************************************************************************
* Course     : CS4310
* Assignment : Project #2
*                 Task #1 - Input virtual address,
*                           Outputs: page number and offset.
*                 Task #2 - Disk Scheduling Algorithms:
*                           FCFS, SSTF, SCAN, C-SCAN
*                 Task #3 - Inputs: Sector address,
*                                   # of cylinders,
*                                   # of tracks,
*                                   # of sectors
*                           Outputs: the sector address in
*                                    <Cylinder#, Track#, Sector#> form.
* Name       : Johnson Wei
* ID         : 004150718
***************************************************************************/
#include <iostream>
#include <vector>
#include <queue>
#include <ctime>    // to seed srand method
#include <random>   // random number generator

using namespace std;
//Global Variable for Task #2
const int GENERATE_REQUESTS = 0; // represents random generation of requests
const int USER_INPUT_REQUESTS = 1; // indicates that user will input request
const int PRESENTATION_DEMO = 2; // for presentation purposes
const int CYLINDER_MAX = 5000; // Maximum  number of cylinders (but is most often used as (CYLINDER_MAX - 1)
const int NUM_OF_REQUESTS = 1000; // used with GENERATE_REQUESTS for number of random requests
/*******************************************************************************
 *                  Beginning of Function declarations                         *
 *         Descriptions of functions are listed during definition              *
 *******************************************************************************/
/********************[Function declarations for Task #1]************************/
/********************[Function declarations for Task #2]************************/
// for the purpose of sorting vector in ascending order
struct SortByAscending{
  bool operator()(const int& lhs, const int& rhs) const{
    return lhs < rhs;
  }
};
// required for list::unique; gets rid of all but one duplicate value.
bool same_integral_part (int first, int second){
    return (first==second);
}
void decision_msg();
int find_closest_index(const vector<int> sorted_vector, int init_head_value);
void update_dir_count(char &prev_dir, int dir_ch_count);
queue<int> generate_fifo_queue(int request_type);
vector<int> generate_sorted_vector(int request_type);
void FCFS(int init_head_value);
void SSTF(int init_head_value);
void SCAN(int init_head_value);
void C_SCAN(int init_head_value);
void LOOK(int init_head_value);
void C_LOOK(int init_head_value);
/********************[Function declarations for Task #3]************************/

/*******************************************************************************
 *                     End of Function declarations                            *
 *******************************************************************************/
int main()
{
    int init_head_value = 2150;
//    printf("\n%*s %*s\n", 22, "head_mov |", 10, "dir_ch/jumps");
//    FCFS(init_head_value);
//    SSTF(init_head_value);
//    SCAN(init_head_value);
    C_SCAN(init_head_value);
//    LOOK(init_head_value);
//    C_LOOK(init_head_value);

    cout << "\nEND PROGRAM\n";
    return 0;
}
/*******************************************************************************
 *                    Beginning of Function definitions                        *
 *******************************************************************************/
/*******************************************************************************
 * Purpose     : Signals to user via console that decision is needed.          *
 * Description : Generates random series of 1,000
 * requests or Accepts user     *
 *               input requests. Reports total amount of head movement and     *
 *               number of times the head changes direction.                   *
 * Params      : init_pos must be within the range [0, 4999] and a positive    *
 *               number.                                                       *
 *******************************************************************************/
void decision_msg(){
    printf("(1) Generate 1,000 random queue requests\n\t=OR=\n"
           "(2) Input requests (terminate with a negative number)\n");
}
/*******************************************************************************
 * Purpose     : Returns closest index relative to initial position.           *
 *               Returns -1 if sorted_vector is empty.                         *
 * Params      : sorted_vector is a sorted int vector containing requests.     *
 *               init_head_value must be a positive number and represents      *
 *               initial head position.                                        *
 *******************************************************************************/
int find_closest_index(const vector<int> sorted_vector, int init_head_value){
    if(init_head_value < 0 || init_head_value > 4999){
        printf("\ninit_pos = %d, out of the range [0, 4999]\n[ABNORMAL EXIT]\n", init_head_value);
        exit(1);
    }
    if(!sorted_vector.empty()){
        int distance = abs(init_head_value - sorted_vector[0]);
        unsigned index = 0;
        // find min distance
        for(unsigned i = 1; i < sorted_vector.size(); i++){
            int temp = abs(init_head_value - sorted_vector[i]);
            if(distance > temp){
                distance = temp;
                index = i;
            }
        }
        return static_cast<int>(index);
    }
    return -1;
}
/*******************************************************************************
 * Purpose     : To update the direction change count.                         *
 * Description : If previous direction was 'R' and current direction is 'L',   *
 *               then increment the direction-change counter by one.           *
 * Params      : prev_dir represents previous direction and dir_ch_count       *
 *               represents the direction-change counter.                      *
 *               Both aforementioned params are passed by reference.           *
 *               difference will represent the value obtained from subtracting *
 *               current head position from previous head position.            *
 *******************************************************************************/
void update_dir_count(char &prev_dir, int &dir_ch_count, int difference){
    char current_direction;
    if(difference < 0){ // going Right
        current_direction = 'R';
    } else if(difference > 0){ // going Left
        current_direction = 'L';
    } else{
        current_direction = prev_dir;
    }
    if(prev_dir == 'U'){ // checking for uninitialized condition
        prev_dir = current_direction;
    }
    if(current_direction != prev_dir){ // update direction-change counter
        dir_ch_count++;
        prev_dir = current_direction;
    }

}
/*******************************************************************************
 * Purpose     : Generates FIFO queue for FCFS disk-scheduling algorithm.      *
 * Description : Based on Params, will either generate 1000 random requests    *
 *               or allow user to input the number of desired requests.        *
 * Params      : request_type represents either a random generation or user    *
 *               created fifo queue.                                           *
 *******************************************************************************/
queue<int> generate_fifo_queue(int request_type){
    string user_input = "";
    int valid_number;
    bool run_loop = true; // initialize while-loop
    srand(time(nullptr));
    queue<int> fifo_queue;
    if(request_type == GENERATE_REQUESTS){ // generate 1,000 random requests range: [0, 4999]
        for(int i = 0; i < NUM_OF_REQUESTS; i++){
            fifo_queue.push(rand() % CYLINDER_MAX);
        }
        return fifo_queue;
    } else if(request_type == PRESENTATION_DEMO){ // presentation demo purposes
        vector<int> intVector = {643, 12, 67, 3567, 2900, 1801, 476, 2,
                                 1123, 4790, 889, 326, 4600, 789, 10, 637,
                                 3127, 17, 699, 3268};
        int size = intVector.size() - 1;
        for(int i = 0; i <= size; i++){
            fifo_queue.push(intVector[i]);
        }
        return fifo_queue;
    }
    printf("Enter positive integers followed by spaces or return-key:\n");
    while(run_loop){
        // gather user input
        cin >> user_input;
        string::const_iterator it = user_input.begin();
        while(it != user_input.end() && isdigit(*it)){
            it++;
        }
        // test if user input is a valid number
        if(it == user_input.end()){
            // success, we have an interger
            valid_number = stoi(user_input);
            fifo_queue.push(valid_number);
        } else{
            valid_number = -1;
            run_loop = false;
        }
    }
    return fifo_queue;
}
/*******************************************************************************
 * Purpose     : Helper function for SCAN disk-scheduling algorithm.           *
 * Description : Generates random series of 1,000 requests or Accepts user     *
 *               input requests.                                               *
 * Params      : request_type represents either a random generation or user    *
 *               created vector. Returns a vector of type int.                 *
 *******************************************************************************/
vector<int> generate_sorted_vector(int request_type){
    string user_input = "";
    int valid_number;
    bool run_loop = true; // initialize while-loop
    srand(time(nullptr));
    vector<int> request_vector;
    if(request_type == GENERATE_REQUESTS){ // generate 1,000 random requests range: [0, 4999]
        for(int i = 0; i < NUM_OF_REQUESTS; i++){
            request_vector.push_back(rand() % CYLINDER_MAX);
        }
        sort(request_vector.begin(), request_vector.end(), SortByAscending());
        return request_vector;
    } else if(request_type == PRESENTATION_DEMO){
        request_vector = {643, 12, 67, 3567, 2900, 1801, 476, 2, 1123,
                          4790, 889, 326, 4600, 789, 10, 637, 3127, 17,
                          699, 3268};
        sort(request_vector.begin(), request_vector.end(), SortByAscending());
        return request_vector;
    }
    printf("Enter positive integers followed by spaces or return-key:\n");
    while(run_loop){
        // gather user input
        cin >> user_input;
        string::const_iterator it = user_input.begin();
        while(it != user_input.end() && isdigit(*it)){
            it++;
        }
        // test if user input is a valid number
        if(it == user_input.end()){
            // success, we have an interger
            valid_number = stoi(user_input);
            request_vector.push_back(valid_number);
        } else{
            valid_number = -1;
            run_loop = false;
        }
    }
    if(request_vector.empty()){
        printf("\nIn generate_sorted_vector(USER_REQUESTS) function\n"
               "User failed to enter at least one valid number\n"
               "[ABNORMAL EXIT]\n");
        exit(1);
    }
    sort(request_vector.begin(), request_vector.end(), SortByAscending());
    return request_vector;
}
/*******************************************************************************
 * Purpose     : Runs FCFS disk-scheduling algorithm.                          *
 * Description : Generates random series of 1,000 requests or Accepts user     *
 *               input requests. Reports total amount of head movement and     *
 *               number of times the head changes direction.                   *
 * Params      : init_head_value must be within the range [0, 4999] and a      *
 *               positive number.                                              *
 *******************************************************************************/
void FCFS(int init_head_value){
//    decision_msg();
    int head_movement_sum = 0;
    int dc_counter = 0;
    char previous_dir = 'U';  // uninitialized direction
    int current_request = init_head_value;
    queue<int> fifo_queue = generate_fifo_queue(USER_INPUT_REQUESTS);
    // make sure that fifi_queue is not empty
    if(fifo_queue.empty()){
        printf("\nIn FCFS(...):\nterminating due to empty fifo_queue\n[ABNORMAL EXIT]\n");
        exit(1);
    }
    // initial setup
    head_movement_sum = current_request - fifo_queue.front();
    if(head_movement_sum < 0){ // going Right
        previous_dir = 'R';
    } else if(head_movement_sum > 0){ // going Left
        previous_dir = 'L';
    }
    update_dir_count(previous_dir, dc_counter, head_movement_sum);
    head_movement_sum = abs(head_movement_sum);
    int difference;
    while(!fifo_queue.empty()){
        current_request = fifo_queue.front();
        fifo_queue.pop();
        // there could be a chance that there are no more entries in the fifo queue since
        // we just popped en entry, check fifo queue.
        if(!fifo_queue.empty()){
            difference = current_request - fifo_queue.front();
            head_movement_sum += abs(difference);
            update_dir_count(previous_dir, dc_counter, difference);
        }
    }
    printf("\n| FCFS  | %*d | %*d |\n", 10, head_movement_sum, 7, dc_counter);
}
/*******************************************************************************
 * Purpose     : Runs SSTF disk-scheduling algorithm.                          *
 * Description : Generates random series of 1,000 requests or Accepts user     *
 *               input requests. Reports total amount of head movement and     *
 *               number of times the head changes direction.                   *
 * Params      : init_head_value must be within the range [0, 4999] and a      *
 *               positive number.                                              *
 *******************************************************************************/
void SSTF(int init_head_value){
//    decision_msg();
    // keep track of head movement
    int head_movement_sum = 0;
    // keep track of direction change count
    int dc_counter = 0;
    // keep track of previous and current direction
    char previous_dir = ' ';
    // keep track of current and next indexes
    int closest_index;
    // predefined list example
    vector<int> requestVector = generate_sorted_vector(USER_INPUT_REQUESTS);
    // initial setup
    closest_index = find_closest_index(requestVector, init_head_value);
    // calculate distance moved realtive to init_head_value
    // let temp be placeholder for distance calculation
    int temp = init_head_value - requestVector[closest_index];
    if(temp < 0){
        previous_dir = 'R'; // moving Right
    } else if(temp > 0){
        previous_dir = 'L'; // moving Left
    } else{ // coincidentally if init_head_value == requestVector[current_index]
        previous_dir = 'U'; // Unchanged since we are already on the mark
    }
    head_movement_sum += abs(temp); // adds the very first head movement calculation
//    printf("******head movement: %d\n", head_movement_sum);

    // Calculate the remaining request queue.
    while(requestVector.size() > 1){
        // remove serviced queue, but save its previous visited value
        int prev_visited_value = requestVector[closest_index];
//        printf("current index = %d\nprev visited value = %d\n", closest_index, prev_visited_value);

        requestVector.erase(requestVector.begin() + closest_index);
        closest_index = find_closest_index(requestVector, prev_visited_value);
        if(closest_index >= 0){
            temp = prev_visited_value - requestVector[closest_index];
            if(temp < 0){ // moving Right
                if(previous_dir == 'L'){
                    dc_counter++;
                }
                previous_dir = 'R';
            } else if(temp > 0){ // moving left
                if(previous_dir == 'R'){
                    dc_counter++;
                }
                previous_dir = 'L';
            }
            head_movement_sum += abs(temp);
//            printf("head movement: %d\n", head_movement_sum);
        }
    }
    printf("\n| SSTF  | %*d | %*d |\n", 10, head_movement_sum, 7, dc_counter);
}
/*******************************************************************************
 * Purpose     : Runs SCAN disk-scheduling algorithm.                          *
 * Description : Generates random series of 1,000 requests or Accepts user     *
 *               input requests. Reports total amount of head movement and     *
 *               number of times the head changes direction.                   *
 * Params      : init_head_value must be within the range [0, 4999] and a      *
 *               positive number.                                              *
 *******************************************************************************/
void SCAN(int init_head_value){
//    decision_msg();
    int dc_counter;
    int head_movement_sum = 0;
    // as the name implies: generating sorted vector
    vector<int> sorted_vector = generate_sorted_vector(USER_INPUT_REQUESTS);
    // find starting index
    int closest_index = find_closest_index(sorted_vector, init_head_value);
    //Edge Case I: init_head_value is <= sorted_vector[0] first index
    //requires no dir change
    if(init_head_value <= sorted_vector[0]){
        dc_counter = 0;
        head_movement_sum += abs(init_head_value - sorted_vector[0])
                          + abs(sorted_vector[0] - sorted_vector[sorted_vector.size() - 1]);
    }
    //Edge Case II: init_head_value is >= sorted_vector[last index]
    // requires no dir change
    else if(init_head_value >= sorted_vector[sorted_vector.size() - 1]){
        dc_counter = 0;
        head_movement_sum += init_head_value - sorted_vector[sorted_vector.size() - 1]
                          + sorted_vector[sorted_vector.size() - 1] - sorted_vector[0];
    }
    // try two calculations and choose the result with the lesser head movement
    else{ // this branch indicates that init_head_value is in between sorted_vector values
        int left_first_move = 0; // head movement when we go left first
        int right_first_move = 0; // head movement when we go right first
        dc_counter = 1;
        left_first_move = right_first_move = init_head_value - sorted_vector[closest_index];
        if(left_first_move < 0){ // calculate left first movements
            // arm moves to cylinder 0 and back to init head movement, then to the end index of sorted vector
            left_first_move = 2 * init_head_value
                              + abs(init_head_value
                              - sorted_vector[sorted_vector.size() - 1]);
        } else{
            left_first_move += 2 * sorted_vector[closest_index]
                               + abs(sorted_vector[closest_index]
                               - sorted_vector[sorted_vector.size() - 1]);
        }
        // calculate right first movements
        if(right_first_move <= 0){ // indicates we are indeed going in the right direction
            right_first_move *= -1; // make value positive
            right_first_move += 2 * abs(sorted_vector[closest_index]
                                - (CYLINDER_MAX - 1))
                                + abs(sorted_vector[closest_index] - sorted_vector[0]);
        } else{
            right_first_move = 2 * abs(init_head_value - (CYLINDER_MAX - 1))
                               + abs(init_head_value - sorted_vector[0]);
        }
        // finding the lesser head movement
        if(left_first_move <= right_first_move){
            head_movement_sum = left_first_move;
        } else{
            head_movement_sum = right_first_move;
        }
    }
    printf("\n| SCAN  | %*d | %*d |\n", 10, head_movement_sum, 7, dc_counter);
}
/*******************************************************************************
 * Purpose     : Runs C-SCAN disk-scheduling algorithm.                        *
 * Description : Generates random series of 1,000 requests or Accepts user     *
 *               input requests. Reports total amount of head movement and     *
 *               number of times the head "jumps" to the other end.            *
 * Params      : init_head_value must be within the range [0, 4999] and a      *
 *               positive number.                                              *
 *******************************************************************************/
void C_SCAN(int init_head_value){
//    decision_msg();
    /*  3 different scenarios exist:
            (1) initial head is in between the vector of requests
                ex: [(index 0)  ...  (init_head_value) ... (max index)]
            (2) initial head resides in the left area such that initial head < index 0
                ex: [(init_head_value) ... (index 0) ... (max index)]
            (3) initial head resides in the right area such that initial head > max index
                ex: [(index 0) ... (max index) ... (init_head_value)]
        We will need to determine which initial direction yields optimal total head movement.
    */
    int jump_counter = 0;
    int head_movement_sum = 0;
    int left_first_move;
    int right_first_move;
    vector<int> sorted_vector = generate_sorted_vector(USER_INPUT_REQUESTS);
    int max_index = sorted_vector.size() - 1;
    int closest_index = find_closest_index(sorted_vector, init_head_value);
    // figure out difference between init_head_value and sorted_vector[closest_index]
    int offset = init_head_value - sorted_vector[closest_index];
    // test for scenario (1)
    if(init_head_value > sorted_vector[0] && init_head_value < sorted_vector[max_index]){
        jump_counter++; // either direction results in a circular-jump
        if(offset <= 0){
            offset *= -1; // set offset to positive
            right_first_move = abs(init_head_value - (CYLINDER_MAX - 1)) + sorted_vector[closest_index - 1];
            left_first_move = init_head_value + ((CYLINDER_MAX - 1) - sorted_vector[closest_index]);
        } else{ // if(offset > 0)
            right_first_move = abs(init_head_value - (CYLINDER_MAX - 1)) + sorted_vector[closest_index];
            left_first_move = init_head_value + ((CYLINDER_MAX - 1) - sorted_vector[closest_index + 1]);
        }
        if(right_first_move <= left_first_move){
            head_movement_sum += right_first_move;
        } else{
            head_movement_sum += left_first_move;
        }
    } // test for scenario (2)
    else if(init_head_value <= sorted_vector[0]){
        right_first_move = abs(init_head_value - sorted_vector[0])
                           + abs(sorted_vector[0] - sorted_vector[max_index]);
        left_first_move = init_head_value + (CYLINDER_MAX - 1) - sorted_vector[0];
        if(right_first_move <= left_first_move){
            head_movement_sum += right_first_move;
        } else{
            jump_counter++; // if we go left, then there will be a circular-jump
            head_movement_sum += left_first_move;
        }
    } // test for scenario (3)
    else{ // if(init_head_value >= sorted_vector[max_index])
        right_first_move = abs(init_head_value - (CYLINDER_MAX - 1))
                           + sorted_vector[max_index];
        left_first_move = init_head_value - sorted_vector[0];
        if(right_first_move <= left_first_move){
            jump_counter++; // circular-jump exists when we go right
            head_movement_sum += right_first_move;
        } else{
            head_movement_sum += left_first_move;
        }
    }
    printf("\n| CSCAN | %*d | %*d |\n", 10, head_movement_sum, 7, jump_counter);
}
/*******************************************************************************
 * Purpose     : Runs LOOK disk-scheduling algorithm.                          *
 * Description : Generates random series of 1,000 requests or Accepts user     *
 *               input requests. Reports total amount of head movement and     *
 *               number of times the head changes direction.                   *
 * Params      : init_head_value must be within the range [0, 4999] and a      *
 *               positive number.                                              *
 *******************************************************************************/
void LOOK(int init_head_value){
//    decision_msg();
    int dc_counter;
    int head_movement_sum = 0;
    // as the name implies: generating sorted vector
    vector<int> sorted_vector = generate_sorted_vector(PRESENTATION_DEMO);
    // find starting index
    int max_index = sorted_vector.size() - 1;
    int closest_index = find_closest_index(sorted_vector, init_head_value);
    //Edge Case I: init_head_value is <= sorted_vector[0] first index
    //requires no dir change
    if(init_head_value <= sorted_vector[0]){
        dc_counter = 0;
        head_movement_sum += abs(init_head_value - sorted_vector[0])
                          + abs(sorted_vector[0] - sorted_vector[max_index]);
    }
    //Edge Case II: init_head_value is >= sorted_vector[last index]
    // requires no dir change
    else if(init_head_value >= sorted_vector[max_index]){
        dc_counter = 0;
        head_movement_sum += init_head_value - sorted_vector[max_index]
                          + sorted_vector[max_index] - sorted_vector[0];
    }
    //Scenario where init_head_movement is in between sorted_vector range
    else{
        int left_first_move = 0; // head movement when we go left first
        int right_first_move = 0; // head movement when we go right first
        dc_counter = 1;
        left_first_move = init_head_value - sorted_vector[0]
                          + abs(sorted_vector[0] - sorted_vector[max_index]);
        // calculate right first movements
        right_first_move = abs(init_head_value - sorted_vector[max_index])
                           + (sorted_vector[max_index] - sorted_vector[0]);
        // finding the lesser head movement
        if(left_first_move <= right_first_move){
            head_movement_sum += left_first_move;
        } else{
            head_movement_sum += right_first_move;
        }
    }
    printf("\n| LOOK  | %*d | %*d |\n", 10, head_movement_sum, 7, dc_counter);
}
/*******************************************************************************
 * Purpose     : Runs C-LOOK disk-scheduling algorithm.                        *
 * Description : Generates random series of 1,000 requests or Accepts user     *
 *               input requests. Reports total amount of head movement and     *
 *               number of times the head "jumps" to the other end.            *
 * Params      : init_head_value must be within the range [0, 4999] and a      *
 *               positive number.                                              *
 *******************************************************************************/
void C_LOOK(int init_head_value){
//    decision_msg();
    /*  3 different scenarios exist:
            (1) initial head is in between the vector of requests
                ex: [(index 0)  ...  (init_head_value) ... (max index)]
            (2) initial head resides in the left area such that initial head < index 0
                ex: [(init_head_value) ... (index 0) ... (max index)]
            (3) initial head resides in the right area such that initial head > max index
                ex: [(index 0) ... (max index) ... (init_head_value)]
        We will need to determine which initial direction yields optimal total head movement.
    */
    int jump_counter = 0;
    int head_movement_sum = 0;
    int left_first_move;
    int right_first_move;
    vector<int> sorted_vector = generate_sorted_vector(USER_INPUT_REQUESTS);
    int max_index = sorted_vector.size() - 1;
    int closest_index = find_closest_index(sorted_vector, init_head_value);
    // figure out difference between init_head_value and sorted_vector[closest_index]
    int offset = init_head_value - sorted_vector[closest_index];
    // test for scenario (1)
    if(init_head_value > sorted_vector[0] && init_head_value < sorted_vector[max_index]){
        jump_counter++; // either direction results in a circular-jump
        if(offset <= 0){
            offset *= -1; // set offset to positive
            right_first_move = abs(init_head_value - sorted_vector[max_index])
                               + abs(sorted_vector[0] - sorted_vector[closest_index - 1]);
        } else{ // if(offset > 0)
            right_first_move = abs(init_head_value - sorted_vector[max_index])
                               + abs(sorted_vector[0] - sorted_vector[closest_index]);
        }
        left_first_move = (init_head_value - sorted_vector[0])
                          + (sorted_vector[max_index] - sorted_vector[closest_index]);
        if(right_first_move <= left_first_move){
            head_movement_sum += right_first_move;
        } else{
            head_movement_sum += left_first_move;
        }
    } // test for scenario (2)
    else if(init_head_value <= sorted_vector[0]){
        right_first_move = abs(init_head_value - sorted_vector[0])
                           + abs(sorted_vector[0] - sorted_vector[max_index]);
        left_first_move = init_head_value + sorted_vector[max_index] - sorted_vector[0];
        if(right_first_move <= left_first_move){
            head_movement_sum += right_first_move;
        } else{
            jump_counter++; // if we go left, then there will be a circular-jump
            head_movement_sum += left_first_move;
        }
    } // test for scenario (3)
    else{ // if(init_head_value >= sorted_vector[max_index])
        right_first_move = abs(init_head_value - (CYLINDER_MAX - 1))
                           + sorted_vector[max_index];
        left_first_move = init_head_value - sorted_vector[0];
        if(right_first_move <= left_first_move){
            jump_counter++; // circular-jump exists when we go right
            head_movement_sum += right_first_move;
        } else{
            head_movement_sum += left_first_move;
        }
    }
//    printf("\n| CLOOK | %*d | %*d |\n", 10, head_movement_sum, 7, jump_counter);
    printf("\nCLOOK algorithm results:\nhead_movement_sum: %d\njump_counter: %d\n", head_movement_sum, jump_counter);
}
/*******************************************************************************
 *                      End of Function definitions                            *
 *******************************************************************************/
