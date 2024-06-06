<?php

// $arr = get_defined_vars();

// print_r($arr);
// echo "<br>";
?>

Bonjour, <?php echo htmlspecialchars($_POST['nom']); ?>.
Tu as <?php echo (int)$_POST['age']; ?> ans