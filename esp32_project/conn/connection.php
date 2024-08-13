<?php
$servername = "localhost";
$username = "root"; // Cambia esto si tienes un usuario diferente
$password = "39090169"; // Cambia esto si tienes una contraseña diferente
$dbname = "esp32_report"; // Cambia esto por el nombre de tu base de datos

// Crear la conexión
$conn = new mysqli($servername, $username, $password, $dbname);

// Verificar la conexión
if ($conn->connect_error) {
    die("Conexión fallida: " . $conn->connect_error);
}
?>
