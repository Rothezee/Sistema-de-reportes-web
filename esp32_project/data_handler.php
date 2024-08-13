<?php
// Configuración de la conexión a la base de datos
$servername = "localhost";
$username = "root";
$password = "39090169";
$dbname = "esp32_report";

try {
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Verificar si se ha proporcionado el nombre de la máquina como parámetro GET
    if (isset($_GET['machine'])) {
        $machine = $_GET['machine'];

        // Obtener el último registro de la tabla de datos
        $stmt = $conn->prepare("SELECT dato1 AS pesos, dato2 AS coin, dato3 AS premios, dato4 AS banco FROM datos WHERE device_id = :machine ORDER BY id DESC LIMIT 1");
        $stmt->bindParam(':machine', $machine, PDO::PARAM_STR);
        $stmt->execute();
        $data = $stmt->fetch(PDO::FETCH_ASSOC);

        if (!$data) {
            throw new Exception("No data found for machine: $machine");
        }

        // Obtener el estado de la máquina
        $stmt_heartbeat = $conn->prepare("SELECT last_heartbeat FROM devices WHERE device_id = :machine");
        $stmt_heartbeat->bindParam(':machine', $machine, PDO::PARAM_STR);
        $stmt_heartbeat->execute();
        $heartbeat = $stmt_heartbeat->fetch(PDO::FETCH_ASSOC);

        if ($heartbeat) {
            $last_heartbeat = strtotime($heartbeat['last_heartbeat']);
            $current_time = time();
            $is_online = ($current_time - $last_heartbeat) <= 300; // 5 minutos
            $data['status'] = $is_online ? 'online' : 'offline';
        } else {
            $data['status'] = 'unknown';
        }

        echo json_encode($data);
    } else {
        echo json_encode(["error" => "No machine parameter provided."]);
    }
} catch (PDOException $e) {
    echo json_encode(["error" => "Error: " . $e->getMessage()]);
} catch (Exception $e) {
    echo json_encode(["error" => $e->getMessage()]);
}
$conn = null;
?>
