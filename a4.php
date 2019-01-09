<html> <head> <title> Html file converter </title> </head> <body>
<?php
    function make_hyperlink ($str)
    {
        echo '<a href = "./files/' . $str . '" target = "_blank">' . 
            $str . '</a><br>';
    }
    if(is_dir("./files") == false)
    {
        mkdir("./files", 0775);
    }
?>

<form target = "_blank" action = "button.php">
    <input type = "submit" name = "submit" value = "Upload"/>
</form>

<div>
    <h3> Text files </h3> 
    <p> 
    <?php
        foreach(glob("./files/*") as $filename)
        {
            if(substr_compare($filename, ".html", -5, 5) and
                substr_compare($filename, ".info", -5, 5) and
                is_dir($filename) == false)
            {
                make_hyperlink(substr($filename, 8));
            }
        } 
    ?> 
    </p> 
</div>

<form target = "_blank" action = "select.php">
    <input type = "submit" name = "submit" value = "Convert"/>
</form>

<div>
    <h3> Html files </h3> 
    <p>
    <?php       
        exec('./display &', $output);
        
        foreach(glob("./files/*.html") as $elem)
        {
            $sys_files[] = substr($elem, 8);
        }
        
        $html_files = array_unique(array_merge($sys_files, $output));

        foreach($html_files as $link)
        {
            make_hyperlink($link);
        }

    ?> 
    </p> 
</div>

<form target = "_blank" action = "select_db.php">
    <input type = "submit" name = "submit" value = "DB Store"/>
</form>
</body> </html>
