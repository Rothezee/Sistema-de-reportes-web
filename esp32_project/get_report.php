<?php
header('Content-Type: application/json');

$servername = "localhost";
$username = "root";
$password = "39090169";
$dbname = "esp32_report";

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die(json_encode(["error" => "Connection failed: " . $conn->connect_error]));
}

if (!isset($_GET['device_id'])) {
    die(json_encode(["error" => "device_id is required"]));
}

$device_id = $conn->real_escape_string($_GET['device_id']);

// Retrieve reports
$sql = "SELECT id, timestamp, dato1, dato2, dato3, dato4 FROM datos WHERE device_id = '$device_id' ORDER BY timestamp DESC";
$result = $conn->query($sql);

if (!$result) {
    die(json_encode(["error" => "Error executing query: " . $conn->error]));
}

$reports = [];

if ($result->num_rows > 0) {
    while($row = $result->fetch_assoc()) {
        $reports[] = $row;
    }
}

// Retrieve daily closes
$sql_closes = "SELECT close_date FROM daily_closes ORDER BY close_date ASC";
$result_closes = $conn->query($sql_closes);

if (!$result_closes) {
    die(json_encode(["error" => "Error executing query: " . $conn->error]));
}

$closes = [];

if ($result_closes->num_rows > 0) {
    while($row = $result_closes->fetch_assoc()) {
        $closes[] = $row['close_date'];
    }
}

echo json_encode(["reports" => $reports, "closes" => $closes]);

$conn->close();
?>
