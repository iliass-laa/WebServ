#!/usr/bin/php
<?php

header('Content-type: image/jpeg');//with header Content type 
echo file_get_contents("https://media.geeksforgeeks.org/wp-content/uploads/geeksforgeeks-6.png");

?>