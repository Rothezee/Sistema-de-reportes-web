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
    header('Content-Type: application/json');
    die(json_encode(["error" => "Connection failed: " . $conn->connect_error]));
}

// Verificar si se ha proporcionado un device_id
if (!isset($_GET['device_id'])) {
    header('Content-Type: application/json');
    die(json_encode(["error" => "device_id no proporcionado."]));
}

$device_id = $_GET['device_id'];

// Consulta para obtener los datos de la máquina de tickets
$sql = "SELECT id, device_id, dato2, dato5, timestamp FROM datos WHERE device_id = ?";
$stmt = $conn->prepare($sql);
if (!$stmt) {
    header('Content-Type: application/json');
    die(json_encode(["error" => "Prepare failed: " . $conn->error]));
}
$stmt->bind_param("s", $device_id);
$stmt->execute();
$result = $stmt->get_result();

$reports = [];
while ($row = $result->fetch_assoc()) {
    $reports[] = $row;
}

// Obtener los cierres diarios (puedes modificar esta lógica según tu necesidad)
$closes = [];
$close_sql = "SELECT timestamp FROM daily_closes WHERE device_id = ?";
$close_stmt = $conn->prepare($close_sql);
if ($close_stmt) {
    $close_stmt->bind_param("s", $device_id);
    $close_stmt->execute();
    $close_result = $close_stmt->get_result();
    while ($close_row = $close_result->fetch_assoc()) {
        $closes[] = $close_row['timestamp'];
    }
    $close_stmt->close();
}

$response = [
    "reports" => $reports,
    "closes" => $closes
];

header('Content-Type: application/json');
echo json_encode($response);

$stmt->close();
$conn->close();
?>
