<?php
// The Public IP addresses for these hooks are in 192.30.252.0/22.
$ip = $_SERVER['HTTP_X_FORWARDED_FOR'];
$ip = array_pop(explode(", ", $ip));
$ip = ip2long($ip);
$mask_22 = 0xFFFFFC00;
if (($ip & $mask_22) === ip2long('192.30.252.0'))
{
	$payload = $_POST['payload'];
	if (!$payload) die('no');
	system('git fetch 1>/dev/null 2>/dev/null; git stash 1>/dev/null; git checkout origin/master; cd ../doc; doxygen 1>/dev/null 2>/dev/null');
	echo 'ok';
}
else
{
	mail(base64_decode("ZGthbGlua2luQGJubC5nb3Y="), "unauthorized access", var_export($_SERVER, true));
	die('');
}
?>
