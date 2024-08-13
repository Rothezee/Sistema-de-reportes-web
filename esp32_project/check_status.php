<?php
$servername = "localhost";
$username = "root";
$password = "39090169";
$dbname = "esp32_report";

// Configurar la zona horaria del servidor
date_default_timezone_set('America/Argentina/Buenos_Aires');

try {
    // Crear una nueva conexión PDO
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Verificar si se ha proporcionado el parámetro device_id como parámetro GET
    if (isset($_GET['device_id'])) {
        $device_id = $_GET['device_id'];

        // Obtener el estado de la máquina
        $stmt = $conn->prepare("SELECT last_heartbeat FROM devices WHERE device_id = :device_id");
        $stmt->bindParam(':device_id', $device_id, PDO::PARAM_STR);
        $stmt->execute();
        $heartbeat = $stmt->fetch(PDO::FETCH_ASSOC);

        if ($heartbeat) {
            $last_heartbeat = strtotime($heartbeat['last_heartbeat']);
            $current_time = time();

            // Depuración: imprimir los valores de last_heartbeat y current_time
            error_log("last_heartbeat: " . date('Y-m-d H:i:s', $last_heartbeat));
            error_log("current_time: " . date('Y-m-d H:i:s', $current_time));
            error_log("difference: " . ($current_time - $last_heartbeat));

            // Evaluar si la última señal fue en los últimos 60 segundos
            $is_online = ($current_time - $last_heartbeat) <= 60;
            $status = $is_online ? 'online' : 'offline';
        } else {
            $status = 'unknown';
        }

        // Devolver el estado en formato JSON
        echo json_encode(["status" => $status]);
    } else {
        // Si no se proporciona device_id, devolver un error en formato JSON
        echo json_encode(["error" => "No device_id parameter provided."]);
    }
} catch (PDOException $e) {
    // Manejo de errores de PDO
    echo json_encode(["error" => "Error: " . $e->getMessage()]);
} catch (Exception $e) {
    // Manejo de otras excepciones
    echo json_encode(["error" => $e->getMessage()]);
}

// Cerrar la conexión
$conn = null;
?>
