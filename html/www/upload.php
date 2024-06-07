<?php

$arr = get_defined_vars();

print_r($arr);
echo "<br>";

if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_FILES['image'])) {
    $errors = [];
    $file_name = $_FILES['image']['name'];
    $file_size = $_FILES['image']['size'];
    $file_tmp = $_FILES['image']['tmp_name'];
    $file_type = $_FILES['image']['type'];
    $file_ext = strtolower(end(explode('.', $_FILES['image']['name'])));

    $extensions = ["jpeg", "jpg", "png", "gif"];

    if (in_array($file_ext, $extensions) === false) {
        $errors[] = "Extension non autorisée, veuillez choisir un fichier JPEG, PNG ou GIF.";
    }

    if ($file_size > 2097152) {
        $errors[] = 'La taille du fichier ne doit pas dépasser 2 Mo';
    }

    if (empty($errors) == true) {
        move_uploaded_file($file_tmp, "uploads/" . $file_name);
        echo "L'image a été téléchargée avec succès.";
    } else {
        foreach ($errors as $error) {
            echo $error . "<br>";
        }
    }
}
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Upload d'image</title>
</head>
<body>
    <form action="" method="POST" enctype="multipart/form-data">
        <input type="file" name="image" />
        <input type="submit" />
    </form>
</body>
</html>