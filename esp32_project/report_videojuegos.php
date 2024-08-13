<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

$servername = "localhost";
$username = "root";
$password = "39090169";
$dbname = "esp32_report";

// Crear conexión
$conn = new mysqli($servername, $username, $password, $dbname);

// Verificar conexión
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Verificar si se ha proporcionado un device_id
if (!isset($_GET['device_id'])) {
    die("device_id no proporcionado.");
}

$device_id = $_GET['device_id'];

// Consulta para obtener los datos de la máquina de videojuegos
$sql = "SELECT id, device_id, dato2, timestamp FROM datos WHERE device_id = ?";
$stmt = $conn->prepare($sql);
if (!$stmt) {
    die("Prepare failed: " . $conn->error);
}
$stmt->bind_param("s", $device_id);
$stmt->execute();
$result = $stmt->get_result();

if ($result->num_rows > 0) {
    echo "<h2>Reporte de la máquina de videojuegos: $device_id</h2>";
    echo "<table border='1'>
            <tr>
                <th>ID</th>
                <th>Device ID</th>
                <th>Coin</th>
                <th>Timestamp</th>
            </tr>";
    while ($row = $result->fetch_assoc()) {
        echo "<tr>
                <td>" . $row['id'] . "</td>
                <td>" . $row['device_id'] . "</td>
                <td>" . $row['dato2'] . "</td>
                <td>" . $row['timestamp'] . "</td>
              </tr>";
    }
    echo "</table>";
} else {
    echo "No se encontraron registros para la máquina de videojuegos: $device_id";
}

$stmt->close();
$conn->close();
?>
