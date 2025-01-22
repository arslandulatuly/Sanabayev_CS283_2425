#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//Arslan Sanabayev, as5764, 01/21/2025

#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
int reverse_string(char *reversed, char *buff, int str_len);
int word_print(char *buff, int str_len);
int replace_word(char *buff, int str_len, const char *old_word, const char* new_word);
//add additional prototypes here


int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
	int inp_str = 0;
	int buf_str = 0;
	int space = 0;
	
	if (buff == NULL || user_str == NULL) {
		return -2; // return -2 for null pointers because that would be invalid input
	}

	while (*(user_str + inp_str) != '\0') {
		if (buf_str >= len - 1) {
			printf("error : Provided input string is too long");
			exit(3);
			
		}
		char current_char = *(user_str + inp_str);
		if (current_char == ' ' || current_char == '\t') {
			if(space == 0) {
				*(buff + buf_str) = ' ';
				buf_str++;
				space = 1;
			}
		}
		else {
			*(buff + buf_str) = current_char;
			buf_str++;
			space = 0;
		}	
	inp_str++;
	}
	memset(buff + buf_str, '.', len - buf_str);
	return buf_str;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int count = 0;
	 int word = 0;

	for (int i = 0; i < str_len; i++) {
		if (*(buff + i) != ' ' && *(buff+i) != '\t') {
			if(!word) {
				count++;
				word = 1;
			}
		}
		else {
			word = 0;
		}
	}
	return count;
}

int reverse_string(char *reversed, char *buff, int str_len) {
	int real_len = 0;
	char *end = buff;

	while (*(end) != '.' && real_len < str_len) {
		real_len++;
		end++;
	}
	char *start = buff;
	char *rev_ptr = reversed;
	end--;
	while (start <= end) {
		*rev_ptr = *end;
		rev_ptr++;
		end--;
		start++;
	}

	while (rev_ptr < (reversed + BUFFER_SZ)) {
		*rev_ptr = '.';
		rev_ptr++;
	}

	*rev_ptr = '\0';
	return 0;
}


int word_print(char *buff, int str_len) {
    int word = 0;
    int word_count = 0;
	 int char_count = 0;

    printf("Word Print\n");
    printf("----------\n");

    for (int i = 0; i <= str_len; i++) {
        if (*(buff + i) != ' ' && *(buff + i) != '.') {
            if (!word) {
                word_count++;
					 char_count = 0;
                printf("%d. ", word_count);
                word = 1;
            }
            putchar(*(buff + i));
				char_count++;
        } else {
            if (word) {
                printf(" (%d)\n", char_count);
                word = 0;
            }
        }
    }

    return 0;
}

int replace_word(char *buff, int str_len, const char *old_word, const char *new_word) {
    // Length of old_word and new_word, using pointer-based operations
    int old_word_len = 0;
    while (*(old_word + old_word_len) != '\0') {
        old_word_len++;
    }

    int new_word_len = 0;
    while (*(new_word + new_word_len) != '\0') {
        new_word_len++;
    }

    char *pos = buff;
    int replaced_count = 0;

    while (pos < (buff + str_len)) {
        // Find the old word in the buffer
        if (strncmp(pos, old_word, old_word_len) == 0) {
            // If the new word is longer, shift characters in the buffer
            if (new_word_len > old_word_len) {
                // Check if there's enough space in the buffer
                if (str_len + (new_word_len - old_word_len) > BUFFER_SZ) {
                    printf("Error: Buffer is too small for replacement.\n");
                    return -1;
                }

                // Move the characters after the old word to the right
                char *shift_pos = (buff + str_len - 1);
                while (shift_pos >= (pos + old_word_len)) {
                    *(shift_pos + (new_word_len - old_word_len)) = *shift_pos;
                    shift_pos--;
                }
            } else if (new_word_len < old_word_len) {
                // If the new word is shorter, shift characters to the left
                char *shift_pos = (buff + (pos - buff) + old_word_len);
                while (shift_pos <= (buff + str_len)) {
                    *(shift_pos - (old_word_len - new_word_len)) = *shift_pos;
                    shift_pos++;
                }
            }

            // Replace the old word with the new word
            for (int i = 0; i < new_word_len; i++) {
                *(pos + i) = *(new_word + i);
            }

            // Move position to the end of the newly replaced word
            pos += new_word_len;
            replaced_count++;
        } else {
            pos++;  // Move to the next character
        }
    }
    return replaced_count;
}




//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
int string_length(char *str) {
    int length = 0;
    while (*(str + length) != '\0') {  
        length++;
    }
    return length;
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      because argc makes sure there's at least one argument aside from argv[0] which is always there. second
	 //      condition makes sure that the second argument always starts with a '-', which indicates a parameter, else returning -1 status and stopping the program due to a command line problem. 
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      the previous statement ensured for another argument (the parameter) to be passed in, and this if ensures that there is another string for the program to process after this parameter.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3

	buff = (char *)malloc(BUFFER_SZ * sizeof(char));
	if (buff == NULL) {
		printf("Error: Memory allocation failed.\n");
		exit(99);
	}

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
					 free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
			

			case 'r': {
				char reversed[user_str_len + 1];
				reverse_string(reversed, buff, user_str_len);
				printf("Reversed String : %s\n", reversed);
				break;
			}

			case 'w' : {
				rc = word_print(buff, user_str_len);
				if(rc < 0) {
					printf("Error printing words, rc = %d\n", rc);
					free(buff);
					exit(3);
				}
				break;
			}

  	  case 'x': {
  	      if (argc != 5) {
  	          printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", argv[0]);
  	          exit(1);
	        }
			const char *old_word = argv[3];
			const char *new_word = argv[4];
			rc = replace_word(buff, user_str_len, old_word, new_word);
			if (rc < 0) {
				free(buff);
				exit(3);
			}
			printf("Modified String: %s\n", buff);
    	    break;
    	}

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        default:
            usage(argv[0]);
				free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
	 free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          i think that it is a good practice firstly because if we do not provide a pointer, the program
//          might try to write more than the size of allocated memory it has. also we always know the stable size of a
//          buffer, which is a benefit of having a constant
