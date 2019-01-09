<?php 
    if(file_exists($_FILES['filename']['tmp_name'])) 
    {
        move_uploaded_file($_FILES['filename']['tmp_name'], 
            __DIR__ . "/files/" . $_FILES['filename']['name']);

        chmod(__DIR__ . "/files/" . $_FILES['filename']['name'], 0755);
    }
    echo '<script>window.close()</script>';
?>