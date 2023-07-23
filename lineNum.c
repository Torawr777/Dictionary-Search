#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/**********************************************************************
Search for given word in dictionary using file descriptor fd.
Return the line number the word was found on, negative of the last line searched
if not found, or the error number if an error occurs.
**********************************************************************/

//Counts the length of a string 
int sLen(char *string) {
	int length = 0;

	for(int n = 0; string[n] > 0; n++) { 
		length++; 
	}
	return length;
}

//Concatenates strings 
//Thanks to https://www.techiedelight.com/implement-strcat-function-c/
char *sCat(char *addr, char *string) {
	char *n = addr + sLen(addr);

	while(*string != '\0') {  //Go to end of string
		*n++ = *string++; 
	}
	*n = '\0';

	return addr;
}

char *sRemove(char *addr, char *string) {
	char *n = addr + sLen(addr);

	while(*string != '\0') {  //Go to end of string
		*n++ = *string++; 
	}
	*n = '\0';

	return addr;

}

//Add spaces to the end of strings
char *add_spaces(char *string, int length) {
	int spaces = length - sLen(string)-1; 
	char *str = (char*)calloc(100,1);

	//Concatenate input string to memory assigned string
	sCat(str, string); 

	for(int i = 0; i < spaces; i++) { 
		sCat(str, " "); 
	}
	return str;
}


int lineNum(char *dictionaryName, char *word, int length) {
	int fd, compare, maxLine, mid;
	int minLine = 1;
	char *spaced_word;
	char buffer[length];


	fd = open(dictionaryName, O_RDONLY); //Open dictionary with read only

	//Test if file exists/readable
	if (fd < 0) { 
		write(2, strerror(errno), sLen(strerror(errno)));
		return errno; 
	}

	//Test if file can be offset
	if (lseek(fd, 0, SEEK_SET) < 0) {
		write(2, strerror(errno), sLen(strerror(errno)));
		return errno; 
	}

	//Get max line 
	maxLine = lseek(fd, 0, SEEK_END)/length; 
	lseek(fd, 0, SEEK_SET); //Return pointer to start of file

	//Add spaces to word; "truncate"
	spaced_word = add_spaces(word, length);


    while(maxLine >= minLine) {
    	mid = minLine + (maxLine - minLine)/2;

    	//More error checking
    	if(lseek(fd, mid*length, SEEK_SET) < 0) {
			write(2, strerror(errno), sLen(strerror(errno)));
			return errno;
		}
		if(read(fd, buffer, length-1) < 0) {
			write(2, strerror(errno), sLen(strerror(errno)));
			return errno;
		}

        buffer[length-1] = '\0';
        compare = strcmp(buffer, spaced_word);

        //Binary search
        //Comparing by ASCII values
     	if(compare > 0) { maxLine = mid - 1; } 
     	else if(compare < 0) { minLine = mid + 1; } 
     	
     	else if(compare == 0) { //Match found
     		free(spaced_word);
     		return mid+1; 
     	} 
    } 

	close(fd); 
	free(spaced_word);
	return mid*-1;
}
