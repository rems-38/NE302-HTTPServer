<?php

// $arr = get_defined_vars();

// print_r($arr);
// echo "<br>";
?>

Bonjour, <?php echo htmlspecialchars($_GET['nom']); ?>.
Tu as <?php echo (int)$_GET['age']; ?> ans