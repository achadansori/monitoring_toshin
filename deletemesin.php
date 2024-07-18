<!-- deletemesin.php -->

<?php
// Koneksi ke database
$dbhost = 'localhost';
$dbname = 'monitoring_toshin';
$dbuser = 'root';
$dbpass = 't';

$conn = new mysqli($dbhost, $dbuser, $dbpass, $dbname);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

// Cek apakah tombol "delete all" ditekan
if (isset($_POST['delete_all']) && $_POST['delete_all'] == 'true') {
    // Query untuk menghapus semua data dari tabel node chiller
    $sql = "DELETE FROM node_mesin";
    if ($conn->query($sql) === TRUE) {
        // Set auto_increment kembali ke 1
        $sql_reset_auto_increment = "ALTER TABLE node_mesin AUTO_INCREMENT = 1";
        $conn->query($sql_reset_auto_increment);
        echo "All records have been deleted successfully.";
    } else {
        echo "Error deleting all records: " . $conn->error;
    }
} else if (isset($_POST['selected'])) {
    // Jika hanya beberapa checkbox yang dipilih
    $selected = $_POST['selected'];
    foreach ($selected as $id) {
        // Query untuk menghapus data tertentu dari tabel node chiller
        $sql = "DELETE FROM node_chiller WHERE id=$id";
        if ($conn->query($sql) !== TRUE) {
            echo "Error deleting record with ID $id: " . $conn->error;
        }
    }
}

$conn->close();
// Redirect dilakukan di client-side setelah AJAX call
?>
