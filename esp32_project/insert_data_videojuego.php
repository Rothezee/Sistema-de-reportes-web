<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

// Crear un archivo de registro de errores
$log_file = 'error_log.txt';
ini_set('log_errors', 1);
ini_set('error_log', $log_file);

$servername = "localhost";
$username = "root";
$password = "39090169";
$dbname = "esp32_report";

// Crear conexión
$conn = new mysqli($servername, $username, $password, $dbname);

// Verificar conexión
if ($conn->connect_error) {
    error_log("Connection failed: " . $conn->connect_error);
    die("Connection failed: " . $conn->connect_error);
}

// Obtener los datos POST
$data = json_decode(file_get_contents('php://input'), true);

// Verificar que se hayan recibido todos los datos necesarios
if (!isset($data['device_id']) || !isset($data['dato2'])) {
    error_log("Missing data");
    echo json_encode(["error" => "Missing data"]);
    $conn->close();
    exit();
}

$device_id = $data['device_id'];
$dato2 = $data['dato2'];

// Insertar datos específicos para máquinas de videojuegos
$sql = "INSERT INTO datos (device_id, dato1, dato2, dato3, dato4, dato5) VALUES (?, 0, ?, 0, 0, 0)";
$stmt = $conn->prepare($sql);
if (!$stmt) {
    error_log("Prepare failed: " . $conn->error);
    echo json_encode(["error" => "Prepare failed"]);
    $conn->close();
    exit();
}
$stmt->bind_param("si", $device_id, $dato2);

if ($stmt->execute()) {
    // Actualizar el last_heartbeat en la tabla devices
    $update_sql = "INSERT INTO devices (device_id, last_heartbeat) VALUES (?, NOW()) ON DUPLICATE KEY UPDATE last_heartbeat = NOW()";
    $update_stmt = $conn->prepare($update_sql);
    if (!$update_stmt) {
        error_log("Update prepare failed: " . $conn->error);
        echo json_encode(["error" => "Update prepare failed"]);
        $conn->close();
        exit();
    }
    $update_stmt->bind_param("s", $device_id);
    $update_stmt->execute();

    echo json_encode(["success" => "Data inserted successfully"]);
} else {
    error_log("Execute failed: " . $stmt->error);
    echo json_encode(["error" => "Error inserting data"]);
}

$stmt->close();
$conn->close();
?>
