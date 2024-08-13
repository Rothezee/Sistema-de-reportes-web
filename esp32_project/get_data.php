<?php
$servername = "localhost";
$username = "root";
$password = "39090169";
$dbname = "esp32_report";

try {
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // Verificar si se ha proporcionado el parámetro device_id como parámetro GET
    if (isset($_GET['device_id'])) {
        $device_id = $_GET['device_id'];

        // Obtener el último registro de la tabla de datos con las columnas correctas
        $stmt = $conn->prepare("SELECT dato1, dato2, dato3, dato4, dato5 FROM datos WHERE device_id = :device_id ORDER BY timestamp DESC LIMIT 1");
        $stmt->bindParam(':device_id', $device_id, PDO::PARAM_STR);
        $stmt->execute();
        $data = $stmt->fetch(PDO::FETCH_ASSOC);

        if (!$data) {
            // Si no se encuentran datos, lanzar una excepción
            throw new Exception("No data found for device_id: $device_id");
        }

        // Devolver los datos en formato JSON
        echo json_encode($data);
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
