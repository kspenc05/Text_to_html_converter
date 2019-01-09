<?php
     exec('./a4 ./files/' . $_POST['file'] . ' &');
     chmod('./files/' . $_POST['file'], 0755);

     echo '<script>window.close()</script>';    
?>
