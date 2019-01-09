#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "listio.h"
#include "mysql/mysql.h"
#include <errno.h>

int main()
{
    MYSQL mysql;
    mysql_init(&mysql);
    mysql_options(&mysql, MYSQL_READ_DEFAULT_GROUP,  "mydb");
        
    if(!mysql_real_connect(&mysql, "dursley.socs.uoguelph.ca", "kspenc05",
        "0872780", "kspenc05", 0, NULL, 0) )
    {
        printf("%s\n", mysql_error(&mysql));
        return EXIT_FAILURE;
    }    

    if(mysql_query(&mysql, "Select * file_name from html_files"))
    {
        printf("%s\n", mysql_error(&mysql));
        return EXIT_FAILURE;
    }
    mysql_close(&mysql);

}
