<?php
$servername = "localhost";
$username = "root";
$password = "39090169";
$dbname = "esp32_report";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

if (!isset($_GET['device_id'])) {
    die("device_id is required");
}

$device_id = $conn->real_escape_string($_GET['device_id']);

// Obtener reportes
$sql = "SELECT id, timestamp, dato1, dato2, dato3, dato4 FROM datos WHERE device_id = '$device_id' ORDER BY timestamp DESC";
$result = $conn->query($sql);

$reportes = [];
if ($result->num_rows > 0) {
    while ($row = $result->fetch_assoc()) {
        $reportes[] = $row;
    }
}

// Obtener cierres diarios
$sql_closes = "SELECT close_date FROM daily_closes ORDER BY close_date ASC";
$result_closes = $conn->query($sql_closes);

$cierres = [];
if ($result_closes->num_rows > 0) {
    while ($row = $result_closes->fetch_assoc()) {
        $cierres[] = $row['close_date'];
    }
}

// Generar contenido HTML
$html = "<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <link rel='stylesheet' href='css/style.css'>
    <title>Information to Machine</title>
</head>
<body>
<header>
    <nav>
        <div class='navbar'>
            <div class='item'>
                <h1 id='machine_name'>Machine $device_id</h1>
            </div>
        </div>
    </nav>
</header>
<main>
    <div class='container'>
        <table id='report_table'>
            <thead>
                <tr>
                    <th>ID</th>
                    <th>Date and Time</th>
                    <th>PESOS</th>
                    <th>COIN</th>
                    <th>PREMIOS</th>
                    <th>BANCO</th>
                </tr>
            </thead>
            <tbody>";

foreach ($reportes as $reporte) {
    $fecha = date('Y-m-d H:i:s', strtotime($reporte['timestamp']));
    $html .= "<tr>
                <td>{$reporte['id']}</td>
                <td>{$fecha}</td>
                <td>{$reporte['dato1']}</td>
                <td>{$reporte['dato2']}</td>
                <td>{$reporte['dato3']}</td>
                <td>{$reporte['dato4']}</td>
              </tr>";
}

// Añadir fila de cierre diario
$fecha_cierre = date('Y-m-d H:i:s');
$html .= "<tr class='cierre-diario'>
            <td colspan='6'>Cierre Diario: $fecha_cierre</td>
          </tr>";

$html .= "</tbody>
        </table>
    </div>
</main>
<script>
    document.addEventListener('DOMContentLoaded', function() {
        const machineName = document.getElementById('machine_name');
        machineName.innerText = `Machine $device_id`;
    });
</script>
<style>
    .cierre-diario {
        color: red;
        font-weight: bold;
    }
</style>
</body>
</html>";

file_put_contents('C:\AppServ\www\esp32_project\report.html', $html);

$conn->close();
?>
