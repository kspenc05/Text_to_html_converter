#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "listio.h"
#include "mysql/mysql.h"
#include <errno.h>

//PURPOSE:: checks if a string has an html file ext
//ARGUMENTS:: the string
//RETURNS 1 if an html file, 0 otherwise
int is_an_html_file (char * string)
{
    int j = 0;
    
    if(strlen(string) > 5)
    {
        for(int i = strlen(string) - strlen(".html");
            string[i] != '\0'; i++)
        {
            if(string[i] != ".html"[j++])
            {
                return 0;
            }
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

//PURPOSE:: escapes any of the characters in a string
//that will not store properly in mysql
//so far handles single quote or '\' chars.
//ARGUMENTS:: the string
//RETURNS:: allocated string with the escaped characters added
char * escape_single_char(char * string)
{
    char * text;
    int temp = 0, i;

    if(strlen(string) == 0)
    {
        return NULL;
    }

    text = malloc(sizeof(char) * (strlen(string) + 1));
    text[0] = '\0';
  
    temp = strlen(string)+1;
    int size = strlen(string);

    for(i = 0; i < size; i++)
    {
        strncat(text, &string[i], 1);

        if(string[i] == '\'' || string[i] ==  '\\')
        {
            text = realloc(text, sizeof(char) * (strlen(text) + ++temp));
            
            (string[i] == '\'') ? strncat(text, "\'", 1) : strncat(text, "\\", 1);
        }
    }
    return text;
}

//PURPOSE:: gets all of the text in the C
//data structure defined in listio.h 
//and concats it to 1 single string
//
//ARGUMENTS:: the data structure
//RETURNS:: the mega string that contains all the text
char * get_text(struct dataHeader * start)
{
    char * text, * temp;
    struct dataString * elem = start->next;
    text = malloc(sizeof(char) * start->length);
    text[0] = '\0';

    while(elem != NULL)
    {
        temp = escape_single_char(elem->string);
        strcat(text, temp);
        free(temp);

        elem = elem->next;
    }
    return text;
}

//PURPOSE:: default error message sent from mysql
void error_fail(MYSQL * mysql)
{
    fprintf(stderr, "%s\n", mysql_error(mysql));
}

//PURPOSE:: Removes a string at the front of another string
//ARGUMENTS:: the substring to remove, and the string 
//to remove it from
//RETURNS newly re-allocated string without the unwanted
//substring given
void remove_str_front(char * remove, char ** str)
{
    int chomp = strlen(remove), i, end = strlen(*str);

    char * new_str = malloc(sizeof(char) * (end + 1));
    new_str[0] = '\0';
    strcpy(new_str, *str);

    if(strncmp(remove, *str, chomp) == 0)
    {
        for(i = chomp; i < end; i++)
        {
            new_str[i-chomp] = new_str[i];
        }
        new_str[i-chomp] = '\0';
        new_str = realloc(new_str, sizeof(char) * ( (strlen(new_str) + 1)) );
        
        free(*str);
        *str = malloc(sizeof(char) * (strlen(new_str) + 1));
        *str[0] = '\0';        

        strcpy(*str, new_str);
        free(new_str);
    }
}

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        return EXIT_FAILURE;
    }
    
    MYSQL mysql;
    struct returnStruct * new;

    FILE * fp;
    char buffer [500], * filename, * text, * query;
    time_t current;

    if(is_an_html_file(argv[1]) )
    {
        filename = malloc(sizeof(char) * ((strlen(argv[1]) - 
             strlen(".html") + 1)));
 
        filename[strlen(argv[1]) - strlen(".html")] = '\0';
        strncpy(filename, argv[1], strlen(argv[1]) - strlen(".html") );
    }
    else
    {
        filename = malloc(sizeof(char) * (strlen(argv[1]) + 1));
        strcpy(filename, argv[1]);
    }

    if( !(fp = fopen(filename, "r")) )
    {
        printf("could not open file %s\n", filename);
        free(filename);
        return EXIT_FAILURE;
    }
      
    new = buildHeader();
        
    if(!new->value)
    {
        return EXIT_FAILURE;
    }

    while(fgets(buffer, 500, fp) )
    {
        if(!addString(new->header, buffer) )
        {
            return EXIT_FAILURE;
        }
    }

    if(!setName(new->header, filename) ||
        !processStrings(new->header) )
    {
        return EXIT_FAILURE;
    }        

    free(filename);

    if(argc >= 3 && argv[2][0] == 'd' && argv[2][1] == 'b')
    {
        mysql_init(&mysql);
        mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP,  "mydb");
        
        if(!mysql_real_connect(&mysql, "dursley.socs.uoguelph.ca", "kspenc05",
            "0872780", "kspenc05", 0, NULL, 0) )
        {
            error_fail(&mysql);
            return EXIT_FAILURE;
        }        

        if(mysql_query(&mysql, 
            "create table if not exists html_files"
            " (id int not null auto_increment,"
            " text text (5000), length int (4),"
            " file_name char (255), date char (255), primary key(id))"))
        {
            error_fail(&mysql);
            return EXIT_FAILURE; 
        }

        current = time(NULL);

        text = get_text(new->header);

        query = malloc(sizeof(char) * (new->header->length + 200));
        query[0] = '\0';


        sprintf(query, "%s%s",  new->header->name, ".html");
        remove(query); //remove the file, sincce it is now in the database


        //if ./files/ is in front of the filenaame, removes it from the name
        remove_str_front("./files/", &new->header->name);

        query[0] = '\0';

        sprintf(query,
             "Insert into html_files (text, length, file_name, date)"
             "values('%s', %d, '%s', '%s')",
             text, new->header->length, new->header->name, ctime(&current)
        );
        free(text);
           
        if(mysql_query(&mysql, query))
        {
            error_fail(&mysql);
            return EXIT_FAILURE;
        }
        mysql_close(&mysql);

        free(query);
    }
    freeStructure(new->header);
    free(new);

    fclose(fp);
    return EXIT_SUCCESS;
}
