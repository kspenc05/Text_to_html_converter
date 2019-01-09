#include "mysql/mysql.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

void print_line()
{
    for(int i = 0; i < 20; i++)
        printf("*");
    printf("\n");
}

void error(MYSQL * db)
{
    printf("%s\n", mysql_error(db));
}

int main(int argc, char ** argv)
{
    MYSQL db;
    MYSQL_RES * res;
    MYSQL_ROW row;

    if(argc < 2 || argv[1][0] != '-')
    {
        return 0;  
    } 

    mysql_init(&db);
    mysql_options(&db, MYSQL_READ_DEFAULT_GROUP,  "mydb");

    if(!mysql_real_connect(&db, "dursley.socs.uoguelph.ca", "kspenc05",
        "0872780", "kspenc05", 0, NULL, 0) )     
    {
        error(&db);
        return EXIT_FAILURE;
    }
    
    switch(argv[1][1]) //check second letter of flag typed
    {
        case 's': case 'S': //is user enters "show"
        {
            if(mysql_query(&db, "select file_name, length, date from html_files"))
            {
                error(&db);
                return EXIT_FAILURE;
            }
            
            if(!(res = mysql_store_result(&db)))
            {
                error(&db);
                return EXIT_FAILURE;
            }

            while ((row = mysql_fetch_row(res))) 
            {
                for (int i=0; i < mysql_num_fields(res); i++)
                {
                    printf("%s ", row[i]);
                }
            }
            break;
        }
    
        case 'r': case 'R': //if user enters "-reset"
        {
            if(mysql_query(&db, "drop table if exists html_files"))
            {
                error(&db);
                return EXIT_FAILURE;
            }
            break;
        }

        case 'c': case 'C': //if user enters "-clear" as flag, check for 'c'
        {
            if(mysql_query(&db, "truncate html_files"))
            {
                error(&db);
                return EXIT_FAILURE;
            }
            break;
        }
        case 'h': case 'H':
        {
            print_line();
            printf("\nHelp screen: \"What do the command line flags do?\"\n\n"
                "\"-show\": display all table entries\n"
                "\"-clear\": delete all entries in table\n"
                "\"-reset\": delete current table and all entries in it\n"
                "\"-help\": show this screen again\n\n"

                "NOTE: these commands do nothing (except \"help\"),\n"
                "unless a table exists.\n\n"
                "Whenever you store a file, it creates a new table\n"
                "to store that file, if one does not already exist.\n\n"
            );
            print_line();
        }
    }
    mysql_close(&db);
    return 0;
}
