<?php
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

// Obtener el cierre del día anterior
$cierre_date = date('Y-m-d', strtotime('yesterday'));

$sql = "SELECT device_id, SUM(coin) as total_coin FROM datos WHERE DATE(timestamp) = ? GROUP BY device_id";
$stmt = $conn->prepare($sql);
$stmt->bind_param("s", $cierre_date);
$stmt->execute();
$result = $stmt->get_result();

while ($row = $result->fetch_assoc()) {
    $device_id = $row['device_id'];
    $total_coin = $row['total_coin'];

    $insert_sql = "INSERT INTO cierres (device_id, coin, cierre_date) VALUES (?, ?, ?)";
    $insert_stmt = $conn->prepare($insert_sql);
    $insert_stmt->bind_param("sis", $device_id, $total_coin, $cierre_date);
    $insert_stmt->execute();
    $insert_stmt->close();
}

$stmt->close();
$conn->close();
?>
