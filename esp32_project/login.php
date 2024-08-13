<?php
include('config.php');
session_start();

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $username = $_POST['username'];
    $password = $_POST['password'];
    
    $stmt = $conn->prepare("SELECT id, password FROM users WHERE username = ?");
    $stmt->bind_param("s", $username);
    $stmt->execute();
    $stmt->store_result();
    
    if ($stmt->num_rows > 0) {
        $stmt->bind_result($id, $hashed_password);
        $stmt->fetch();
        
        if (password_verify($password, $hashed_password)) {
            $_SESSION['username'] = $username;
            header("Location: dashboard.php");
        } else {
            echo "Invalid credentials";
        }
    } else {
        echo "No user found";
    }
    
    $stmt->close();
}
?>

<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="css/style.css">    
    <title>Login</title>    
</head>
<body>
    <div class="container-login">
        <div class="login-container">
            <h2>Login</h2>
            <form method="post" action="">
                <div class="input-group">
                    Username: <input type="text" name="username" required><br>
                </div>
                <div class="input-group">
                    Password: <input type="password" name="password" required><br>
                </div>
                <button type="submit">Login</button>
            </form>
        </div>
    </div>
    <script src="script/main.js"></script>
</body>
</html>
