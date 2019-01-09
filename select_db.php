
<html> <head> <title> Convert </title> </head> <body>
<?php
    foreach(glob("./files/*") as $filename)
    {
        if(substr_compare($filename, ".info", -5, 5) and 
            is_dir($filename) == false)
        {
            echo substr($filename, 8) . "<br>";
        }
    }
?>
<form action = "database.php"
    method = "post" enctype = "multipart/form-data">
    <input type = "text" value = "file" name = "file">
    <input type = "submit" value = "Convert File" name = "submit">
</form> 
</body> </html>
