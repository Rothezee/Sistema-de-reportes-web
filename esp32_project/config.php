<?php
$host = "localhost";
$user = "root";
$password = "39090169";
$database = "login_system";

$conn = new mysqli($host, $user, $password, $database);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}
?>
