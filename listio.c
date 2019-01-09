#include "listio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

struct returnStruct *buildHeader()
{
    struct returnStruct * new;
    
    new = malloc(sizeof(struct returnStruct));
    new->header = malloc(sizeof(struct dataHeader));
    
    if(new->header == NULL)
    {
        new->value = FAILURE;
        return new;
    }
    else
    {
        new->value = SUCCESS;
        new->header->length = 0;
        new->header->name = NULL;
        new->header->next = NULL;
    }
    
    return new;
}

int setName(struct dataHeader *header, char *name)
{
    header->name = malloc(sizeof(char) * (strlen(name) + 1));
    header->name[strlen(name)] = '\0';
    
    if(header->name == NULL || name == NULL)
    { 
        return FAILURE;
    }
    
    strcpy(header->name, name);
    return SUCCESS;
}

char * getName(struct dataHeader *header)
{
    return (header->name == NULL) ? NULL : header->name;
}

int getLength(struct dataHeader *header)
{
    return header->length;
}

//PURPOSE:: default constructor for the dataString type
//ARGUMENTS:: the dataString, and the string to assign, and the header->length
//RETURNS:: the allocated string
struct dataString * new_dataString(struct dataString * elem, char * str, 
    int * total_length, int * status)
{
    elem = malloc(sizeof(struct dataString));
    
    if(elem == NULL)
    {
        *status = FAILURE;
        return NULL;
    }
    
    elem->string = malloc(sizeof(char) * (strlen(str) + 1));
    
    if(elem->string == NULL)
    {
        *status = FAILURE;
        return NULL;
    }
    else
    {
        strcpy(elem->string, str);
    }
    
    *total_length += strlen(elem->string) + 1;
    
    elem->next = NULL;
    return elem;
}

int addString(struct dataHeader *header, char *str)
{
    struct dataString * temp;
    int status = SUCCESS;
    
    if(header == NULL ||
        str == NULL)
    {
        return FAILURE;
    }
    
    if(header->next == NULL) //only if it's the first element
    {
        header->next = new_dataString(header->next, str, &header->length, 
            &status);
    }
    else //otherwise traverse til it reaches the end
    {
        temp = header->next;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_dataString(temp->next, str, &header->length, 
            &status);
    }
    
    return status;
}

//PURPOSE:: will look for repeats of any character in a string and remove them.
//
//ARGUMENTS:: the string to modify, the character to find, and the current number
//of characters of the header
//
//RETURNS:: the modified string (will be the same if the repeated character
//was not found.
//
char * remov_repeats(char * string, char toFind, int * total_length, 
    int * status)
{
    int length = strlen(string) + 1;
    char * new_string;

    new_string = malloc(sizeof(char) * (strlen(string) + 1));
    if(new_string == NULL)
    {
        *status = FAILURE;
        return NULL;
    }
    
    new_string[0] = '\0';
    
    for(int i = 0; i < length; i++)
    {
        if(string[i] == toFind &&
            string[i + 1] == toFind) //check if the charater repeats
        {
            (*total_length)--;
        }
        else //otherwise, just add the next element in the string
        {
            new_string = realloc(new_string, sizeof(char) * 
                (strlen(new_string) + 2) );
            
            if(new_string == NULL)
            {
                *status = FAILURE;
                return NULL;
            }
                
            strncat(new_string, &string[i], 1);
        }
    }
    
    string = realloc(string, sizeof(char) * (strlen(new_string) + 1));
    
    if(string == NULL)
    {
        *status = 0;
        return NULL;
    }
    
    strcpy(string, new_string);
    free(new_string);
    
    return string;
}

//PURPOSE:: goes through and replaces '\r's and '\n's with <BR> (if there is 1)
//or <P> (if there is more than 1 sequentially).
//
//ARGUMENTS:: the string to be dealt with, and the overall header->length for
//the struct
//
//RETURNS:: the string passed in, but now modified. If there was nothing to 
//change in the string, it will not be damaged.
//
char * replace_EOLs(char * string, int * header_length, int * status)
{
    int str_length = strlen(string) + 1;
    int tag_num, tag_size, num_EOLs = 0;
    
    char * tags [] = {"<BR>", "<P>"}, * new_string;
    
    new_string = malloc(sizeof(char) * str_length );
    if(new_string == NULL)
    {
        *status = FAILURE;
        return NULL;
    }
    
    new_string[0] = '\0';
    *header_length -= str_length;
    
    for(int i = 0; i < str_length; i++)
    {
        if(string[i] == '\n' || string[i] == '\r') //if there is an EOL
        {
            while(isspace(string[i]))
            {
                switch(string[i++])
                {
                    case '\r': case '\n': num_EOLs++; break;
                }
            }
            i--;
            
            switch(string[i])
            {
                case '\t': case ' ': i--; break;
            }
            
            switch(num_EOLs) //chooses one of the tags to concat to the string
            {
                case 1: tag_num = 0; break;
                default: tag_num = 1; break;
            }
            
            tag_size = strlen(tags[tag_num]);
            
            new_string = realloc(new_string, sizeof(char) * 
                (tag_size + strlen(new_string) + 1));
                
            if(new_string == NULL)
            {
                *status = FAILURE;
                return NULL;
            }
            
            strcat(new_string, tags[tag_num]); //adds tag instead of next char
            *header_length += tag_size;
            
            num_EOLs = 0;
        }
        else //otherwise, add the next character
        {
            new_string = realloc(new_string, sizeof(char) * 
                (strlen(new_string) + 2 ));
                
            if(new_string == NULL)
            {
                *status = FAILURE;
                return NULL;
            }
            
            strncat(new_string, &string[i], 1);
            (*header_length)++;
        }
    }
    string = realloc(string, sizeof(char) * (strlen(new_string) + 1));
    
    if(string == NULL)
    {
        *status = FAILURE;
        return NULL;
    }
    
    strcpy(string, new_string);
    free(new_string);
    
    return string;
}

int printString(struct dataHeader *header)
{
    struct dataString * start = header->next;

    if(header->name != NULL)
    {
        printf("%s\n", header->name);
    }
    
    while(start != NULL) 
    {
        printf("%s\n", start->string);
        start = start->next;
    }
    
    return SUCCESS;
}

void freeElem(struct dataString * elem)
{
    if(elem->next != NULL)
    {
        freeElem(elem->next);
    }
    free(elem->string);
    free(elem);
}

int freeStructure(struct dataHeader *header)
{
    if(header != NULL)
    {
        if(header->next != NULL)
        {
            freeElem(header->next);
        }
    
        if(header->name != NULL)
        {
            free(header->name);
        }
        free(header);
    }
    return SUCCESS;
}

//PURPOSE:: converts the text given to it into html, and then displays it 
//to stdout, as well as writing it to a newly-created html file by
//calling a python program called writer.py
//
//ARGUMENTS:: the header with the text that needs to be converted
//
//RETURNS 
int convert_to_html(struct dataHeader * header)
{
    FILE * fifo, * html;
    pid_t child;
    
    int status;
    struct dataString * start = header->next;

    char * args [] = {"./writer.py", header->name, NULL};
    char * html_file, buffer[5000];

    html_file = malloc(sizeof(char) * (strlen(".html") + 
        strlen(header->name) + 1));
    
    html_file[strlen(".html") + strlen(header->name)] = '\0';
    
    strcpy(html_file, header->name);
    strcat(html_file, ".html");
    
    mkfifo("myfifo", 0777);
    mkfifo("return_fifo", 0777);
    
    child = fork();
    if( (child == 0 &&
        execvp(args[0], args) == -1) 
        || (fifo = fopen("myfifo", "w")) == NULL)
    {
        return FAILURE;
    }
    
    //writing to the first fifo, the one to be sent to writer.py
    while(start != NULL)
    {
        fprintf(fifo, "%s\n",start->string);   
        start = start->next;
    }
    fclose(fifo);
    
    //reading in the data sent back from writer.py in the second fifo made
    if( (fifo = fopen("return_fifo", "r")) == NULL ||
        (html = fopen(html_file, "w")) == NULL)
    {
        return FAILURE;
    }

//    chmod(html_file, 0755);
        
    freeElem(header->next); //clear all elements recursively, except name of file

    header->next = NULL;
    header->length = 0;

    while(fgets(buffer, 200, fifo) != NULL)
    {
        fprintf(html, "%s", buffer);
        addString(header, buffer);
    }
    
    if(child != 0)
    {
        waitpid(child, &status, 0);
    }
    
    fclose(html);
    fclose(fifo);
    
    remove("return_fifo");
    remove("myfifo");
        
    free(html_file);
    return SUCCESS;
}

void remove_extra_breaks(struct dataHeader * header)
{
    struct dataString * end, * next, * start = header->next;
    char * strings_end;
    int len, cleared = 0;

    next = start->next;
    
    while(start != NULL && next != NULL)
    {
        next = start->next;
        if(strrchr(start->string, '<') == NULL)
        {
            start = start->next;
            continue;
        }
        
        len = strlen(strrchr(start->string, '<'));
        
        strings_end = malloc(sizeof(char) * (len + 1));
        strings_end[len] = '\0';
        
        strcpy(strings_end, strrchr(start->string, '<'));
        
        if(strings_end != NULL &&
            len >= 4 &&
            strncmp(strings_end, "<BR>", 4) == 0)
        {
            if(len >= 5)
            {
                if(isalpha(strings_end[5]) != 0 ||
                    isalpha(strings_end[6]) != 0)
                {
                    start = start->next;
                    continue;
                }
            }
            end = start; //end has the string where <P> is changed to <BR>
        
            while(next != NULL && strlen(next->string) == 4 
                && strcmp(next->string, "<BR>") == 0) //gets rid of exces <BR>
            {
                start->next = start->next->next;
                header->length = header->length - 4;
                free(next->string);
                free(next);
                next = start->next;
                cleared = 1;
            }
            
            next = start->next;
            
            if(cleared == 1)
            {
                int i = 0, j = 0;
                len = strlen(end->string);
                
                while(i < len && end->string[i++] != strings_end[0]);
            
                --i;
                while(j < 3)
                {
                    end->string[i++] = "<P>"[j++];
                }
            
                end->string = realloc(end->string, sizeof(char) * len);
            
                if(end->string == NULL)
                {
                    return;
                }   
            
                end->string[len - 1] = '\0'; //make string 1 char less
                header->length--;
                cleared = 0;
            }
        }
        free(strings_end);
        start = start->next;    
    }
}

int processStrings(struct dataHeader *header)
{
    if(header == NULL)
    {
        return FAILURE;
    }
    
    struct dataString * start = header->next;
    int status = SUCCESS;
    
    while(start != NULL)
    {
        start->string = remov_repeats(start->string, ' ', &header->length, 
            &status);
        
        if(status == FAILURE) 
        {
            return FAILURE;
        }
            
        start->string = remov_repeats(start->string, '\t', &header->length,
            &status);
        
        if(status == FAILURE) 
        {
            return FAILURE;
        }
            
        start->string = replace_EOLs(start->string, &header->length, 
            &status);
            
        if(status == FAILURE) 
        {
            return FAILURE;
        }
        
        start = start->next;
    }
    
    remove_extra_breaks(header);
    
    return convert_to_html(header);
}

int writeStrings(char *filename, struct dataHeader * header)
{
    FILE * dest;
    struct dataString * temp;
    int len;
    
    if(filename == NULL || 
        header == NULL)
    {
        return FAILURE;
    }
    
    len = strlen(header->name) + 1;

    temp = header->next;
    
    if( filename == NULL ||
        (dest = fopen(filename, "w+")) == NULL ||
        fwrite(&len, sizeof(int), 1, dest) != 1 ||
        fwrite(header->name, sizeof(char), len, dest) != len ||
        fwrite(&header->length, sizeof(int), 1, dest) != 1)
    {
        return FAILURE;
    }
    
    while(temp != NULL)
    {
        len = strlen(temp->string) + 1;
        
        if( fwrite(&len, sizeof(int), 1, dest) != 1 ||
            fwrite(temp->string, sizeof(char), len, dest) != len)
        {
            return FAILURE;
        }
        
        temp = temp->next;
    }
    fclose(dest);
    return SUCCESS;
}

struct returnStruct *readStrings(char *filename) 
{
    FILE * source;
    int bytes, total_bytes, i = 0;
    
    char * new_string = NULL;
    
    struct returnStruct * new;
    
    new = buildHeader();
    
    if(new->value != SUCCESS) //makes sure the struct was properly initialized
    {
        return new;
    }
    
    new->value = FAILURE; //so that it doesn't have to be reset to FAILURE later
    
    if( (source = fopen(filename, "r")) == NULL ||
        fread(&bytes, sizeof(int), 1, source) != 1)
    {     
        return new;
    }
    
    new_string = malloc(sizeof(char) * bytes);
    
    if( new_string == NULL ||
        fread(new_string, sizeof(char), bytes, source) != bytes ||
        fread(&total_bytes, sizeof(int), 1, source) != 1 ||
        setName(new->header, new_string) == FAILURE)
    {
        return new;
    }
    
    free(new_string);
    
    while(i < total_bytes)
    {
        if( fread(&bytes, sizeof(int), 1, source) != 1)
        {
            return new;
        }
        
        new_string = malloc(sizeof(char) * bytes);
        
        if( new_string == NULL ||
            fread(new_string, sizeof(char), bytes, source) != bytes ||
            addString(new->header, new_string) == FAILURE)
        {
            return new;
        }
        free(new_string);
        
        i = i + bytes;
    }
    fclose(source);
    
    new->value = SUCCESS;
    return new;
}
